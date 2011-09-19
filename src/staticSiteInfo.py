#!/s/python-2.7.1/bin/python -O

from binascii import unhexlify
from ctypes import *

__all__ = ['Site', 'Unit', 'StaticSiteInfo']


def once(method):
    """Invoke the method just once, then reuse that result in the future."""

    # attribute name used to cache the result
    attrname = '__once_%s' % id(method)

    def decorated(self, *args, **kwargs):
        try:
            # perhaps we already have the cached result
            return getattr(self, attrname)
        except AttributeError:
            # nope, this must be the first call
            result = method(self, *args, **kwargs)
            setattr(self, attrname, result)
            return result

    # preserve documentation from original method
    decorated.__doc__ = method.__doc__
    return decorated


class Site(Structure):

    """Static information about a single instrumentation site."""

    _fields_ = [
        ('file', c_char_p),
        ('line', c_int),
        ('fun', c_char_p),
        ('cfg_node', c_int),
        ('scheme_code', c_char),
        ('args', c_char_p * 2),
        ]


class Unit(Structure):

    """Static information about a single compiliation unit."""

    _fields_ = [
        ('scheme_code', c_char),
        ('__signature', c_char_p),
        ('__num_sites', c_uint),
        ('__sites', POINTER(Site * 0)),
        ]

    @property
    @once
    def signature(self):
        """Structure/Union member"""
        digits = getattr(self, '__signature')
        return unhexlify(digits)

    @property
    @once
    def sites(self):
        """Structure/Union member"""
        length = getattr(self, '__num_sites')
        start = getattr(self, '__sites')
        atype = POINTER(Site * length)
        aptr = cast(start, atype)
        return aptr.contents


class StaticSiteInfo(object):

    """Static information about a single compiled program."""

    def __init__(self, soname):
        """
        Read static site information from the named shared object.

        Typically `soname` will be the path to the
        \"static-site-info.so\" file in some cbiexp analysis
        subdirectory.
        """
        self.__dso = CDLL(soname)

    def __array(self, element, count, start):
        count = c_uint.in_dll(self.__dso, count)
        atype = element * count.value
        adata = atype.in_dll(self.__dso, start)
        return adata

    @property
    @once
    def units(self):
        """Compilation units within this program."""
        return self.__array(Unit, 'NumUnits', 'units')

    @property
    @once
    def sites(self):
        """
        Instrumentation sites within this program.

        This represents sites across all compilation units.  Use
        `Unit.sites` to access the sites found in single compilation
        units.
        """
        return self.__array(Site, 'NumSites', 'sites')


########################################################################
##
##  tester and usage example
##


def main():
    """Tester and basic usage example."""

    from binascii import hexlify
    from sys import argv, exit, stderr

    if len(argv) != 2:
        print >>stderr, 'Usage: %s </path/to/static-site-info.so>' % argv[0]
        exit(2)

    [soname] = argv[1:]
    info = StaticSiteInfo(soname)

    def siteStr(site):
        location = 'site at %s:%d in %s()' % (site.file, site.line, site.fun)
        details = '%d, %s, [%s, %s]' % (site.cfg_node, site.scheme_code, site.args[0], site.args[1])
        return '%s: %s' % (location, details)

    print 'iterating by compilation unit, then by site within each unit:'
    for unit in info.units:
        print '  unit %s %s:' % (unit.scheme_code, hexlify(unit.signature))
        for site in unit.sites:
            print '    %s' % siteStr(site)


if __name__ == '__main__':
    main()
