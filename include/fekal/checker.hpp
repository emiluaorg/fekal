// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast.hpp>
#include <fekal/diagnostics.hpp>
#include <fekal/checker/context.hpp>
#include <ranges>
#include <format>

namespace fekal {

struct Checker : public Traverser<Checker>
{
    Checker(Context& context, Diagnostics& diagnostics)
        : context{context}
        , diagnostics{diagnostics}
    {}

    void visit(const ast::IntExpr& expr)
    {
        auto& scope = context.peek_scope();
        std::visit(hana::overload(
            [&](const ast::Identifier& identifier) {
                if (scope.has_symbol(identifier.value)) {
                    scope.increase_reference(identifier.value);
                }
            },
            [&](const auto& expr) {}
        ), expr);
    }

    void visit(const ast::SyscallFilter& filter)
    {
        auto& scope = context.peek_scope();
        if (scope.has_symbol(filter.syscall)) {
            diagnostics.error(
                std::format("Syscall filter `{}` already declared in this scope", filter.syscall),
                diagnostics.rangeFromName(filter, filter.syscall)
            );
        }

        scope.declare_symbol(Symbol{filter.syscall});

        if (filter.params.size() > 0) {
            auto& scope = context.push_scope(filter);
            for (const auto& p : filter.params) {
                if (scope.has_symbol(p.value)) {
                    diagnostics.error(
                        std::format("syscall parameter {} already declared", p.value),
                        diagnostics.rangeFromName(p, p.value)
                    );
                    continue;
                }
                scope.declare_symbol(Symbol{p.value});
            }
        }
    }

    void visit_leave(const ast::SyscallFilter& filter)
    {
        if (filter.params.size() > 0) {
            auto& scope = context.peek_scope();
            for (const auto& p : std::views::reverse(filter.params)) {
                auto symbol = scope.get_symbol(p.value);
                if (symbol.references == 0) {
                    diagnostics.warning(
                        std::format("Parameter {} unused", symbol.name),
                        diagnostics.rangeFromName(p, symbol.name)
                    );
                } else {
                    break;
                }
            }
            context.pop_scope();
        }
    }

    void visit(const ast::UseStatement& stmt)
    {
        if (!context.has_symbol(stmt.id())) {
            diagnostics.error(
                std::format("Policy {} doesn't exist", stmt.id()),
                diagnostics.rangeFromName(stmt, stmt.policy)
            );
        }
    }

    void visit(const ast::ActionBlock& block)
    {
        context.push_scope(block);
    }

    void visit_leave(const ast::ActionBlock& block)
    {
        context.pop_scope();
    }

    void visit(const ast::Policy& policy)
    {
        auto& scope = context.peek_scope();
        if (scope.has_symbol(policy.id())) {
            diagnostics.error(
                std::format("policy {} already declared", policy.id()),
                diagnostics.rangeFromName(policy, policy.name)
            );
        }
        scope.declare_symbol(Symbol(policy.id()));
        context.push_scope(policy);
    }

    void visit_leave(const ast::Policy& policy)
    {
        context.pop_scope();
    }

private:
    Context& context;
    Diagnostics& diagnostics;
};

Checker check(
        Context& context,
        Diagnostics& diagnostics,
        const std::vector<ast::ProgramStatement>& ast);

} // namespace fekal
