// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/parser.hpp>
#include <fekal/reader.hpp>
#include <fekal/peg.hpp>

namespace fekal {

namespace hana = boost::hana;
namespace mp11 = boost::mp11;

using peg::choice;

struct recursion_context_rules;
using recursion_context =
    peg::basic_recursion_context<reader, recursion_context_rules>;
using IntExprPtr = std::shared_ptr<ast::IntExpr>;
using BoolExprPtr = std::shared_ptr<ast::BoolExpr>;

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

static BoolExprPtr OrExpr(const recursion_context& recur, reader& r);
static BoolExprPtr AndExpr(const recursion_context& recur, reader& r);
static BoolExprPtr RelOpExpr(const recursion_context& recur, reader& r);
static IntExprPtr BitOrExpr(const recursion_context& recur, reader& r);
static IntExprPtr BitXorExpr(const recursion_context& recur, reader& r);
static IntExprPtr BitAndExpr(const recursion_context& recur, reader& r);
static IntExprPtr BitShiftExpr(const recursion_context& recur, reader& r);
static IntExprPtr SumExpr(const recursion_context& recur, reader& r);
static IntExprPtr MulExpr(const recursion_context& recur, reader& r);
static IntExprPtr Term(const recursion_context& recur, reader& r);

struct recursion_context_rules
    : peg::basic_recursion_context_rules<
        reader, recursion_context_rules,
        OrExpr, AndExpr, RelOpExpr, BitOrExpr, BitXorExpr, BitAndExpr,
        BitShiftExpr, SumExpr, MulExpr, Term>
{};

template<token::symbol S>
static inline bool expect(reader& r)
{
    if (S == token::symbol::END) {
        return r.symbol() == token::symbol::END;
    }

    if (r.symbol() == S) {
        r.next();
        return true;
    } else {
        return false;
    }
}

static inline std::optional<std::int64_t> INTEGER(reader& r)
{
    std::int64_t ret;
    if (r.symbol() == token::symbol::LIT_BIN) {
        ret = r.value<token::symbol::LIT_BIN>();
    } else if (r.symbol() == token::symbol::LIT_OCT) {
        ret = r.value<token::symbol::LIT_OCT>();
    } else if (r.symbol() == token::symbol::LIT_DEC) {
        ret = r.value<token::symbol::LIT_DEC>();
    } else if (r.symbol() == token::symbol::LIT_HEX) {
        ret = r.value<token::symbol::LIT_HEX>();
    } else {
        return std::nullopt;
    }
    r.next();
    return ret;
}

static
mp11::mp_push_front<ast::ProgramStatement, std::monostate>
ProgramStatement(const recursion_context& recur, reader& r)
{
    using Ret = mp11::mp_push_front<ast::ProgramStatement, std::monostate>;
    if (auto res = Policy(recur, r) ; res) {
        return *res;
    } else if (auto res = UseStatement(recur, r) ; res) {
        return *res;
    } else if (auto res = ActionBlock(recur, r) ; res) {
        return *res;
    } else if (auto backup = r ; expect<token::symbol::KW_DEFAULT>(r)) {
        return std::visit(hana::overload(
            [&](const std::monostate&) -> Ret {
                r = backup;
                return {};
            },
            [](const auto& v) -> Ret { return ast::DefaultAction{v}; }
        ), Action(recur, r));
    } else {
        return {};
    }
}

static
std::optional<ast::Policy> Policy(const recursion_context& recur, reader& r)
{
    auto backup = r;
    if (
        !expect<token::symbol::KW_POLICY>(r) ||
        r.symbol() != token::symbol::IDENTIFIER
    ) {
        r = backup;
        return std::nullopt;
    }

    std::string name = r.value<token::symbol::IDENTIFIER>();
    r.next();

    if (!expect<token::symbol::LBRACE>(r)) {
        r = backup;
        return std::nullopt;
    }

    std::vector<ast::PolicyStatement> stmts;
    for (;;) {
        bool found = std::visit(hana::overload(
            [](std::monostate&&) { return false; },
            [&](auto&& stmt) {
                stmts.emplace_back(std::move(stmt)); return true; }
        ), PolicyStatement(recur, r));
        if (found) {
            continue;
        } else {
            if (expect<token::symbol::RBRACE>(r)) {
                return ast::Policy{std::move(name), std::move(stmts)};
            } else {
                r = backup;
                return std::nullopt;
            }
        }
    }
}

static
mp11::mp_push_front<ast::PolicyStatement, std::monostate>
PolicyStatement(const recursion_context& recur, reader& r)
{
    if (auto res = UseStatement(recur, r) ; res) {
        return *res;
    } else if (auto res = ActionBlock(recur, r) ; res) {
        return *res;
    } else {
        return {};
    }
}

static
std::optional<ast::UseStatement>
UseStatement(const recursion_context& recur, reader& r)
{
    auto backup = r;
    if (
        !expect<token::symbol::KW_USE>(r) ||
        r.symbol() != token::symbol::IDENTIFIER
    ) {
        r = backup;
        return std::nullopt;
    }

    ast::UseStatement ret{r.value<token::symbol::IDENTIFIER>()};
    r.next();
    return ret;
}

static
std::optional<ast::ActionBlock>
ActionBlock(const recursion_context& recur, reader& r)
{
    auto backup = r;
    std::optional<ast::Action> action;
    std::visit(hana::overload(
        [](std::monostate&&) {},
        [&](auto&& a) { action.emplace(std::move(a)); }
    ), Action(recur, r));
    if (!action) {
        return std::nullopt;
    }

    if (!expect<token::symbol::LBRACE>(r)) {
        r = backup;
        return std::nullopt;
    }

    std::vector<ast::SyscallFilter> filters;
    for (;;) {
        auto filter = SyscallFilter(recur, r);
        if (filter) {
            filters.emplace_back(std::move(*filter));
            switch (r.symbol()) {
            default:
                r = backup;
                return std::nullopt;
            case token::symbol::COMMA:
                r.next();
                continue;
            case token::symbol::RBRACE:
                r.next();
                return ast::ActionBlock{std::move(*action), std::move(filters)};
            }
        } else {
            if (expect<token::symbol::RBRACE>(r)) {
                return ast::ActionBlock{std::move(*action), std::move(filters)};
            } else {
                r = backup;
                return std::nullopt;
            }
        }
    }
}

static
mp11::mp_push_front<ast::Action, std::monostate>
Action(const recursion_context& recur, reader& r)
{
    auto backup = r;
    switch (r.symbol()) {
    default:
        return {};
    case token::symbol::KW_ALLOW:
        r.next();
        return ast::ActionAllow{};
    case token::symbol::KW_LOG:
        r.next();
        return ast::ActionLog{};
    case token::symbol::KW_KILL_PROCESS:
        r.next();
        return ast::ActionKillProcess{};
    case token::symbol::KW_KILL_THREAD:
        r.next();
        return ast::ActionKillThread{};
    case token::symbol::KW_USER_NOTIF:
        r.next();
        return ast::ActionUserNotif{};
    case token::symbol::KW_ERRNO: {
        r.next();
        if (!expect<token::symbol::LPAREN>(r)) {
            r = backup;
            return {};
        }
        if (auto res = INTEGER(r) ; res) {
            if (!expect<token::symbol::RPAREN>(r)) {
                r = backup;
                return {};
            }
            return ast::ActionErrno{static_cast<int>(*res)};
        } else {
            // TODO: handle known errno constants
            r = backup;
            return {};
        }
    }
    case token::symbol::KW_TRAP:
        r.next();
        if (!expect<token::symbol::LPAREN>(r)) {
            r = backup;
            return {};
        }
        if (auto res = INTEGER(r) ; res) {
            if (!expect<token::symbol::RPAREN>(r)) {
                r = backup;
                return {};
            }
            return ast::ActionTrap{*res};
        } else {
            r = backup;
            return {};
        }
    case token::symbol::KW_TRACE:
        r.next();
        if (!expect<token::symbol::LPAREN>(r)) {
            r = backup;
            return {};
        }
        if (auto res = INTEGER(r) ; res) {
            if (!expect<token::symbol::RPAREN>(r)) {
                r = backup;
                return {};
            }
            return ast::ActionTrace{*res};
        } else {
            r = backup;
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
    r.next();

    auto return_matched = [&r,&syscall,backup=r]() {
        r = backup;
        return ast::SyscallFilter{std::move(syscall)};
    };

    if (!expect<token::symbol::LPAREN>(r)) {
        return return_matched();
    }

    std::vector<std::string> params;

    if (r.symbol() == token::symbol::IDENTIFIER) {
        params.emplace_back(r.value<token::symbol::IDENTIFIER>());
        r.next();
    }

    while (expect<token::symbol::COMMA>(r)) {
        if (r.symbol() != token::symbol::IDENTIFIER) {
            return return_matched();
        }
        params.emplace_back(r.value<token::symbol::IDENTIFIER>());
        r.next();
    }

    if (
        !expect<token::symbol::RPAREN>(r) || !expect<token::symbol::LBRACE>(r)
    ) {
        return return_matched();
    }

    std::vector<BoolExprPtr> body;
    for (;;) {
        auto expr = recur.enter<OrExpr>(r);
        if (expr) {
            body.emplace_back(std::move(expr));
            switch (r.symbol()) {
            default:
                return return_matched();
            case token::symbol::COMMA:
                r.next();
                continue;
            case token::symbol::RBRACE:
                r.next();
                return ast::SyscallFilter{
                    std::move(syscall), std::move(params), std::move(body)};
            }
        } else {
            if (expect<token::symbol::RBRACE>(r)) {
                return ast::SyscallFilter{
                    std::move(syscall), std::move(params), std::move(body)};
            } else {
                return return_matched();
            }
        }
    }
}

static BoolExprPtr OrExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // OrExpr '||' AndExpr
        [](const recursion_context& recur, reader& r) -> BoolExprPtr {
            auto e1 = recur.enter<OrExpr>(r);
            if (!e1) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (!expect<token::symbol::OP_OR>(r)) {
                return nullptr;
            }

            auto e2 = recur.enter<AndExpr>(r);
            if (!e2) {
                return nullptr;
            }
            return ast::make_bool_expr<ast::OrExpr>(
                l, c, std::move(e1), std::move(e2));
        },
        // AndExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<AndExpr>(r);
        });
}

static BoolExprPtr AndExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // AndExpr "&&" RelOpExpr
        [](const recursion_context& recur, reader& r) -> BoolExprPtr {
            auto e1 = recur.enter<AndExpr>(r);
            if (!e1) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (!expect<token::symbol::OP_AND>(r)) {
                return nullptr;
            }

            auto e2 = recur.enter<RelOpExpr>(r);
            if (!e2) {
                return nullptr;
            }
            return ast::make_bool_expr<ast::AndExpr>(
                l, c, std::move(e1), std::move(e2));
        },
        // RelOpExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<RelOpExpr>(r);
        });
}

static BoolExprPtr RelOpExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitOrExpr ("==" / "!=" / "<" / ">" / "<=" / ">=") BitOrExpr
        [](const recursion_context& recur, reader& r) -> BoolExprPtr {
            static constexpr auto OP_EQ = token::symbol::OP_EQ;
            static constexpr auto OP_NE = token::symbol::OP_NE;
            static constexpr auto OP_LT = token::symbol::OP_LT;
            static constexpr auto OP_GT = token::symbol::OP_GT;
            static constexpr auto OP_LTE = token::symbol::OP_LTE;
            static constexpr auto OP_GTE = token::symbol::OP_GTE;

            auto b1 = recur.enter<BitOrExpr>(r);
            if (!b1) {
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
            r.next();

            auto b2 = recur.enter<BitOrExpr>(r);
            if (!b2) {
                return nullptr;
            }
            switch (op) {
            case OP_EQ:
                return ast::make_bool_expr<ast::EqExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_NE:
                return ast::make_bool_expr<ast::NeqExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_LT:
                return ast::make_bool_expr<ast::LtExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_GT:
                return ast::make_bool_expr<ast::GtExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_LTE:
                return ast::make_bool_expr<ast::LteExpr>(
                    l, c, std::move(b1), std::move(b2));
            case OP_GTE:
                return ast::make_bool_expr<ast::GteExpr>(
                    l, c, std::move(b1), std::move(b2));
            default:
                assert(false);
            }
        },
        // '!'? '(' OrExpr ')'
        [](const recursion_context& recur, reader& r) -> BoolExprPtr {
            bool is_neg = r.symbol() == token::symbol::OP_NEG;
            auto l = r.line();
            auto c = r.column();
            if (is_neg) {
                r.next();
            }

            if (!expect<token::symbol::LPAREN>(r)) {
                return nullptr;
            }

            auto e = recur.enter<OrExpr>(r);
            if (!e || !expect<token::symbol::RPAREN>(r)) {
                return nullptr;
            }

            if (is_neg) {
                return ast::make_bool_expr<ast::NegExpr>(l, c, std::move(e));
            } else {
                return e;
            }
        });
}

static IntExprPtr BitOrExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitOrExpr '|' BitXorExpr
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            auto bo = recur.enter<BitOrExpr>(r);
            if (!bo) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (!expect<token::symbol::OP_BOR>(r)) {
                return nullptr;
            }

            auto bx = recur.enter<BitXorExpr>(r);
            if (!bx) {
                return nullptr;
            }
            return ast::make_int_expr<ast::BitOrExpr>(
                l, c, std::move(bo), std::move(bx));
        },
        // BitXorExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<BitXorExpr>(r);
        });
}

static IntExprPtr BitXorExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitXorExpr '^' BitAndExpr
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            auto bx = recur.enter<BitXorExpr>(r);
            if (!bx) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (!expect<token::symbol::OP_BXOR>(r)) {
                return nullptr;
            }

            auto ba = recur.enter<BitAndExpr>(r);
            if (!ba) {
                return nullptr;
            }
            return ast::make_int_expr<ast::BitXorExpr>(
                l, c, std::move(bx), std::move(ba));
        },
        // BitAndExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<BitAndExpr>(r);
        });
}

static IntExprPtr BitAndExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitAndExpr '&' BitShiftExpr
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            auto ba = recur.enter<BitAndExpr>(r);
            if (!ba) {
                return nullptr;
            }

            auto l = r.line();
            auto c = r.column();
            if (!expect<token::symbol::OP_BAND>(r)) {
                return nullptr;
            }

            auto bs = recur.enter<BitShiftExpr>(r);
            if (!bs) {
                return nullptr;
            }
            return ast::make_int_expr<ast::BitAndExpr>(
                l, c, std::move(ba), std::move(bs));
        },
        // BitShiftExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<BitShiftExpr>(r);
        });
}

static IntExprPtr BitShiftExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // BitShiftExpr ("<<" / ">>") SumExpr
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            static constexpr auto OP_LSHIFT = token::symbol::OP_LSHIFT;
            static constexpr auto OP_RSHIFT = token::symbol::OP_RSHIFT;

            auto b = recur.enter<BitShiftExpr>(r);
            if (!b) {
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
                return ast::make_int_expr<ast::LshiftExpr>(
                    l, c, std::move(b), std::move(s));
            } else { assert(op == OP_RSHIFT);
                return ast::make_int_expr<ast::RshiftExpr>(
                    l, c, std::move(b), std::move(s));
            }
        },
        // SumExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<SumExpr>(r);
        });
}

static IntExprPtr SumExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // SumExpr ('+' / '-') MulExpr
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            static constexpr auto OP_PLUS = token::symbol::OP_PLUS;
            static constexpr auto OP_MINUS = token::symbol::OP_MINUS;

            auto s = recur.enter<SumExpr>(r);
            if (!s) {
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
                return ast::make_int_expr<ast::SumExpr>(
                    l, c, std::move(s), std::move(m));
            } else { assert(op == OP_MINUS);
                return ast::make_int_expr<ast::SubtractExpr>(
                    l, c, std::move(s), std::move(m));
            }
        },
        // MulExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<MulExpr>(r);
        });
}

static IntExprPtr MulExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // MulExpr ('*' / '/') Term
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            static constexpr auto OP_MUL = token::symbol::OP_MUL;
            static constexpr auto OP_DIV = token::symbol::OP_DIV;

            auto m = recur.enter<MulExpr>(r);
            if (!m) {
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
                return ast::make_int_expr<ast::MulExpr>(
                    l, c, std::move(m), std::move(t));
            } else { assert(op == OP_DIV);
                return ast::make_int_expr<ast::DivExpr>(
                    l, c, std::move(m), std::move(t));
            }
        },
        // Term
        [](const recursion_context& recur, reader& r) {
            return recur.enter<Term>(r);
        });
}

static IntExprPtr Term(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // INTEGER
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            auto l = r.line();
            auto c = r.column();
            if (auto res = INTEGER(r) ; res) {
                return ast::make_int_expr<ast::IntLit>(l, c, *res);
            } else {
                return nullptr;
            }
        },
        // IDENTIFIER
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            if (r.symbol() == token::symbol::IDENTIFIER) {
                auto ret = ast::make_int_expr<ast::Identifier>(
                    r.line(), r.column(), r.value<token::symbol::IDENTIFIER>());
                r.next();
                return ret;
            } else {
                return nullptr;
            }
        },
        // '(' BitOrExpr ')'
        [](const recursion_context& recur, reader& r) -> IntExprPtr {
            if (!expect<token::symbol::LPAREN>(r)) {
                return nullptr;
            }

            auto e = recur.enter<BitOrExpr>(r);
            if (!e || !expect<token::symbol::RPAREN>(r)) {
                return nullptr;
            }

            return e;
        });
}

std::vector<ast::ProgramStatement> parse(std::string_view input)
{
    std::vector<ast::ProgramStatement> ret;

    for (reader r{input} ; r.symbol() != token::symbol::END ;) {
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
