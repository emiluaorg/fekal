// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/ast.hpp>
#include <boost/hana/functional/overload.hpp>
#include <format>

namespace fekal {

namespace hana = boost::hana;
namespace mp11 = boost::mp11;

static
std::int64_t eval_int(
    const mp11::mp_transform<std::shared_ptr, ast::IntExprs> &e)
{
    return std::visit(hana::overload(
        [](const std::shared_ptr<ast::IntLit>& e) { return e->value; },
        [](const std::shared_ptr<ast::Identifier>&) -> std::int64_t {
            return 0; },
        [](const std::shared_ptr<ast::SumExpr>& e) {
            return eval_int(e->left) + eval_int(e->right); },
        [](const std::shared_ptr<ast::SubtractExpr>& e) {
            return eval_int(e->left) - eval_int(e->right); },
        [](const std::shared_ptr<ast::MulExpr>& e) {
            return eval_int(e->left) * eval_int(e->right); },
        [](const std::shared_ptr<ast::DivExpr>& e) {
            return eval_int(e->left) / eval_int(e->right); },
        [](const std::shared_ptr<ast::LshiftExpr>& e) {
            return eval_int(e->left) << eval_int(e->right); },
        [](const std::shared_ptr<ast::RshiftExpr>& e) {
            return eval_int(e->left) >> eval_int(e->right); },
        [](const std::shared_ptr<ast::BitAndExpr>& e) {
            return eval_int(e->left) & eval_int(e->right); },
        [](const std::shared_ptr<ast::BitXorExpr>& e) {
            return eval_int(e->left) ^ eval_int(e->right); },
        [](const std::shared_ptr<ast::BitOrExpr>& e) {
            return eval_int(e->left) | eval_int(e->right); }
    ), e);
}

static bool eval(const mp11::mp_transform<std::shared_ptr, ast::BoolExprs> &e)
{
    return std::visit(hana::overload(
        [](const std::shared_ptr<ast::EqExpr>& e) {
            return eval_int(e->left) == eval_int(e->right); },
        [](const std::shared_ptr<ast::NeqExpr>& e) {
            return eval_int(e->left) != eval_int(e->right); },
        [](const std::shared_ptr<ast::LtExpr>& e) {
            return eval_int(e->left) < eval_int(e->right); },
        [](const std::shared_ptr<ast::GtExpr>& e) {
            return eval_int(e->left) > eval_int(e->right); },
        [](const std::shared_ptr<ast::LteExpr>& e) {
            return eval_int(e->left) <= eval_int(e->right); },
        [](const std::shared_ptr<ast::GteExpr>& e) {
            return eval_int(e->left) >= eval_int(e->right); },
        [](const std::shared_ptr<ast::NegExpr>& e) {
            return !eval(e->inner); },
        [](const std::shared_ptr<ast::AndExpr>& e) {
            return (eval(e->left) && eval(e->right)); },
        [](const std::shared_ptr<ast::OrExpr>& e) {
            return (eval(e->left) || eval(e->right)); }
    ), e);
}

bool eval(const std::shared_ptr<ast::Expr>& e)
{
    return eval(unwrap_bool_expr(e));
}

static std::string format(const ast::Action& action)
{
    return std::visit<std::string>(hana::overload(
        [](const ast::ActionAllow&) { return "ALLOW";},
        [](const ast::ActionLog&) { return "LOG";},
        [](const ast::ActionKillProcess&) { return "KILL_PROCESS";},
        [](const ast::ActionKillThread&) { return "KILL_THREAD";},
        [](const ast::ActionUserNotif&) { return "USER_NOTIF";},
        [](const ast::ActionErrno& a) {
            return std::format("ERRNO{{{}}}", a.errnum);},
        [](const ast::ActionTrap& a) {
            return std::format("TRAP{{{}}}", a.code);},
        [](const ast::ActionTrace& a) {
            return std::format("TRACE{{{}}}", a.code);}
    ), action);
}

static std::string format(
    const mp11::mp_transform<std::shared_ptr, ast::IntExprs>& expr,
    unsigned indent)
{
    return std::visit<std::string>(hana::overload(
        [&](const std::shared_ptr<ast::IntLit>& e) {
            return std::to_string(e->value); },
        [&](const std::shared_ptr<ast::Identifier>& e) { return e->value; },
        [&](const std::shared_ptr<ast::SumExpr>& e) {
            return "(+\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::SubtractExpr>& e) {
            return "(-\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::MulExpr>& e) {
            return "(*\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::DivExpr>& e) {
            return "(/\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::LshiftExpr>& e) {
            return "(<<\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::RshiftExpr>& e) {
            return "(>>\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::BitAndExpr>& e) {
            return "(&\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::BitXorExpr>& e) {
            return "(^\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::BitOrExpr>& e) {
            return "(|\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        }
    ), expr);
}

static std::string format(const ast::SyscallFilter::expr_type& expr, unsigned indent)
{
    return std::visit<std::string>(hana::overload(
        [&](const std::shared_ptr<ast::EqExpr>& e) {
            return "(==\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::NeqExpr>& e) {
            return "(!=\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::LtExpr>& e) {
            return "(<\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::GtExpr>& e) {
            return "(>\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::LteExpr>& e) {
            return "(<=\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::GteExpr>& e) {
            return "(>=\n" +
                std::string(indent + 1, ' ') +
                format(e->left, indent + 1) + '\n' +
                std::string(indent + 1, ' ') +
                format(e->right, indent + 1) + ')';
        },
        [&](const std::shared_ptr<ast::NegExpr>& e) {
            return '!' + format(e->inner, indent); },
        [&](const std::shared_ptr<ast::AndExpr>& e) {
            return "AndExpr{\n" +
                std::string(indent + 1, ' ') + format(e->left, indent + 1) +
                ",\n" +
                std::string(indent + 1, ' ') + format(e->right, indent + 1) +
                '\n' + std::string(indent, ' ') + '}';
        },
        [&](const std::shared_ptr<ast::OrExpr>& e) {
            return "OrExpr{\n" +
                std::string(indent + 1, ' ') + format(e->left, indent + 1) +
                ",\n" +
                std::string(indent + 1, ' ') + format(e->right, indent + 1) +
                '\n' + std::string(indent, ' ') + '}';
        }
    ), expr);
}

static std::string format(const ast::SyscallFilter& filter, unsigned indent)
{
    if (filter.body.size() == 0) {
        return filter.syscall;
    }

    std::string ret = filter.syscall + "(";
    for (const auto& p : filter.params) {
        ret.append(p + ", ");
    }

    if (filter.params.size() > 0) {
        ret.pop_back();
        ret.pop_back();
    }

    ret.append("){\n");

    for (const auto& e : filter.body) {
        ret.append(std::string(indent + 1, ' ') + format(e, indent + 1) + ",\n");
    }

    return ret + std::string(indent, ' ') + "}";
}

static std::string format(const ast::ActionBlock& block, unsigned indent)
{
    std::string indentstr(indent, ' ');
    std::string ret = format(block.action) + " {\n";
    for (const auto& f : block.filters) {
        ret.append(std::format("{} {},\n", indentstr, format(f, indent + 1)));
    }
    ret.append(indentstr + '}');
    return ret;
}

std::string format(const std::vector<ast::ProgramStatement>& program)
{
    std::string ret = "Program{\n";
    for (const auto& stmt : program) {
        ret.append(std::format(
            " {},\n",
            std::visit<std::string>(hana::overload(
                [](const ast::Policy& p) {
                    std::string ret = "Policy " + p.name + "{\n";
                    for (const auto& stmt : p.body) {
                        ret.append("  ");
                        ret.append(std::visit<std::string>(hana::overload(
                            [&](const ast::UseStatement& stmt) {
                                return std::format(
                                    "UseStatement{{{}}}", stmt.policy);
                            },
                            [&](const ast::ActionBlock& block) {
                                return format(block, /*indent=*/2) + ',';
                            }
                        ), stmt) + '\n');
                    }
                    return ret + " }";
                },
                [](const ast::DefaultAction& a) {
                    return std::format("DEFAULT={}", format(a)); },
                [](const ast::UseStatement& stmt) {
                    return std::format("UseStatement{{{}}}", stmt.policy); },
                [](const ast::ActionBlock& block) {
                    return format(block, /*indent=*/1) + ','; }
            ), stmt)));
    }
    ret.append("}");
    return ret;
}

} // namespace fekal
