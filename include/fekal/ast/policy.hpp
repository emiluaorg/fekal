// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/policystatement.hpp>

namespace fekal::ast {

struct Policy : NodeBase
{
    Policy(std::string name, std::string version,
           std::vector<PolicyStatement> body)
        : name{std::move(name)}
        , version{std::move(version)}
        , body{std::move(body)}
    {}

    std::string name;
    std::string version;
    std::vector<PolicyStatement> body;
};

} // namespace fekal::ast
