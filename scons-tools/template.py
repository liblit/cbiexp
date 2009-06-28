import os

from shutil import copymode
from stat import S_IMODE
from string import Template
from sys import stderr

from SCons.Action import Action
from SCons.Builder import Builder
from SCons.Script import Exit


########################################################################


class AtTemplate(Template):

    delimiter = '@'

    pattern = r"""
    (?:
    (?P<escaped>@@) |
    (?P<named>(?!)) |
    @(?P<braced>%(id)s)@ |
    (?P<invalid>(?!))
    )
    """ % {'id': Template.idpattern}


def Instantiate(self, source, target, **kwargs):
    source = open(str(source))
    target = open(str(target), 'w')
    for line in source:
        target.write(AtTemplate(line).substitute(kwargs))
    target.close()
    source.close()


########################################################################


def __instantiate_exec(target, source, env):
    varlist = env['varlist']

    missing = [key for key in varlist if not env.has_key(key)]
    if missing:
        print >>stderr, 'error: missing variables for template instantiation:', ', '.join(missing)
        Exit(1)

    keywords = dict((key, env.subst('$' + key)) for key in varlist)
    Instantiate(env, source[0], target[0], **keywords)

def __instantiate_show(target, source, env):
    __pychecker__ = 'no-argsused'
    return 'instantiate "%s" as "%s"' % (source[0], target[0])


def __chmod_copy_exec(target, source, env):
    [target] = target
    [source] = source
    copymode(str(source), str(target))

def __chmod_copy_show(target, source, env):
    __pychecker__ = 'no-argsused'
    return 'copy file mode from "%s" to "%s"' % (source[0], target[0])

__chmod_copy = Action(__chmod_copy_exec, __chmod_copy_show)


def __generator(source, target, env, for_signature):
    __pychecker__ = 'no-argsused'
    varlist = env['varlist']
    actions = [Action(__instantiate_exec, __instantiate_show, varlist=varlist)]
    if env['template_copy_mode']:
        actions.append(__chmod_copy)
    return actions


__template_builder = Builder(
    generator=__generator,
    src_suffix=['.in'],
    single_source=True,
    )


def generate(env):
    env.AddMethod(Instantiate)
    env.AppendUnique(BUILDERS={'Template': __template_builder})
    env.SetDefault(template_copy_mode=True)

def exists(env):
    return True
