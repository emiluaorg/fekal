// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/nodebase.hpp>
#include <fekal/ast/intexpr.hpp>
#include <memory>

namespace fekal::ast {

struct LteExpr : NodeBase
{
    LteExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
            std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    const NodeBase& base() const { return *this; }
    NodeBase& base() { return *this; }

    bool operator==(const LteExpr&) const;

    std::shared_ptr<IntExpr> left, right;
};

} // namespace fekal::ast
