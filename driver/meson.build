src = [
    'main.cpp',
]

fekal_bin = executable(
    'fekal',
    src,
    link_with : fekal_lib,
    dependencies : [boost],
    include_directories : incdir,
    implicit_include_directories: false,
    install : true,
)
