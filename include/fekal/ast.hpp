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
    SumExpr(Expr left, Expr right);
    SumExpr(SumExpr&& o) = default;

    SumExpr(const SumExpr& o)
        : left{std::make_unique<Expr>(*o.left)}
        , right{std::make_unique<Expr>(*o.right)}
    {}

    std::unique_ptr<Expr> left, right;
};

inline void swap(SumExpr& a, SumExpr& b)
{
    using std::swap;
    swap(a.left, b.left);
    swap(a.right, b.right);
}

struct SubtractExpr
{
    SubtractExpr(Expr left, Expr right);
    SubtractExpr(SubtractExpr&& o) = default;

    SubtractExpr(const SubtractExpr& o)
        : left{std::make_unique<Expr>(*o.left)}
        , right{std::make_unique<Expr>(*o.right)}
    {}

    std::unique_ptr<Expr> left, right;
};

inline void swap(SubtractExpr& a, SubtractExpr& b)
{
    using std::swap;
    swap(a.left, b.left);
    swap(a.right, b.right);
}

struct MulExpr
{
    MulExpr(Expr left, Expr right);
    MulExpr(MulExpr&& o) = default;

    MulExpr(const MulExpr& o)
        : left{std::make_unique<Expr>(*o.left)}
        , right{std::make_unique<Expr>(*o.right)}
    {}

    std::unique_ptr<Expr> left, right;
};

inline void swap(MulExpr& a, MulExpr& b)
{
    using std::swap;
    swap(a.left, b.left);
    swap(a.right, b.right);
}

struct DivExpr
{
    DivExpr(Expr left, Expr right);
    DivExpr(DivExpr&& o) = default;

    DivExpr(const DivExpr& o)
        : left{std::make_unique<Expr>(*o.left)}
        , right{std::make_unique<Expr>(*o.right)}
    {}

    std::unique_ptr<Expr> left, right;
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

inline SumExpr::SumExpr(Expr left, Expr right)
    : left{std::make_unique<Expr>(std::move(left))}
    , right{std::make_unique<Expr>(std::move(right))}
{}

inline SubtractExpr::SubtractExpr(Expr left, Expr right)
    : left{std::make_unique<Expr>(std::move(left))}
    , right{std::make_unique<Expr>(std::move(right))}
{}

inline MulExpr::MulExpr(Expr left, Expr right)
    : left{std::make_unique<Expr>(std::move(left))}
    , right{std::make_unique<Expr>(std::move(right))}
{}

inline DivExpr::DivExpr(Expr left, Expr right)
    : left{std::make_unique<Expr>(std::move(left))}
    , right{std::make_unique<Expr>(std::move(right))}
{}

} // namespace ast

int eval(const ast::Expr& e);

} // namespace fekal
