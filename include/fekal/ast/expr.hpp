// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <boost/mp11/list.hpp>

#include <fekal/ast/boolexpr.hpp>
#include <fekal/ast/intexpr.hpp>

namespace fekal::ast {

using Expr = boost::mp11::mp_append<IntExpr, BoolExpr>;

} // namespace fekal::ast
