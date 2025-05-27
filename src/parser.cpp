// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/recursion_context.hpp>
#include <fekal/parser.hpp>
#include <fekal/reader.hpp>
#include <fekal/peg.hpp>

namespace fekal {

namespace hana = boost::hana;
namespace mp11 = boost::mp11;

struct recursion_context_rules;
using recursion_context =
    basic_recursion_context<ast::Expr, reader, recursion_context_rules>;
using ExprPtr = std::shared_ptr<ast::Expr>;

static
mp11::mp_push_front<ast::ProgramStatement, std::monostate>
ProgramStatement(const recursion_context& recur, reader& r);

static
std::optional<ast::Policy> Policy(const recursion_context& recur, reader& r);

static
mp11::mp_push_front<ast::PolicyStatement, std::monostate>
PolicyStatement(const recursion_context& recur, reader& r);

static
std::optional<ast::UseStatement>
UseStatement(const recursion_context& recur, reader& r);

static
std::optional<ast::ActionBlock>
ActionBlock(const recursion_context& recur, reader& r);

static
mp11::mp_push_front<ast::Action, std::monostate>
Action(const recursion_context& recur, reader& r);

static
std::optional<ast::SyscallFilter>
SyscallFilter(const recursion_context& recur, reader& r);

static ExprPtr OrExpr(const recursion_context& recur, reader& r);
static ExprPtr AndExpr(const recursion_context& recur, reader& r);
static ExprPtr RelOpExpr(const recursion_context& recur, reader& r);
static ExprPtr BitOrExpr(const recursion_context& recur, reader& r);
static ExprPtr BitXorExpr(const recursion_context& recur, reader& r);
static ExprPtr BitAndExpr(const recursion_context& recur, reader& r);
static ExprPtr BitShiftExpr(const recursion_context& recur, reader& r);
static ExprPtr SumExpr(const recursion_context& recur, reader& r);
static ExprPtr MulExpr(const recursion_context& recur, reader& r);
static ExprPtr Term(const recursion_context& recur, reader& r);

struct recursion_context_rules
    : basic_recursion_context_rules<
        ast::Expr, reader, recursion_context_rules,
        OrExpr, AndExpr, RelOpExpr, BitOrExpr, BitXorExpr, BitAndExpr,
        BitShiftExpr, SumExpr, MulExpr, Term>
{};

static std::optional<std::int64_t> INTEGER(const reader& r)
{
    if (r.symbol() == token::symbol::LIT_BIN) {
        return r.value<token::symbol::LIT_BIN>();
    } else if (r.symbol() == token::symbol::LIT_OCT) {
        return r.value<token::symbol::LIT_OCT>();
    } else if (r.symbol() == token::symbol::LIT_DEC) {
        return r.value<token::symbol::LIT_DEC>();
    } else if (r.symbol() == token::symbol::LIT_HEX) {
        return r.value<token::symbol::LIT_HEX>();
    } else {
        return std::nullopt;
    }
}

static
mp11::mp_push_front<ast::ProgramStatement, std::monostate>
ProgramStatement(const recursion_context& recur, reader& r)
{
    using Ret = mp11::mp_push_front<ast::ProgramStatement, std::monostate>;
    auto backup = r;
    if (auto res = Policy(recur, r) ; res) {
        return *res;
    } else if (auto res = ((r = backup), UseStatement(recur, r)) ; res) {
        return *res;
    } else if (auto res = ((r = backup), ActionBlock(recur, r)) ; res) {
        return *res;
    } else if (backup.symbol() == token::symbol::KW_DEFAULT) {
        r = backup;
        if (!r.next()) {
            return {};
        }
        return std::visit(hana::overload(
            [](const std::monostate&) -> Ret { return {}; },
            [](const auto& v) -> Ret { return ast::DefaultAction{v}; }
        ), Action(recur, r));
    } else {
        return {};
    }
}

static
std::optional<ast::Policy> Policy(const recursion_context& recur, reader& r)
{
    if (
        r.symbol() != token::symbol::KW_POLICY || !r.next() ||
        r.symbol() != token::symbol::IDENTIFIER
    ) {
        return std::nullopt;
    }

    std::string name = r.value<token::symbol::IDENTIFIER>();

    if (!r.next() || r.symbol() != token::symbol::LBRACE || !r.next()) {
        return std::nullopt;
    }

    std::vector<ast::PolicyStatement> stmts;
    for (;;) {
        auto backup = r;
        bool found = std::visit(hana::overload(
            [](std::monostate&&) { return false; },
            [&](auto&& stmt) {
                stmts.emplace_back(std::move(stmt)); return true; }
        ), PolicyStatement(recur, r));
        if (found) {
            if (!r.next()) {
                return std::nullopt;
            }
            switch (r.symbol()) {
            default:
                return std::nullopt;
            case token::symbol::COMMA:
                if (r.next()) {
                    continue;
                } else {
                    return std::nullopt;
                }
            case token::symbol::RBRACE:
                return ast::Policy{std::move(name), std::move(stmts)};
            }
        } else {
            r = backup;
            if (r.symbol() == token::symbol::RBRACE) {
                return ast::Policy{std::move(name), std::move(stmts)};
            } else {
                return std::nullopt;
            }
        }
    }
}

static
mp11::mp_push_front<ast::PolicyStatement, std::monostate>
PolicyStatement(const recursion_context& recur, reader& r)
{
    auto backup = r;
    if (auto res = UseStatement(recur, r) ; res) {
        return *res;
    } else if (auto res = ((r = backup), ActionBlock(recur, r)) ; res) {
        return *res;
    } else {
        return {};
    }
}

static
std::optional<ast::UseStatement>
UseStatement(const recursion_context& recur, reader& r)
{
    if (
        r.symbol() != token::symbol::KW_USE || !r.next() ||
        r.symbol() != token::symbol::IDENTIFIER
    ) {
        return std::nullopt;
    }

    return ast::UseStatement{r.value<token::symbol::IDENTIFIER>()};
}

static
std::optional<ast::ActionBlock>
ActionBlock(const recursion_context& recur, reader& r)
{
    std::optional<ast::Action> action;
    std::visit(hana::overload(
        [](std::monostate&&) {},
        [&](auto&& a) { action.emplace(std::move(a)); }
    ), Action(recur, r));
    if (!action) {
        return std::nullopt;
    }

    if (!r.next() || r.symbol() != token::symbol::LBRACE || !r.next()) {
        return std::nullopt;
    }

    std::vector<ast::SyscallFilter> filters;
    for (;;) {
        auto backup = r;
        auto filter = SyscallFilter(recur, r);
        if (filter) {
            filters.emplace_back(std::move(*filter));
            if (!r.next()) {
                return std::nullopt;
            }
            switch (r.symbol()) {
            default:
                return std::nullopt;
            case token::symbol::COMMA:
                if (r.next()) {
                    continue;
                } else {
                    return std::nullopt;
                }
            case token::symbol::RBRACE:
                return ast::ActionBlock{std::move(*action), std::move(filters)};
            }
        } else {
            r = backup;
            if (r.symbol() == token::symbol::RBRACE) {
                return ast::ActionBlock{std::move(*action), std::move(filters)};
            } else {
                return std::nullopt;
            }
        }
    }
}

static
mp11::mp_push_front<ast::Action, std::monostate>
Action(const recursion_context& recur, reader& r)
{
    switch (r.symbol()) {
    default:
        return {};
    case token::symbol::KW_ALLOW:
        return ast::ActionAllow{};
    case token::symbol::KW_LOG:
        return ast::ActionLog{};
    case token::symbol::KW_KILL_PROCESS:
        return ast::ActionKillProcess{};
    case token::symbol::KW_KILL_THREAD:
        return ast::ActionKillThread{};
    case token::symbol::KW_USER_NOTIF:
        return ast::ActionUserNotif{};
    case token::symbol::KW_ERRNO: {
        if (
            !r.next() || r.symbol() != token::symbol::LPAREN || !r.next()
        ) {
            return {};
        }
        if (auto res = INTEGER(r) ; res) {
            if (!r.next() || r.symbol() != token::symbol::RPAREN) {
                return {};
            }
            return ast::ActionErrno{static_cast<int>(*res)};
        } else {
            // TODO: handle known errno constants
            return {};
        }
    }
    case token::symbol::KW_TRAP:
        if (!r.next() || r.symbol() != token::symbol::LPAREN || !r.next()) {
            return {};
        }
        if (auto res = INTEGER(r) ; res) {
            if (!r.next() || r.symbol() != token::symbol::RPAREN) {
                return {};
            }
            return ast::ActionTrap{*res};
        } else {
            return {};
        }
    case token::symbol::KW_TRACE:
        if (!r.next() || r.symbol() != token::symbol::LPAREN || !r.next()) {
            return {};
        }
        if (auto res = INTEGER(r) ; res) {
            if (!r.next() || r.symbol() != token::symbol::RPAREN) {
                return {};
            }
            return ast::ActionTrace{*res};
        } else {
            return {};
        }
    }
}

static
std::optional<ast::SyscallFilter>
SyscallFilter(const recursion_context& recur, reader& r)
{
    if (r.symbol() != token::symbol::IDENTIFIER) {
        return std::nullopt;
    }

    std::string syscall = r.value<token::symbol::IDENTIFIER>();

    auto return_matched = [&r,&syscall,backup=r]() {
        r = backup;
        return ast::SyscallFilter{std::move(syscall)};
    };

    if (!r.next() || r.symbol() != token::symbol::LPAREN || !r.next()) {
        return return_matched();
    }

    std::vector<std::string> params;

    if (r.symbol() == token::symbol::IDENTIFIER) {
        params.emplace_back(r.value<token::symbol::IDENTIFIER>());
        if (!r.next()) {
            return return_matched();
        }
    }

    while (r.symbol() == token::symbol::COMMA) {
        if (!r.next() || r.symbol() != token::symbol::IDENTIFIER) {
            return return_matched();
        }
        params.emplace_back(r.value<token::symbol::IDENTIFIER>());
        if (!r.next()) {
            return return_matched();
        }
    }

    if (
        r.symbol() != token::symbol::RPAREN || !r.next() ||
        r.symbol() != token::symbol::LBRACE || !r.next()
    ) {
        return return_matched();
    }

    std::vector<ast::SyscallFilter::expr_type> body;
    for (;;) {
        auto backup = r;
        auto expr = recur.enter<OrExpr>(r);
        if (expr) {
            body.emplace_back(ast::unwrap_bool_expr(expr));
            if (!r.next()) {
                return return_matched();
            }
            switch (r.symbol()) {
            default:
                return return_matched();
            case token::symbol::COMMA:
                if (r.next()) {
                    continue;
                } else {
                    return return_matched();
                }
            case token::symbol::RBRACE:
                return ast::SyscallFilter{
                    std::move(syscall), std::move(params), std::move(body)};
            }
        } else {
            r = backup;
            if (r.symbol() == token::symbol::RBRACE) {
                return ast::SyscallFilter{
                    std::move(syscall), std::move(params), std::move(body)};
            } else {
                return return_matched();
            }
        }
    }
}

static ExprPtr OrExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // OrExpr '||' AndExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            auto e1 = recur.enter<OrExpr>(r);
            if (!e1 || !r.next()) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (r.symbol() != token::symbol::OP_OR || !r.next()) {
                return nullptr;
            }

            auto e2 = recur.enter<AndExpr>(r);
            if (!e2) {
                return nullptr;
            }
            return ast::make_expr<ast::OrExpr>(
                l, c, std::move(e1), std::move(e2));
        },
        // AndExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<AndExpr>(r);
        });
}

static ExprPtr AndExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // AndExpr "&&" RelOpExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            auto e1 = recur.enter<AndExpr>(r);
            if (!e1 || !r.next()) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (r.symbol() != token::symbol::OP_AND || !r.next()) {
                return nullptr;
            }

            auto e2 = recur.enter<RelOpExpr>(r);
            if (!e2) {
                return nullptr;
            }
            return ast::make_expr<ast::AndExpr>(
                l, c, std::move(e1), std::move(e2));
        },
        // RelOpExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<RelOpExpr>(r);
        });
}

static ExprPtr RelOpExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitOrExpr ("==" / "!=" / "<" / ">" / "<=" / ">=") BitOrExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            static constexpr auto OP_EQ = token::symbol::OP_EQ;
            static constexpr auto OP_NE = token::symbol::OP_NE;
            static constexpr auto OP_LT = token::symbol::OP_LT;
            static constexpr auto OP_GT = token::symbol::OP_GT;
            static constexpr auto OP_LTE = token::symbol::OP_LTE;
            static constexpr auto OP_GTE = token::symbol::OP_GTE;

            auto b1 = recur.enter<BitOrExpr>(r);
            if (!b1 || !r.next()) {
                return nullptr;
            }

            auto op = r.symbol();
            auto l = r.line();
            auto c = r.column();
            switch (op) {
            default:
                return nullptr;
            case OP_EQ:
            case OP_NE:
            case OP_LT:
            case OP_GT:
            case OP_LTE:
            case OP_GTE:
                break;
            }
            if (!r.next()) {
                return nullptr;
            }

            auto b2 = recur.enter<BitOrExpr>(r);
            if (!b2) {
                return nullptr;
            }
            switch (op) {
            case OP_EQ:
                return ast::make_expr<ast::EqExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_NE:
                return ast::make_expr<ast::NeqExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_LT:
                return ast::make_expr<ast::LtExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_GT:
                return ast::make_expr<ast::GtExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_LTE:
                return ast::make_expr<ast::LteExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_GTE:
                return ast::make_expr<ast::GteExpr>(
                    l, c, std::move(b1), std::move(b2));
            default:
                assert(false);
            }
        },
        // '!'? '(' OrExpr ')'
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            bool is_neg = r.symbol() == token::symbol::OP_NEG;
            auto l = r.line();
            auto c = r.column();
            if (is_neg && !r.next()) {
                return nullptr;
            }

            if (r.symbol() != token::symbol::LPAREN || !r.next()) {
                return nullptr;
            }

            auto e = recur.enter<OrExpr>(r);
            if (!e || !r.next() || r.symbol() != token::symbol::RPAREN) {
                return nullptr;
            }

            if (is_neg) {
                return ast::make_expr<ast::NegExpr>(l, c, std::move(e));
            } else {
                return e;
            }
        });
}

static ExprPtr BitOrExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitOrExpr '|' BitXorExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            auto bo = recur.enter<BitOrExpr>(r);
            if (!bo || !r.next()) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (r.symbol() != token::symbol::OP_BOR || !r.next()) {
                return nullptr;
            }

            auto bx = recur.enter<BitXorExpr>(r);
            if (!bx) {
                return nullptr;
            }
            return ast::make_expr<ast::BitOrExpr>(
                l, c, std::move(bo), std::move(bx));
        },
        // BitXorExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<BitXorExpr>(r);
        });
}

static ExprPtr BitXorExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitXorExpr '^' BitAndExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            auto bx = recur.enter<BitXorExpr>(r);
            if (!bx || !r.next()) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (r.symbol() != token::symbol::OP_BXOR || !r.next()) {
                return nullptr;
            }

            auto ba = recur.enter<BitAndExpr>(r);
            if (!ba) {
                return nullptr;
            }
            return ast::make_expr<ast::BitXorExpr>(
                l, c, std::move(bx), std::move(ba));
        },
        // BitAndExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<BitAndExpr>(r);
        });
}

static ExprPtr BitAndExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitAndExpr '&' BitShiftExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            auto ba = recur.enter<BitAndExpr>(r);
            if (!ba || !r.next()) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (r.symbol() != token::symbol::OP_BAND || !r.next()) {
                return nullptr;
            }

            auto bs = recur.enter<BitShiftExpr>(r);
            if (!bs) {
                return nullptr;
            }
            return ast::make_expr<ast::BitAndExpr>(
                l, c, std::move(ba), std::move(bs));
        },
        // BitShiftExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<BitShiftExpr>(r);
        });
}

static ExprPtr BitShiftExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitShiftExpr ("<<" / ">>") SumExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            static constexpr auto OP_LSHIFT = token::symbol::OP_LSHIFT;
            static constexpr auto OP_RSHIFT = token::symbol::OP_RSHIFT;

            auto b = recur.enter<BitShiftExpr>(r);
            if (!b || !r.next()) {
                return nullptr;
            }

            auto op = r.symbol();
            auto l = r.line();
            auto c = r.column();
            if ((op != OP_LSHIFT && op != OP_RSHIFT) || !r.next()) {
                return nullptr;
            }

            auto s = recur.enter<SumExpr>(r);
            if (!s) {
                return nullptr;
            }
            if (op == OP_LSHIFT) {
                return ast::make_expr<ast::LshiftExpr>(
                    l, c, std::move(b), std::move(s));
            } else { assert(op == OP_RSHIFT);
                return ast::make_expr<ast::RshiftExpr>(
                    l, c, std::move(b), std::move(s));
            }
        },
        // SumExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<SumExpr>(r);
        });
}

static ExprPtr SumExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // SumExpr ('+' / '-') MulExpr
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            static constexpr auto OP_PLUS = token::symbol::OP_PLUS;
            static constexpr auto OP_MINUS = token::symbol::OP_MINUS;

            auto s = recur.enter<SumExpr>(r);
            if (!s || !r.next()) {
                return nullptr;
            }

            auto op = r.symbol();
            auto l = r.line();
            auto c = r.column();
            if ((op != OP_PLUS && op != OP_MINUS) || !r.next()) {
                return nullptr;
            }

            auto m = recur.enter<MulExpr>(r);
            if (!m) {
                return nullptr;
            }
            if (op == OP_PLUS) {
                return ast::make_expr<ast::SumExpr>(
                    l, c, std::move(s), std::move(m));
            } else { assert(op == OP_MINUS);
                return ast::make_expr<ast::SubtractExpr>(
                    l, c, std::move(s), std::move(m));
            }
        },
        // MulExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<MulExpr>(r);
        });
}

static ExprPtr MulExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // MulExpr ('*' / '/') Term
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            static constexpr auto OP_MUL = token::symbol::OP_MUL;
            static constexpr auto OP_DIV = token::symbol::OP_DIV;

            auto m = recur.enter<MulExpr>(r);
            if (!m || !r.next()) {
                return nullptr;
            }

            auto op = r.symbol();
            auto l = r.line();
            auto c = r.column();
            if ((op != OP_MUL && op != OP_DIV) || !r.next()) {
                return nullptr;
            }

            auto t = recur.enter<Term>(r);
            if (!t) {
                return nullptr;
            }
            if (op == OP_MUL) {
                return ast::make_expr<ast::MulExpr>(
                    l, c, std::move(m), std::move(t));
            } else { assert(op == OP_DIV);
                return ast::make_expr<ast::DivExpr>(
                    l, c, std::move(m), std::move(t));
            }
        },
        // Term
        [](const recursion_context& recur, reader& r) {
            return recur.enter<Term>(r);
        });
}

static ExprPtr Term(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // INTEGER
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            if (auto res = INTEGER(r) ; res) {
                return ast::make_expr<ast::IntLit>(r.line(), r.column(), *res);
            } else {
                return nullptr;
            }
        },
        // IDENTIFIER
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            if (r.symbol() == token::symbol::IDENTIFIER) {
                return ast::make_expr<ast::Identifier>(
                    r.line(), r.column(), r.value<token::symbol::IDENTIFIER>());
            } else {
                return nullptr;
            }
        },
        // '(' BitOrExpr ')'
        [](const recursion_context& recur, reader& r) -> ExprPtr {
            if (r.symbol() != token::symbol::LPAREN || !r.next()) {
                return nullptr;
            }

            auto e = recur.enter<BitOrExpr>(r);
            if (!e || !r.next() || r.symbol() != token::symbol::RPAREN) {
                return nullptr;
            }

            return e;
        });
}

std::vector<ast::ProgramStatement> parse(std::string_view input)
{
    std::vector<ast::ProgramStatement> ret;
    reader r{input};

    while (r.next()) {
#if defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        recursion_context recur{r};
#else // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        recursion_context::cache_type parse_cache;
        recursion_context recur{parse_cache, r};
#endif // defined(FEKAL_DISABLE_PEG_MEMOIZATION)

        std::visit(hana::overload(
            [](std::monostate&&) {
                throw std::runtime_error{"no match"}; },
            [&](auto&& stmt) { ret.emplace_back(std::move(stmt)); }
        ), ProgramStatement(recur, r));
    }

    return ret;
}

} // namespace fekal
