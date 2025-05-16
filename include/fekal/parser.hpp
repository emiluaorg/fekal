// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <fekal/ast.hpp>
#include <string_view>

namespace fekal {

ast::Expr parse(std::string_view input);

} // namespace fekal
