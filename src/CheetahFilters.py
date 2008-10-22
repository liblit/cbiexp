# Filters for Cheetah templates 

from Cheetah.Filters import Filter

class CppFilter(Filter):
    """Convert strings for C++ source code"""

    def filter(self, val, **kw):
        """converts val to a C++ compatible string, escaping special chars"""

        def handleString(val):

            # escape backslashes first
            val = val.replace('\\', '\\\\')

            # _then_ escape double quotes
            val = val.replace('"', '\\"')

            # finally, put quotation marks on each end
            val = '"' + val +'"'

            return val

        if 'format' in kw:

            if kw['format'] == 'string':

                return handleString(val)
 
            if kw['format'] == 'char':

                return "'" + val + "'"
 
            if kw['format'] == 'stringlist':

                return ' ,'.join(map(lambda x : handleString(x), val))

        else:
            return Filter.filter(self, val, **kw)

        return val


# any class defined above should have an entry here
filters = {
  'CppFilter' : CppFilter
}

def factory(name):
    """function to look up classes by name"""
    try:
        return filters[name]
    except KeyError:
        raise ValueError('Unknown filter name %s' % name)

