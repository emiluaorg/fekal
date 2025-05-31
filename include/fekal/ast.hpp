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

using IntExpr = std::variant<
    IntLit, Identifier, SumExpr, SubtractExpr, MulExpr, DivExpr, LshiftExpr,
    RshiftExpr, BitAndExpr, BitXorExpr, BitOrExpr>;

using BoolExpr = std::variant<
    EqExpr, NeqExpr, LtExpr, GtExpr, LteExpr, GteExpr, NegExpr, AndExpr,
    OrExpr>;

using Expr = boost::mp11::mp_append<IntExpr, BoolExpr>;

template<class T>
constexpr bool IsIntExpr = boost::mp11::mp_contains<IntExpr, T>::value;

template<class T>
constexpr bool IsBoolExpr = boost::mp11::mp_contains<BoolExpr, T>::value;

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
    SumExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct SubtractExpr : NodeBase
{
    SubtractExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct MulExpr : NodeBase
{
    MulExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct DivExpr : NodeBase
{
    DivExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct LshiftExpr : NodeBase
{
    LshiftExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct RshiftExpr : NodeBase
{
    RshiftExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct BitAndExpr : NodeBase
{
    BitAndExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct BitXorExpr : NodeBase
{
    BitXorExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct BitOrExpr : NodeBase
{
    BitOrExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct EqExpr : NodeBase
{
    EqExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct NeqExpr : NodeBase
{
    NeqExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct LtExpr : NodeBase
{
    LtExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct GtExpr : NodeBase
{
    GtExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct LteExpr : NodeBase
{
    LteExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct GteExpr : NodeBase
{
    GteExpr(std::shared_ptr<IntExpr> left, std::shared_ptr<IntExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct NegExpr : NodeBase
{
    NegExpr(std::shared_ptr<BoolExpr> inner)
        : inner{std::move(inner)}
    {}

    std::shared_ptr<BoolExpr> inner;
};

struct AndExpr : NodeBase
{
    AndExpr(std::shared_ptr<BoolExpr> left, std::shared_ptr<BoolExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<BoolExpr> left, right;
};

struct OrExpr : NodeBase
{
    OrExpr(std::shared_ptr<BoolExpr> left, std::shared_ptr<BoolExpr> right)
        : left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<BoolExpr> left, right;
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
    Policy(std::string name, std::vector<PolicyStatement> body)
        : name{std::move(name)}
        , body{std::move(body)}
    {}

    std::string name;
    std::vector<PolicyStatement> body;
};

struct DefaultAction : NodeBase, Action
{
    using Action::Action;
};

template<class E, class... Args>
inline std::shared_ptr<IntExpr> make_int_expr(
    unsigned line, unsigned column, Args&&... args)
{
    auto ret = std::make_shared<IntExpr>(
        std::in_place_type<E>, std::forward<Args>(args)...);
    NodeBase& base =
        std::visit([](NodeBase& e) -> NodeBase& { return e; }, *ret);
    base.line = line;
    base.column = column;
    return ret;
}

template<class E, class... Args>
inline std::shared_ptr<BoolExpr> make_bool_expr(
    unsigned line, unsigned column, Args&&... args)
{
    auto ret = std::make_shared<BoolExpr>(
        std::in_place_type<E>, std::forward<Args>(args)...);
    NodeBase& base =
        std::visit([](NodeBase& e) -> NodeBase& { return e; }, *ret);
    base.line = line;
    base.column = column;
    return ret;
}

} // namespace ast

bool eval(const ast::BoolExpr& e);
std::string format(const std::vector<ast::ProgramStatement>& program);

} // namespace fekal
