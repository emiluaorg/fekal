// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <variant>

#include <boost/mp11/algorithm.hpp>

#include <fekal/ast/intlit.hpp>
#include <fekal/ast/identifier.hpp>
#include <fekal/ast/sumexpr.hpp>
#include <fekal/ast/subtractexpr.hpp>
#include <fekal/ast/mulexpr.hpp>
#include <fekal/ast/divexpr.hpp>
#include <fekal/ast/lshiftexpr.hpp>
#include <fekal/ast/rshiftexpr.hpp>
#include <fekal/ast/bitandexpr.hpp>
#include <fekal/ast/bitxorexpr.hpp>
#include <fekal/ast/bitorexpr.hpp>

namespace fekal::ast {

template<class T>
static constexpr bool IsIntExpr = boost::mp11::mp_contains<IntExpr, T>::value;

template<class E, class... Args>
inline std::shared_ptr<IntExpr> make_int_expr(
    unsigned line, unsigned column, Args&&... args)
{
    return std::make_shared<IntExpr>(
        std::in_place_type<E>, line, column, std::forward<Args>(args)...);
}

} // namespace fekal::ast
