// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/nodebase.hpp>
#include <fekal/ast/fwd/boolexpr.hpp>
#include <memory>

namespace fekal::ast {

struct OrExpr : NodeBase
{
    OrExpr(unsigned line, unsigned column, std::shared_ptr<BoolExpr> left,
           std::shared_ptr<BoolExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<BoolExpr> left, right;
};

} // namespace fekal::ast
