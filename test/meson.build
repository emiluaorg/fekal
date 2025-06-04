test_src = [
    'test_ast.cpp',
]

test_bin = executable(
    'tests',
    test_src,
    link_with: fekal_lib,
    dependencies: [boost],
    include_directories: incdir,
)

test('tests', test_bin)
