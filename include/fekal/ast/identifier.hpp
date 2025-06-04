// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <string>

#include <fekal/ast/nodebase.hpp>

namespace fekal::ast {

struct Identifier : NodeBase
{
    Identifier(unsigned line, unsigned column, std::string value)
        : NodeBase{line, column}
        , value{std::move(value)}
    {}

    std::string value;

    bool operator==(const Identifier&) const = default;
};

} // namespace fekal::ast
