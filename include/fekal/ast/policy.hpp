// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/policystatement.hpp>

namespace fekal::ast {

struct Policy : NodeBase
{
    Policy(unsigned line,
           unsigned column,
           std::string name,
           std::string version,
           std::vector<PolicyStatement> body)
        : NodeBase{line, column}
        , name{std::move(name)}
        , version{std::move(version)}
        , body{std::move(body)}
    {}

    std::string name;
    std::string version;
    std::vector<PolicyStatement> body;

    bool operator==(const Policy&) const = default;

    std::string id() const {
        return name + version;
    }
};

} // namespace fekal::ast
