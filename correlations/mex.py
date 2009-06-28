from SCons.Script import *


mex_builder = Builder(
    single_source=True,
    src_suffix='mexglx',
    suffix='c',
    action='$mex $SOURCE',
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
