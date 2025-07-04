// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/nodebase.hpp>
#include <string>

namespace fekal::ast {

struct UseStatement : NodeBase
{
    UseStatement(unsigned line, unsigned column, std::string policy,
            std::string version)
        : NodeBase{line, column}
        , policy{std::move(policy)}
        , version{std::move(version)}
    {}

    bool operator==(const UseStatement&) const = default;

    std::string policy;
    std::string version;
};

} // namespace fekal::ast
