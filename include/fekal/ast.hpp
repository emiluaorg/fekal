// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <variant>
#include <memory>
#include <boost/mp11/list.hpp>

namespace fekal {

namespace ast {

struct Expr;

struct IntLit
{
    std::int64_t value;
};

struct SumExpr
{
    SumExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);
    SumExpr(SumExpr&& o) = default;
    SumExpr(const SumExpr& o) = default;

    std::shared_ptr<Expr> left, right;
};

inline void swap(SumExpr& a, SumExpr& b)
{
    using std::swap;
    swap(a.left, b.left);
    swap(a.right, b.right);
}

struct SubtractExpr
{
    SubtractExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);
    SubtractExpr(SubtractExpr&& o) = default;
    SubtractExpr(const SubtractExpr& o) = default;

    std::shared_ptr<Expr> left, right;
};

inline void swap(SubtractExpr& a, SubtractExpr& b)
{
    using std::swap;
    swap(a.left, b.left);
    swap(a.right, b.right);
}

struct MulExpr
{
    MulExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);
    MulExpr(MulExpr&& o) = default;
    MulExpr(const MulExpr& o) = default;

    std::shared_ptr<Expr> left, right;
};

inline void swap(MulExpr& a, MulExpr& b)
{
    using std::swap;
    swap(a.left, b.left);
    swap(a.right, b.right);
}

struct DivExpr
{
    DivExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);
    DivExpr(DivExpr&& o) = default;
    DivExpr(const DivExpr& o) = default;

    std::shared_ptr<Expr> left, right;
};

inline void swap(DivExpr& a, DivExpr& b)
{
    using std::swap;
    swap(a.left, b.left);
    swap(a.right, b.right);
}

using Exprs = boost::mp11::mp_list<
    IntLit, SumExpr, SubtractExpr, MulExpr, DivExpr>;

struct Expr : boost::mp11::mp_apply<std::variant, Exprs>
{
    using boost::mp11::mp_apply<std::variant, Exprs>::variant;

    unsigned line = 0;
    unsigned column = 0;
};

template<class E, class... Args>
inline std::shared_ptr<ast::Expr> make_expr(Args&&... args)
{
    return std::make_shared<ast::Expr>(
        std::in_place_type<E>, std::forward<Args>(args)...);
}

inline SumExpr::SumExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
    : left{std::move(left)}
    , right{std::move(right)}
{}

inline SubtractExpr::SubtractExpr(
    std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
    : left{std::move(left)}
    , right{std::move(right)}
{}

inline MulExpr::MulExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
    : left{std::move(left)}
    , right{std::move(right)}
{}

inline DivExpr::DivExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
    : left{std::move(left)}
    , right{std::move(right)}
{}

} // namespace ast

int eval(const ast::Expr& e);

} // namespace fekal
