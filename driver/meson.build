src = [
    'main.cpp',
]

ncurses = dependency('ncurses')

fekal_bin = executable(
    'fekal',
    src,
    link_with : fekal_lib,
    dependencies : [boost, ncurses],
    include_directories : incdir,
    implicit_include_directories: false,
    install : true,
)
