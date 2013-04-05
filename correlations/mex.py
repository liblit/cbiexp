from platform import architecture
from SCons.Script import *


mex_builder = Builder(
    single_source=True,
    src_suffix='c',
    suffix={'64bit': 'mexa64', '32bit': 'mexglx'}[architecture()[0]],
    action='$mex $mex_flags -output $TARGET $SOURCE',
)


########################################################################


def generate(env):
    env.SetDefault(mex=env.WhereIs('mex'))
    env.AppendUnique(
        BUILDERS = {
            'Mex': mex_builder,
            },
        )


def exists(env):
    return env.get('mex') or env.WhereIs('mex')
