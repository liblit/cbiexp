from SCons.Script import *


########################################################################


def check_nonempty(target, source, env):
    for needed in target:
        if needed.getsize() == 0:
            return '"%s" is empty' % needed


extract_action = Action([
        '$extract_section $section $SOURCE >$TARGET',
        check_nonempty,
        ])


def extract_emitter(target, source, env):
    [target] = target
    target = [File(target.name, 'share')]
    return target, source


extract_sites_builder = Builder(
    emitter=extract_emitter,
    single_source=True,
    action=extract_action,
    section='.debug_site_info',
    suffix='sites',
    )


extract_implications_builder = Builder(
    single_source=True,
    action=extract_action,
    section='.debug_sampler_implications',
    suffix='implications',
    )


########################################################################


def generate(env):
    env.AppendUnique(
        BUILDERS = {
            'ExtractImplications': extract_implications_builder,
            'ExtractSites': extract_sites_builder,
            },
        )


def exists(env):
    return env['$extract_section']
