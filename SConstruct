SetOption('implicit_cache', True)


def validate_sampler_build(key, val, env):
    if val:
        PathVariable.PathIsDir(key, val, env)

def validate_csurf(key, value, env):
    if not value: return
    PathVariable.PathIsFile(key, value, env)


saved_vars = 'scons-config.py'
vars = Variables(saved_vars, ARGUMENTS)
vars.AddVariables(
    PathVariable('sampler_prefix', 'use sampler tools installed under this directory', '/usr/local', PathVariable.PathIsDir),
    PathVariable('sampler_build', 'use sampler tools from build tree in this directory', None, validate_sampler_build),
    BoolVariable('debug', 'compile with debugging information', False),
    BoolVariable('optimize', 'compile with optimization', True),
    PathVariable('csurf', 'path to CodeSurfer executable', WhereIs('csurf'), validate_csurf),
    )

for item in vars.UnknownVariables():
    print 'unknown variable: %s=%s' % item

env = Environment(
    variables=vars,
    __ccflags_debug='${("", "-g")[debug]}',
    __ccflags_optimize='${("", "-O")[optimize]}',
    CCFLAGS=['$__ccflags_debug', '$__ccflags_optimize', '-Werror', '-Wall', '-Wextra', '-Wformat=2'],
    CXXFLAGS=['-Wno-deprecated'],
    LIBPATH=Dir('.'),
    tools=['ar', 'c++', 'cc', 'link', 'template'],
    toolpath=['scons-tools'],
    )

Help(vars.GenerateHelpText(env))
vars.Save(saved_vars, env)

cbienv = env.Clone(
    CC='$sampler_cc',
    )


########################################################################


if cbienv.get('sampler_build'):
    cbienv['sampler_cc'] = cbienv.WhereIs('sampler-cc-here', '$sampler_build/driver')
    cbienv['extract_section'] = cbienv.WhereIs('extract-section', '$sampler_build/tools')
else:
    cbienv['sampler_cc'] = cbienv.WhereIs('sampler-cc-here', '$sampler_prefix/bin')
    cbienv['extract_section'] = cbienv.WhereIs('extract-section', '$sampler_prefix/lib/sampler/tools')


cbienv.Tool('autotools', toolpath='.')
cbienv.Tool('extract', toolpath='.')


sconf = env.Configure(config_h='src/config.h', clean=False, help=False)
for header in ['argp', 'dlfcn', 'unistd']:
    sconf.CheckCXXHeader('%s.h' % header)
sconf.Finish()


########################################################################


SConscript(
    dirs=[
        'bc',
        'ccrypt',
        'correlations',
        'exif',
        'sanity',
        'src',
        ],
    exports=['cbienv', 'env'],
    )
