from itertools import imap
from SCons.Script import *


########################################################################


makefile_action = Action(
    'CONFIG_SITE=$config_site.abspath $SOURCE.abspath/configure $configure_flags',
    chdir=True,
    source_scanner=DirScanner,
    )


def makefile_emitter(target, source, env):
    build = target[0].dir
    filenames = ['config.h', 'config.log', 'config.status', 'stamp-h1']
    target += imap(build.File, filenames)
    config = env.get('config_site')
    if config: source.append(config)
    return target, source


makefile_builder = Builder(
    emitter=makefile_emitter,
    action=makefile_action,
    )


########################################################################


def generate(env):
    env.AppendUnique(
        BUILDERS = {
            'Makefile': makefile_builder,
            },
        )


def exists(env):
    return True
