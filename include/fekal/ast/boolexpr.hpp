// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <variant>

#include <boost/mp11/algorithm.hpp>

#include <fekal/ast/eqexpr.hpp>
#include <fekal/ast/neqexpr.hpp>
#include <fekal/ast/ltexpr.hpp>
#include <fekal/ast/gtexpr.hpp>
#include <fekal/ast/lteexpr.hpp>
#include <fekal/ast/gteexpr.hpp>
#include <fekal/ast/negexpr.hpp>
#include <fekal/ast/andexpr.hpp>
#include <fekal/ast/orexpr.hpp>

namespace fekal::ast {

template<class T>
static constexpr bool IsBoolExpr = boost::mp11::mp_contains<BoolExpr, T>::value;

template<class E, class... Args>
inline std::shared_ptr<BoolExpr> make_bool_expr(
    unsigned line, unsigned column, Args&&... args)
{
    return std::make_shared<BoolExpr>(
        std::in_place_type<E>, line, column, std::forward<Args>(args)...);
}

} // namespace fekal::ast
