// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <variant>
#include <memory>
#include <vector>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>

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
    IntLit(unsigned line, unsigned column, std::int64_t value)
        : NodeBase{line, column}
        , value{value}
    {}

    std::int64_t value;
};

struct Identifier : NodeBase
{
    Identifier(unsigned line, unsigned column, std::string value)
        : NodeBase{line, column}
        , value{std::move(value)}
    {}

    std::string value;
};

struct SumExpr : NodeBase
{
    SumExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
            std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct SubtractExpr : NodeBase
{
    SubtractExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
                 std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct MulExpr : NodeBase
{
    MulExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
            std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct DivExpr : NodeBase
{
    DivExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
            std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct LshiftExpr : NodeBase
{
    LshiftExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
               std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct RshiftExpr : NodeBase
{
    RshiftExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
               std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct BitAndExpr : NodeBase
{
    BitAndExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
               std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct BitXorExpr : NodeBase
{
    BitXorExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
               std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct BitOrExpr : NodeBase
{
    BitOrExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
              std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct EqExpr : NodeBase
{
    EqExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
           std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct NeqExpr : NodeBase
{
    NeqExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
            std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct LtExpr : NodeBase
{
    LtExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
           std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct GtExpr : NodeBase
{
    GtExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
           std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct LteExpr : NodeBase
{
    LteExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
            std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct GteExpr : NodeBase
{
    GteExpr(unsigned line, unsigned column, std::shared_ptr<IntExpr> left,
            std::shared_ptr<IntExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<IntExpr> left, right;
};

struct NegExpr : NodeBase
{
    NegExpr(unsigned line, unsigned column, std::shared_ptr<BoolExpr> inner)
        : NodeBase{line, column}
        , inner{std::move(inner)}
    {}

    std::shared_ptr<BoolExpr> inner;
};

struct AndExpr : NodeBase
{
    AndExpr(unsigned line, unsigned column, std::shared_ptr<BoolExpr> left,
            std::shared_ptr<BoolExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
        , right{std::move(right)}
    {}

    std::shared_ptr<BoolExpr> left, right;
};

struct OrExpr : NodeBase
{
    OrExpr(unsigned line, unsigned column, std::shared_ptr<BoolExpr> left,
           std::shared_ptr<BoolExpr> right)
        : NodeBase{line, column}
        , left{std::move(left)}
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
    std::string version;
};

struct Policy : NodeBase
{
    Policy(std::string name, std::string version, std::vector<PolicyStatement> body)
        : name{std::move(name)}
        , version{std::move(version)}
        , body{std::move(body)}
    {}

    std::string name;
    std::string version;
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
    return std::make_shared<IntExpr>(
        std::in_place_type<E>, line, column, std::forward<Args>(args)...);
}

template<class E, class... Args>
inline std::shared_ptr<BoolExpr> make_bool_expr(
    unsigned line, unsigned column, Args&&... args)
{
    return std::make_shared<BoolExpr>(
        std::in_place_type<E>, line, column, std::forward<Args>(args)...);
}

} // namespace ast

bool eval(const ast::BoolExpr& e);
std::string format(const std::vector<ast::ProgramStatement>& program);

} // namespace fekal
