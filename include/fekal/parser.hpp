// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast.hpp>
#include <string_view>

namespace fekal {

std::vector<ast::ProgramStatement> parse(std::string_view input);

} // namespace fekal
