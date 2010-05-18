from itertools import imap
from string import Template
from subprocess import PIPE, Popen

from SCons.Action import Action

import sys
sys.path[1:1] = ['/usr/lib/scons']


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


def instantiate(source, sink, **kwargs):
    source = file(source)
    sink = file(sink, 'w')
    for line in source:
        sink.write(AtTemplate(line).substitute(kwargs))
    sink.close()


def read_pipe(command, env):
    [command] = env.subst_list(command)
    command = map(str, command)
    print ' '.join(command)
    process = Popen(command, env=env['ENV'], stdout=PIPE)

    for line in process.stdout:
        yield line

    status = process.wait()
    if status != 0:
        env.Exit(status)


def __literal_exec(target, source, env):
    __pychecker__ = 'no-argsused'
    target = file(str(target[0]), 'w')
    print >>target, source[0].get_contents()
    target.close()

def __literal_show(target, source, env):
    return 'create "%s" containing %s' % (target[0], source[0])

__literal_action = Action(__literal_exec, __literal_show)


def literal(env, target, value):
    return env.Command(target, env.Value(value), __literal_action)
