// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#include <fekal/checker.hpp>
#include <fekal/diagnostics.hpp>

namespace fekal {

Checker check(
    Context& context,
    Diagnostics& diagnostics,
    const std::vector<ast::ProgramStatement>& ast)
{
    Checker checker{context, diagnostics};
    checker.traverse(ast);
    return checker;
}

} // namespace fekal
