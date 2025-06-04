// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <variant>

#include <fekal/ast/usestatement.hpp>
#include <fekal/ast/actionblock.hpp>

namespace fekal::ast {

using PolicyStatement = std::variant<UseStatement, ActionBlock>;

} // namespace fekal::ast
