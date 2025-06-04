// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <cstdint>

#include <fekal/ast/nodebase.hpp>

namespace fekal::ast {

struct IntLit : NodeBase
{
    IntLit(unsigned line, unsigned column, std::int64_t value)
        : NodeBase{line, column}
        , value{value}
    {}

    std::int64_t value;

    bool operator==(const IntLit&) const = default;
};

} // namespace fekal::ast
