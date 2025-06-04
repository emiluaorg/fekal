// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/syscallfilter.hpp>
#include <fekal/ast/action.hpp>

namespace fekal::ast {

struct ActionBlock : NodeBase
{
    ActionBlock(Action action, std::vector<SyscallFilter> filters)
        : action{std::move(action)}
        , filters{std::move(filters)}
    {}

    Action action;
    std::vector<SyscallFilter> filters;

    bool operator==(const ActionBlock&) const = default;
};

} // namespace fekal::ast
