from SCons.Script import *
from subprocess import PIPE, Popen


def ReadPipe(self, command):
    command = map(str, command)
    if not self.GetOption('silent'):
        print ' '.join(command)
    process = Popen(command, env=self['ENV'], stdout=PIPE)

    for line in process.stdout:
        yield line

    status = process.wait()
    if status != 0:
        raise OSError(
            errstr='pipe failed',
            status=status,
            exitstatus=status,
            command=command,
            )


def generate(env):
    env.AddMethod(ReadPipe)

def exists(env):
    return True
