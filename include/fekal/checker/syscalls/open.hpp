// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/identifier.hpp>
#include <fekal/ast.hpp>
#include <fekal/checker.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/describe/enum.hpp>
#include <boost/describe/enum_to_string.hpp>
#include <optional>

namespace fekal::rules {

namespace mp11 = boost::mp11;

BOOST_DEFINE_ENUM_CLASS(
    OFLAGS,

    o_async,
    o_direct,
    o_dsync,
    o_largefile,
    o_noatime,
    o_noctty,
    o_path,
    o_sync,
    o_tmpfile,
    o_rdonly,
    o_wronly,
    o_rdwr,
    o_nonblock,
    o_append,
    o_creat,
    o_trunc,
    o_excl,
    o_directory,
    o_nofollow,
    o_cloexec,
);


struct SyscallOpen : Traverser<SyscallOpen>
{
    SyscallOpen(Context context, Diagnostics& diagnostics)
        : context{context}
        , diagnostics{diagnostics}
    {}

    static constexpr std::array<std::string_view, 2> syscalls = { "open", "openat" };

    void visit(const ast::IntExpr& expr)
    {
        std::visit(hana::overload(
            [](const ast::Identifier&) {},
            [](const ast::IntLit&) {},
            [&](const auto& expr) {
                check_expr(expr.left.get(), expr.right.get());
            }
        ), expr);
    }

    void visit(const ast::BoolExpr& expr)
    {
        std::visit(hana::overload(
            [](const ast::NegExpr&) {},
            [](const ast::AndExpr&) {},
            [](const ast::OrExpr&) {},
            [&](const auto& expr) {
                check_expr(expr.left.get(), expr.right.get());
            }
        ), expr);
    }

    bool visit(const ast::SyscallFilter& filter)
    {
        if (!std::find(syscalls.begin(), syscalls.end(), filter.syscall) || filter.params.size() < 2) {
            return false;
        }

        oflag_index = 1;
        // openat oflag is the third parameter
        if (filter.syscall == "openat") {
            oflag_index = 2;
        }

        auto& scope = context.get_scope_by_node(filter);

        mp11::mp_for_each<boost::describe::describe_enumerators<OFLAGS>>([&](auto flag) {
            std::string name = boost::to_upper_copy(
                std::string(boost::describe::enum_to_string(flag.value, "")),
                std::locale::classic()
            );
            scope.declare_inherit_symbol(Symbol{name});
        });

        filter_scope = scope;

        return true;
    }

    void check(const std::vector<ast::ProgramStatement>& ast)
    {
        traverse(ast);
    }

private:
    Context context;
    Diagnostics& diagnostics;
    std::optional<std::reference_wrapper<Scope>> filter_scope;
    unsigned oflag_index = 1;

    template<class Left, class Right>
    void check_expr(const Left& left, const Right& right)
    {
        auto& scope = filter_scope.value().get();
        if (scope.num_symbols() <= 1) {
            return;
        }
        const auto* identifierLeft = std::get_if<ast::Identifier>(left);
        if (identifierLeft) {
            this->check_expr_value(*identifierLeft, right, scope);
        }
        const auto* identifierRight = std::get_if<ast::Identifier>(right);
        if (identifierRight) {
            this->check_expr_value(*identifierRight, left, scope);
        }
    }

    template<class Other>
    void check_expr_value(const ast::Identifier& identifier, const Other& other, const Scope& scope)
    {
        const auto& symbol_position = scope.get_symbol_position(identifier.value);
        if (symbol_position.has_value()) {
            if (symbol_position.value() != oflag_index) {
                return;
            }
            std::visit(hana::overload(
                [&](const ast::Identifier& identifier) {
                    if (!scope.has_symbol(identifier.value)) {
                        diagnostics.error(
                            std::format("Invalid oflag {}", identifier.value),
                            diagnostics.rangeFromName(identifier, identifier.value)
                        );
                    }
                },
                [](const auto&) {}
            ), *other);
        }
    }
};
} // namespace fekal::rules
