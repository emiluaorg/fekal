// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast.hpp>
#include <ostream>

namespace fekal {

void print(std::ostream& stream, const std::vector<ast::ProgramStatement>& ast);

} // namespace fekal
