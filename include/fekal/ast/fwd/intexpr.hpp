// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/fwd/intlit.hpp>
#include <fekal/ast/fwd/identifier.hpp>
#include <fekal/ast/fwd/sumexpr.hpp>
#include <fekal/ast/fwd/subtractexpr.hpp>
#include <fekal/ast/fwd/mulexpr.hpp>
#include <fekal/ast/fwd/divexpr.hpp>
#include <fekal/ast/fwd/lshiftexpr.hpp>
#include <fekal/ast/fwd/rshiftexpr.hpp>
#include <fekal/ast/fwd/bitandexpr.hpp>
#include <fekal/ast/fwd/bitxorexpr.hpp>
#include <fekal/ast/fwd/bitorexpr.hpp>

namespace std {
template<class...> class variant;
} // namespace std

namespace fekal::ast {

using IntExpr = std::variant<
    IntLit, Identifier, SumExpr, SubtractExpr, MulExpr, DivExpr, LshiftExpr,
    RshiftExpr, BitAndExpr, BitXorExpr, BitOrExpr>;

} // namespace fekal::ast
