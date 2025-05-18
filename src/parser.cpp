// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/recursion_context.hpp>
#include <fekal/parser.hpp>
#include <fekal/reader.hpp>
#include <fekal/peg.hpp>

namespace fekal {

struct recursion_context_rules;
using recursion_context =
    basic_recursion_context<ast::Expr, reader, recursion_context_rules>;
using OptExpr = std::optional<ast::Expr>;

// SumExpr = SumExpr, ("+" / "-"), SumExpr / MulExpr;
static OptExpr SumExpr(const recursion_context& recur, reader& r);

// MulExpr = MulExpr, ("*" / "/"), MulExpr / Term;
static OptExpr MulExpr(const recursion_context& recur, reader& r);

// Term = int / "(", SumExpr, ")";
static OptExpr Term(const recursion_context& recur, reader& r);

struct recursion_context_rules
    : basic_recursion_context_rules<
        ast::Expr, reader, recursion_context_rules,
        SumExpr, MulExpr, Term>
{};

static OptExpr SumExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // SumExpr, ("+" / "-"), SumExpr
        [](const recursion_context& recur, reader& r) -> OptExpr {
            static constexpr auto OP_PLUS = token::symbol::OP_PLUS;
            static constexpr auto OP_MINUS = token::symbol::OP_MINUS;

            auto s1 = recur.enter<SumExpr>(r);
            if (!s1 || !r.next()) {
                return std::nullopt;
            }

            auto op = r.symbol();
            if ((op != OP_PLUS && op != OP_MINUS) || !r.next()) {
                return std::nullopt;
            }

            auto s2 = recur.right1<SumExpr>(r);
            if (!s2) {
                return std::nullopt;
            }
            if (op == OP_PLUS) {
                return ast::SumExpr{std::move(*s1), std::move(*s2)};
            } else { assert(op == OP_MINUS);
                return ast::SubtractExpr{std::move(*s1), std::move(*s2)};
            }
        },
        // MulExpr
        [](const recursion_context& recur, reader& r) {
            return recur.enter<MulExpr>(r);
        });
}

static OptExpr MulExpr(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // MulExpr, ("*" / "/"), MulExpr
        [](const recursion_context& recur, reader& r) -> OptExpr {
            static constexpr auto OP_MUL = token::symbol::OP_MUL;
            static constexpr auto OP_DIV = token::symbol::OP_DIV;

            auto m1 = recur.enter<MulExpr>(r);
            if (!m1 || !r.next()) {
                return std::nullopt;
            }

            auto op = r.symbol();
            if ((op != OP_MUL && op != OP_DIV) || !r.next()) {
                return std::nullopt;
            }

            auto m2 = recur.right1<MulExpr>(r);
            if (!m2) {
                return std::nullopt;
            }
            if (op == OP_MUL) {
                return ast::MulExpr{std::move(*m1), std::move(*m2)};
            } else { assert(op == OP_DIV);
                return ast::DivExpr{std::move(*m1), std::move(*m2)};
            }
        },
        // Term
        [](const recursion_context& recur, reader& r) {
            return recur.enter<Term>(r);
        });
}

static OptExpr Term(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // int
        [](const recursion_context& recur, reader& r) -> OptExpr {
            if (r.symbol() == token::symbol::LIT_INT) {
                return ast::IntLit{r.value<token::symbol::LIT_INT>()};
            } else {
                return std::nullopt;
            }
        },
        // "(", SumExpr, ")"
        [](const recursion_context& recur, reader& r) -> OptExpr {
            if (r.symbol() != token::symbol::LPAREN || !r.next()) {
                return std::nullopt;
            }

            auto e = recur.enter<SumExpr>(r);
            if (!e || !r.next() || r.symbol() != token::symbol::RPAREN) {
                return std::nullopt;
            }

            return e;
        });
}

ast::Expr parse(std::string_view input)
{
    reader r{input};
    if (!r.next()) {
        throw std::runtime_error{"empty tree"};
    }

    auto e = recursion_context{r}.enter<SumExpr>(r);
    if (!e) {
        throw std::runtime_error{"no match"};
    }

    if (r.next()) {
        throw std::runtime_error{"unexpected token"};
    }
    return *e;
}

} // namespace fekal
