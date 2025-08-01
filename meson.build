project(
    'fekal', 'cpp',
    default_options : ['cpp_std=c++20'],
    meson_version : '>=1.3.0',
    license : 'MIT-0',
    version : '0.1.0',
)

pkgconfig = import('pkgconfig')

libseccomp = dependency('libseccomp')
boost = dependency(
    'boost',
    version : '>=1.82',
)

extra_re2c_args = []

if meson.get_compiler('cpp').get_id() == 'gcc'
    extra_re2c_args += '--case-ranges'
endif

re2c_gen = generator(
    find_program('re2c'),
    output: '@BASENAME@.cpp',
    depfile : '@PLAINNAME@.d',
    arguments: [
        '--depfile', '@DEPFILE@',
        '-W', # Turn on all warnings
        '--flex-syntax',
        '--empty-class', 'match-none',
        '--input-encoding', 'ascii',
        extra_re2c_args,
        '-I', '@CURRENT_SOURCE_DIR@/include',
        '@INPUT@', '-o', '@OUTPUT@',
    ],
)

incdir = include_directories([
    'include',
])

src = [
    'src/ast.cpp',
    'src/parser.cpp',
    'src/checker.cpp',
    'src/compiler.cpp',
    'src/printer.cpp',
]

re2c_src = [
    'src/reader.ypp',
]

fekal_lib = library(
    'fekal',
    src,
    re2c_gen.process(re2c_src),
    dependencies : [libseccomp, boost],
    include_directories : incdir,
    implicit_include_directories : false,
    install : true,
)

subdir('driver')
subdir('test')
