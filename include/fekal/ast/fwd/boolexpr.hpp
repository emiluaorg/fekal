// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/fwd/eqexpr.hpp>
#include <fekal/ast/fwd/neqexpr.hpp>
#include <fekal/ast/fwd/ltexpr.hpp>
#include <fekal/ast/fwd/gtexpr.hpp>
#include <fekal/ast/fwd/lteexpr.hpp>
#include <fekal/ast/fwd/gteexpr.hpp>
#include <fekal/ast/fwd/negexpr.hpp>
#include <fekal/ast/fwd/andexpr.hpp>
#include <fekal/ast/fwd/orexpr.hpp>

namespace std {
template<class...> class variant;
} // namespace std

namespace fekal::ast {

using BoolExpr = std::variant<
    EqExpr, NeqExpr, LtExpr, GtExpr, LteExpr, GteExpr, NegExpr, AndExpr,
    OrExpr>;

} // namespace fekal::ast
