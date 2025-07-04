// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <vector>

#include <fekal/ast/boolexpr.hpp>

namespace fekal::ast {

using SyscallParameters = std::vector<ast::Identifier>;

struct SyscallFilter : NodeBase
{
    SyscallFilter(unsigned line, unsigned column, std::string syscall)
        : NodeBase{line, column}
        , syscall{std::move(syscall)} {}

    SyscallFilter(
        unsigned line,
        unsigned column,
        std::string syscall,
        SyscallParameters params,
        std::vector<std::shared_ptr<BoolExpr>> body)
        : NodeBase{line, column}
        , syscall{std::move(syscall)}
        , params{std::move(params)}
        , body{std::move(body)}
    {}

    std::string syscall;
    SyscallParameters params;
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
