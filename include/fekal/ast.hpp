// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/programstatement.hpp>

namespace fekal {

bool eval(const ast::BoolExpr& e);
std::string format(const std::vector<ast::ProgramStatement>& program);

} // namespace fekal
