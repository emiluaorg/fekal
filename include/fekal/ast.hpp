// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <cstdlib>
#include <variant>
#include <memory>
#include <vector>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/hana/functional/overload.hpp>

namespace fekal {

namespace ast {

struct Expr;
struct IntLit;
struct Identifier;
struct SumExpr;
struct SubtractExpr;
struct MulExpr;
struct DivExpr;
struct LshiftExpr;
struct RshiftExpr;
struct BitAndExpr;
struct BitXorExpr;
struct BitOrExpr;
struct EqExpr;
struct NeqExpr;
struct LtExpr;
struct GtExpr;
struct LteExpr;
struct GteExpr;
struct NegExpr;
struct AndExpr;
struct OrExpr;
struct ActionAllow {};
struct ActionLog {};
struct ActionKillProcess {};
struct ActionKillThread {};
struct ActionUserNotif {};
struct ActionErrno;
struct ActionTrap;
struct ActionTrace;
using Action = std::variant<
    ActionAllow, ActionLog, ActionKillProcess, ActionKillThread,
    ActionUserNotif, ActionErrno, ActionTrap, ActionTrace>;
struct SyscallFilter;
struct ActionBlock;
struct UseStatement;
using PolicyStatement = std::variant<UseStatement, ActionBlock>;
struct Policy;
struct DefaultAction;
using ProgramStatement = boost::mp11::mp_append<
    std::variant<Policy, DefaultAction>,
    PolicyStatement>;

using IntExprs = std::variant<
    IntLit, Identifier, SumExpr, SubtractExpr, MulExpr, DivExpr, LshiftExpr,
    RshiftExpr, BitAndExpr, BitXorExpr, BitOrExpr>;

using BoolExprs = std::variant<
    EqExpr, NeqExpr, LtExpr, GtExpr, LteExpr, GteExpr, NegExpr, AndExpr,
    OrExpr>;

using Exprs = boost::mp11::mp_append<IntExprs, BoolExprs>;

template<class T>
constexpr bool IsIntExpr = boost::mp11::mp_contains<IntExprs, T>::value;

template<class T>
constexpr bool IsBoolExpr = boost::mp11::mp_contains<BoolExprs, T>::value;

struct NodeBase
{
    unsigned line = 0;
    unsigned column = 0;
};

struct IntLit : NodeBase
{
    IntLit(std::int64_t value) : value{value} {}

    std::int64_t value;
};

struct Identifier : NodeBase
{
    Identifier(std::string value) : value{std::move(value)} {}

    std::string value;
};

struct SumExpr : NodeBase
{
    SumExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct SubtractExpr : NodeBase
{
    SubtractExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct MulExpr : NodeBase
{
    MulExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct DivExpr : NodeBase
{
    DivExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct LshiftExpr : NodeBase
{
    LshiftExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct RshiftExpr : NodeBase
{
    RshiftExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct BitAndExpr : NodeBase
{
    BitAndExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct BitXorExpr : NodeBase
{
    BitXorExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct BitOrExpr : NodeBase
{
    BitOrExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct EqExpr : NodeBase
{
    EqExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct NeqExpr : NodeBase
{
    NeqExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct LtExpr : NodeBase
{
    LtExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct GtExpr : NodeBase
{
    GtExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct LteExpr : NodeBase
{
    LteExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct GteExpr : NodeBase
{
    GteExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, IntExprs> left, right;
};

struct NegExpr : NodeBase
{
    NegExpr(const std::shared_ptr<Expr>& inner);

    boost::mp11::mp_transform<std::shared_ptr, BoolExprs> inner;
};

struct AndExpr : NodeBase
{
    AndExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, BoolExprs> left, right;
};

struct OrExpr : NodeBase
{
    OrExpr(
        const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);

    boost::mp11::mp_transform<std::shared_ptr, BoolExprs> left, right;
};

struct Expr : Exprs
{
    using Exprs::Exprs;

    NodeBase& base()
    {
        return std::visit([](NodeBase& e) -> NodeBase& { return e; }, *this);
    }

    const NodeBase& base() const
    {
        return std::visit([](const NodeBase& e) -> const NodeBase& {
            return e; }, *this);
    }
};

struct ActionErrno
{
    int errnum;
};

struct ActionTrap
{
    std::int64_t code;
};

struct ActionTrace
{
    std::int64_t code;
};

struct SyscallFilter : NodeBase
{
    using expr_type = boost::mp11::mp_transform<std::shared_ptr, BoolExprs>;

    SyscallFilter(std::string syscall) : syscall{std::move(syscall)} {}

    SyscallFilter(
        std::string syscall, std::vector<std::string> params,
        std::vector<expr_type> body)
        : syscall{std::move(syscall)}
        , params{std::move(params)}
        , body{std::move(body)}
    {}

    std::string syscall;
    std::vector<std::string> params;
    std::vector<expr_type> body;
};

struct ActionBlock : NodeBase
{
    ActionBlock(Action action, std::vector<SyscallFilter> filters)
        : action{std::move(action)}
        , filters{std::move(filters)}
    {}

    Action action;
    std::vector<SyscallFilter> filters;
};

struct UseStatement
{
    std::string policy;
};

struct Policy : NodeBase
{
    Policy(std::string name, std::vector<ast::PolicyStatement> body)
        : name{std::move(name)}
        , body{std::move(body)}
    {}

    std::string name;
    std::vector<ast::PolicyStatement> body;
};

struct DefaultAction : NodeBase, Action
{
    using Action::Action;
};

template<class E, class... Args>
inline std::shared_ptr<ast::Expr> make_expr(
    unsigned line, unsigned column, Args&&... args)
{
    auto ret = std::make_shared<ast::Expr>(
        std::in_place_type<E>, std::forward<Args>(args)...);
    ret->base().line = line;
    ret->base().column = column;
    return ret;
}

inline
boost::mp11::mp_transform<std::shared_ptr, BoolExprs>
unwrap_bool_expr(const std::shared_ptr<Expr>& e)
{
    using Ret = boost::mp11::mp_transform<std::shared_ptr, BoolExprs>;

    return std::visit(boost::hana::overload(
        [&]<class T>(T& inner) -> std::enable_if_t<IsBoolExpr<T>, Ret> {
            return std::shared_ptr<T>{e, &inner};
        },
        []<class T>(T&) -> std::enable_if_t<!IsBoolExpr<T>, Ret> {
            std::abort();
        }
    ), *e);
}

inline
boost::mp11::mp_transform<std::shared_ptr, IntExprs>
unwrap_int_expr(const std::shared_ptr<Expr>& e)
{
    using Ret = boost::mp11::mp_transform<std::shared_ptr, IntExprs>;

    return std::visit(boost::hana::overload(
        [&]<class T>(T& inner) -> std::enable_if_t<IsIntExpr<T>, Ret> {
            return std::shared_ptr<T>{e, &inner};
        },
        []<class T>(T&) -> std::enable_if_t<!IsIntExpr<T>, Ret> {
            std::abort();
        }
    ), *e);
}

inline SumExpr::SumExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline SubtractExpr::SubtractExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline MulExpr::MulExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline DivExpr::DivExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline LshiftExpr::LshiftExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline RshiftExpr::RshiftExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline BitAndExpr::BitAndExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline BitXorExpr::BitXorExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline BitOrExpr::BitOrExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline EqExpr::EqExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline NeqExpr::NeqExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline LtExpr::LtExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline GtExpr::GtExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline LteExpr::LteExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline GteExpr::GteExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_int_expr(left)}
    , right{unwrap_int_expr(right)}
{}

inline NegExpr::NegExpr(const std::shared_ptr<Expr>& inner)
    : inner{unwrap_bool_expr(inner)}
{}

inline AndExpr::AndExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_bool_expr(left)}
    , right{unwrap_bool_expr(right)}
{}

inline OrExpr::OrExpr(
    const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right)
    : left{unwrap_bool_expr(left)}
    , right{unwrap_bool_expr(right)}
{}

} // namespace ast

bool eval(const std::shared_ptr<ast::Expr>& e);

} // namespace fekal
