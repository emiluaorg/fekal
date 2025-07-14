// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <vector>
#include <fekal/ast.hpp>
#include <fekal/checker.hpp>
#include <fekal/diagnostics.hpp>

namespace fekal {

struct Compiler
{
    Context context;
    Diagnostics diagnostics;

    Compiler();
    Compiler(bool stdout_has_colors) : diagnostics(stdout_has_colors) {};

    void reset();
    void print_errors();
    std::vector<ast::ProgramStatement> compile(const std::string_view source);
    void compile_rules(const std::vector<ast::ProgramStatement>& source);
};

} // namespace fekal
