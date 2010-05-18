from itertools import chain, ifilter, imap
from os.path import dirname
from subprocess import PIPE, Popen

import sys
sys.path[1:1] = ['/usr/lib/scons']
from SCons.Action import Action
from SCons.Builder import Builder
from SCons.Node.FS import File
from SCons.Scanner import Scanner


########################################################################


__source_to_object = {
    False: {'.mli': '.cmi', '.ml': '.cmo'},
    True:  {'.mli': '.cmi', '.ml': '.cmx'}
    }
__object_to_source = { '.cmi': '.mli', '.cmo': '.ml', '.cmx': '.ml' }


def __warn(message):
    print >>sys.stderr, 'scons-ocaml: warning:', message


########################################################################
#
#  ocamldep dependency scanner
#
#  This scanner runs ocamldep on ".ml" or ".mli" files.  It parses
#  ocamldep's output to idenfiy other modules that the scanned file
#  depends upon.
#


def __read_pipe(command, env):
    [command] = env.subst_list(command)
    command = map(str, command)
    print ' '.join(command)
    process = Popen(command, env=env['ENV'], stdout=PIPE)

    for line in process.stdout:
        yield line

    status = process.wait()
    if status != 0:
        env.Exit(status)


def __ocamldep_scan(node, env, path):
    __pychecker__ = 'no-argsused'
    suffix = node.get_suffix()
    if not suffix in __source_to_object[env['OCAML_NATIVE']]:
        __warn('%s does not have a suitable suffix' % node)
        return

    if not node.exists():
        __warn('%s does not exist' % node)

    target = node.target_from_source('', __source_to_object[env['OCAML_NATIVE']][suffix])
    target = str(target) + ':'

    def joinLines(stream):
        """reassemble long lines that were split using backslashed newlines"""
        accum = ''
        for line in stream:
            if line[-2:] == '\\\n':
                accum += line[0:-2]
            else:
                yield accum + line
                accum = ''

    deps = __read_pipe(['$OCAMLDEP', '$_OCAML_PATH', '$_OCAML_PP', str(node)], env)
    deps = joinLines(deps)
    deps = imap(str.split, deps)
    deps = ( fields[1:] for fields in deps if fields[0] == target )
    deps = chain(*deps)
    deps = imap(env.File, deps)
    return deps


__ocamldep_scanner = Scanner(
    function=__ocamldep_scan,
    name='ocamldep_scanner',
    node_class=File,
    skeys=['.mli', '.ml'],
    )


########################################################################
#
#  Object file builder
#


__lex_action = Action([['$OCAMLLEX', '-o', '$TARGET', '$SOURCE']])


__lex_builder = Builder(
    src_suffix='.mll',
    single_source=True,
    action=__lex_action,
    suffix='.ml',
    )


########################################################################
#
#  Object file builder
#


def __obj_emitter(target, source, env):
    if env['OCAML_DTYPES'] or env['OCAML_NATIVE']:
        cmo_suffix = __source_to_object[env['OCAML_NATIVE']]['.ml']
        cmos = ( node for node in target if node.get_suffix() == cmo_suffix )
        for cmo in cmos:
            if env['OCAML_DTYPES']:
                target.append(cmo.target_from_source('', '.annot'))
            if env['OCAML_NATIVE']:
                target.append(cmo.target_from_source('', '.o'))
    target = [ node.disambiguate() for node in target ]
    return target, source


def __obj_suffix(env, sources):
    [source] = sources
    result = __source_to_object[env['OCAML_NATIVE']][source.get_suffix()]
    return result


__obj_action = Action([['$_OCAMLC', '$_OCAML_DEBUG', '$_OCAML_DTYPES', '$_OCAML_PATH', '$_OCAML_PP', '$_OCAML_WARN', '$_OCAML_WARN_ERROR', '-o', '$TARGET', '-c', '$SOURCE']])


__obj_builder = Builder(
    source_scanner=__ocamldep_scanner,
    emitter=__obj_emitter,
    action=__obj_action,
    src_suffix=['.mli', '.ml'],
    suffix=__obj_suffix,
    single_source=True,
    )


########################################################################
#
#  Library archive builder
#
#  Building a library is fairly straightforward: run a command on a
#  list of sources to produce the target.  The only subtlety is that
#  native libraries have an extra depepdency: the ".a" file for each
#  ".cmxa" file.  We tell SCons about that using an emitter function.
#


__lib_suffix = { False: '.cma', True: '.cmxa' }


def __lib_emitter(target, source, env):
    if env['OCAML_NATIVE']:
        afiles = ( node.target_from_source('', '.a') for node in target )
        target += afiles
    return target, source


__lib_action = Action([['$_OCAMLC', '-a', '$_OCAML_WARN', '$_OCAML_WARN_ERROR', '-o', '$TARGET', '$SOURCES']])


__lib_builder = Builder(
    emitter=__lib_emitter,
    action=__lib_action,
    src_builder=__obj_builder,
    src_suffix='$OCAML_CMO',
    suffix='$OCAML_CMA',
    )


########################################################################
#
#  Executable builder
#


def __exe_depends(node, env):
    obj_suffix = node.get_suffix()
    if not obj_suffix in __object_to_source: return []
    src_suffix = __object_to_source[obj_suffix]
    source = node.target_from_source('', src_suffix)
    cmis = ( cmi for cmi in node.children() if cmi != source )
    cmos = ( cmi.target_from_source('', obj_suffix) for cmi in cmis )
    cmos = ( env.FindFile(str(cmo), '#.') for cmo in cmos )
    cmos = ifilter(None, cmos)
    if env['OCAML_NATIVE']:
        deps = ( (cmo, cmo.target_from_source('', '.o')) for cmo in cmos )
        return chain(*deps)
    else:
        return cmos


def __exe_scan(node, env, path):
    __pychecker__ = 'no-argsused'
    return list(__exe_depends(node, env))


__exe_scanner = Scanner(
    function=__exe_scan,
    name='exe_scanner',
    node_class=File,
    recursive=True,
    )


def __find_libs(libs, env, path):
    path = list(path)
    for lib in libs:
        found = env.FindFile(lib, path)
        if found:
            yield found
        else:
            __warn('no %s in search path [%s]' % (lib, ', '.join(path)))


def __exe_target_scan(node, env, path):
    __pychecker__ = 'no-argsused'
    suffix = __lib_suffix[env['OCAML_NATIVE']]
    libs = ( lib + suffix for lib in env['OCAML_LIBS'] )
    libs = __find_libs(libs, env, path)
    libs = list(libs)
    return libs


def __exe_path_function(env, directory, target=None, source=None):
    __pychecker__ = 'no-argsused'
    [paths] = env.subst_list(['$OCAML_PATH', '$OCAML_STDLIB'])
    return tuple(paths)


__exe_target_scanner = Scanner(
    function=__exe_target_scan,
    name='exe_target_scanner',
    path_function=__exe_path_function,
    node_class=File,
    )


__exe_action = Action([['$_OCAMLC', '$_OCAML_PATH', '-o', '$TARGET', '$_OCAML_LIBS', '$_OCAML_LINK_ORDER']])


__exe_builder = Builder(
    target_scanner=__exe_target_scanner,
    source_scanner=__exe_scanner,
    action=__exe_action,
    src_builder=__obj_builder,
    src_suffix='$_OCAML_CMO',
    suffix='$PROGSUFFIX',
    )


########################################################################
#
#  Construction-variable substitution functions
#


def __var_ocamlc(target, source, env, for_signature):
    __pychecker__ = 'no-argsused'
    compilers = {False: '$OCAMLC', True: '$OCAMLOPT'}
    return compilers[env['OCAML_NATIVE']]


def __var_ocaml_cma(target, source, env, for_signature):
    __pychecker__ = 'no-argsused'
    native = env['OCAML_NATIVE']
    return __lib_suffix[native]


def __var_ocaml_cmo(target, source, env, for_signature):
    __pychecker__ = 'no-argsused'
    native = env['OCAML_NATIVE']
    return __source_to_object[native]['.ml']


def __var_ocaml_debug(target, source, env, for_signature):
    __pychecker__ = 'no-argsused'
    if env['OCAML_DEBUG']:
        return '-g'


def __var_ocaml_dtypes(target, source, env, for_signature):
    __pychecker__ = 'no-argsused'
    if env['OCAML_DTYPES']:
        return '-dtypes'


def __var_ocaml_link_order(target, source, env, for_signature):
    __pychecker__ = 'no-argsused'

    def __link_order_expand(cmo, env, seen):
        order = []
        obj_suffix = cmo.get_suffix()
        if not cmo in seen:
            prereqs = __exe_depends(cmo, env)
            prereqs = ( pre.target_from_source('', obj_suffix) for pre in prereqs )
            prereqs = ( pre for pre in prereqs if pre != cmo )
            prereqs = ( __link_order_expand(pre, env, seen) for pre in prereqs )
            prereqs = chain(*prereqs)
            order += prereqs
            order.append(cmo)
            seen.add(cmo)
        return order

    [source] = source
    return __link_order_expand(source, env, set([]))


def __var_ocaml_stdlib(target, source, env, for_signature):
    ocamlc = env['OCAMLC']
    ocamlc = env.WhereIs(ocamlc)
    prefix = dirname(dirname(ocamlc))
    stdlib = '%s/lib/ocaml' % prefix
    return stdlib


########################################################################
#
#  SCons tool initialization
#


def generate(env):
    env.AppendUnique(
        OCAMLC=env.Detect(['ocamlc.opt', 'ocamlc']),
        OCAMLDEP=env.Detect(['ocamldep.opt', 'ocamldep']),
        OCAMLLEX=env.Detect(['ocamllex.opt', 'ocamllex']),
        OCAMLOPT=env.Detect(['ocamlopt.opt', 'ocamlopt']),
        OCAML_CMA=__var_ocaml_cma,
        OCAML_CMO=__var_ocaml_cmo,
        OCAML_DEBUG=False,
        OCAML_DTYPES=False,
        OCAML_LIBS=[],
        OCAML_NATIVE=False,
        OCAML_PATH=[],
        OCAML_PP='',
        OCAML_STDLIB=__var_ocaml_stdlib,
        OCAML_WARN='',
        OCAML_WARN_ERROR='',
        _OCAMLC=__var_ocamlc,
        _OCAML_DEBUG=__var_ocaml_debug,
        _OCAML_DTYPES=__var_ocaml_dtypes,
        _OCAML_LIBS='${_concat("", OCAML_LIBS, _OCAML_CMA, __env__)}',
        _OCAML_LINK_ORDER=__var_ocaml_link_order,
        _OCAML_PATH='${_concat("-I ", OCAML_PATH, "", __env__)}',
        _OCAML_PP='${_concat("-pp ", OCAML_PP, "", __env__)}',
        _OCAML_WARN='${_concat("-w ", OCAML_WARN, "", __env__)}',
        _OCAML_WARN_ERROR='${_concat("-warn-error ", OCAML_WARN_ERROR, "", __env__)}',

        #SCANNERS=[__ocamldep_scanner],

        BUILDERS={
        'OcamlObject': __obj_builder,
        'OcamlLex': __lex_builder,
        'OcamlLibrary': __lib_builder,
        'OcamlProgram': __exe_builder,
        },
        )


def exists(env):
    return env.Detect([
            'ocamlc', 'ocamlc.opt',
            'ocamlopt', 'ocamlopt.opt',
            'ocamllex', 'ocamllex.opt',
            ])
