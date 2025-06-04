// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/nodebase.hpp>
#include <fekal/ast/fwd/boolexpr.hpp>
#include <memory>

namespace fekal::ast {

struct NegExpr : NodeBase
{
    NegExpr(unsigned line, unsigned column, std::shared_ptr<BoolExpr> inner)
        : NodeBase{line, column}
        , inner{std::move(inner)}
    {}

    const NodeBase& base() const { return *this; }
    NodeBase& base() { return *this; }

    bool operator==(const NegExpr&) const;

    std::shared_ptr<BoolExpr> inner;
};

} // namespace fekal::ast
