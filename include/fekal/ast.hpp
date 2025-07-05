// Copyright (c) 2025 Vinícius dos Santos Oliveira, César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/programstatement.hpp>
#include <boost/hana/functional/overload.hpp>
#include <memory>
#include <vector>

namespace fekal {

namespace hana = boost::hana;

bool eval(const ast::BoolExpr& e);

template<class Derived>
struct Traverser
{
    bool postOrder()
    {
        return false;
    }

    bool traverse(const ast::IntExpr& node)
    {
        return std::visit(hana::overload(
            [&](const ast::Identifier& identifier) {
                return traverse(identifier);
            },
            [&](const ast::IntLit& lit) {
                return traverse(lit);
            },
            [&](const auto& expr) {
                return traverse_expr(node, *expr.left, *expr.right);
            }
        ), node);
    }

    bool traverse(const ast::BoolExpr& node)
    {
        return std::visit(hana::overload(
            [&](const ast::NegExpr& neg) {
                return traverse_children(neg, *neg.inner);
            },
            [&](const auto& expr) {
                return traverse_expr(node, *expr.left, *expr.right);
            }
        ), node);
    }

    bool traverse(const ast::SyscallFilter& node)
    {
        return traverse_children(node, node.body);
    }

    bool traverse(const ast::ActionBlock& node)
    {
        return traverse_children(node, node.filters);
    }

    bool traverse(const ast::Policy& node)
    {
        return traverse_children(node, node.body);
    }

    template<class T>
    bool traverse(const T& node)
    {
        return visit_enter_impl(node);
    }

    template<class... T>
    bool traverse(const std::variant<T...>& node)
    {
        return std::visit<bool>([&](const auto& node) {
            return traverse(node);
        }, node);
    }

    template<class T>
    bool traverse(const std::vector<T>& nodes)
    {
        for (const auto& stmt : nodes) {
            if (!traverse(stmt)) {
                return false;
            }
        }
        return true;
    }

    template<class T>
    bool traverse(const std::vector<std::shared_ptr<T>>& nodes)
    {
        for (const auto& stmt : nodes) {
            if (!traverse(*stmt.get())) {
                return false;
            }
        }
        return true;
    }

private:
    Derived& derived()
    {
        return static_cast<Derived&>(*this);
    }

    template<class Node, class Child>
    bool traverse_children(const Node& node, const Child& child)
    {
        if (derived().postOrder()) {
            return traverse(child) && visit_enter_impl(node);
        } else {
            return visit_enter_impl(node) && traverse(child) && visit_leave_impl(node);
        }
    }

    template<class Node, class Left, class Right>
    bool traverse_expr(const Node& node, Left left, Right right)
    {
        if (derived().postOrder()) {
            return traverse(left) && traverse(right) && visit_enter_impl(node) && visit_leave_impl(node);
        } else {
            return visit_enter_impl(node) &&
                traverse(left) &&
                traverse(right) &&
                visit_leave_impl(node);
        }
    }

    template<class T>
    bool visit_enter_impl(const T& node)
    {
        if constexpr (requires(Derived& d, const T& node) { d.visit(node); }) {
            using ReturnType = decltype(std::declval<Derived&>().visit(node));
            if constexpr (std::is_void_v<ReturnType>) {
                derived().visit(node);
                return true;
            } else {
                return derived().visit(node);
            }
        }
        return true;
    }

    template<class T>
    bool visit_leave_impl(const T& node)
    {
        if constexpr (requires(Derived& d, const T& node) { d.visit_leave(node); }) {
            derived().visit_leave(node);
        }
        return true;
    }
};

} // namespace fekal
