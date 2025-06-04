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

    bool operator==(const SyscallFilter& o) const {
        bool syscall_eq = syscall == o.syscall;
        bool params_eq = params == o.params;
        bool body_eq = body.size() == o.body.size() && std::equal(
            body.begin(),
            body.end(),
            o.body.begin(),
            o.body.end(),
            [](const std::shared_ptr<BoolExpr>& a, const std::shared_ptr<BoolExpr>& b) {
                return *a == *b;
            }
        );
        return syscall_eq && params_eq && body_eq;
    }
};

} // namespace fekal::ast
