// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#include <fekal/compiler.hpp>
#include <fekal/checker.hpp>
#include <fekal/parser.hpp>
#include <fekal/checker/syscalls/open.hpp>

namespace fekal {

Compiler::Compiler() {}

void Compiler::reset()
{
    context.reset();
    diagnostics.reset();
}

void Compiler::print_errors()
{
    diagnostics.print();
}

std::vector<ast::ProgramStatement> Compiler::compile(const std::string_view source)
{
    auto ast = fekal::parse(source);
    fekal::check(context, diagnostics, ast);
    compile_rules(ast);
    return ast;
}

void Compiler::compile_rules(const std::vector<ast::ProgramStatement>& ast)
{
    auto syscallOpen = fekal::rules::SyscallOpen{context, diagnostics};
    syscallOpen.check(ast);
}

} // namespace fekal
