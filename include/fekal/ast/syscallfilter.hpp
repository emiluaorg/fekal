// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <vector>

#include <fekal/ast/boolexpr.hpp>

namespace fekal::ast {

struct SyscallFilter : NodeBase
{
    SyscallFilter(std::string syscall) : syscall{std::move(syscall)} {}

    SyscallFilter(
        std::string syscall, std::vector<std::string> params,
        std::vector<std::shared_ptr<BoolExpr>> body)
        : syscall{std::move(syscall)}
        , params{std::move(params)}
        , body{std::move(body)}
    {}

    std::string syscall;
    std::vector<std::string> params;
    std::vector<std::shared_ptr<BoolExpr>> body;
};

} // namespace fekal::ast
