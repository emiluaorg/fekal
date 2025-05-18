// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/recursion_context.hpp>
#include <fekal/parser.hpp>
#include <fekal/reader.hpp>

namespace fekal {

using recursion_context = basic_recursion_context<ast::Expr, reader>;
using OptExpr = std::optional<ast::Expr>;

template<class F>
static inline
OptExpr choice(const recursion_context& recur, reader& r, F&& f)
{
    return f(recur, r);
}

template<class F1, class F2, class... F3>
static inline
OptExpr choice(const recursion_context& recur, reader& r, F1&& f1, F2&& f2,
               F3&&... f3)
{
    auto backup = r;
    if (auto res = f1(recur, r) ; res) {
        return res;
    } else {
        r = backup;
        return choice(recur, r, std::forward<F2>(f2), std::forward<F3>(f3)...);
    }
}

// E = E, ("+" / "-"), T / T;
static OptExpr E(const recursion_context& recur, reader& r);

// T = int / "(", E, ")";
static OptExpr T(const recursion_context& recur, reader& r);

static OptExpr E(const recursion_context& recur, reader& r)
{
    return choice(
        recur, r,
        // E, ("+" / "-"), T
        [](const recursion_context& recur, reader& r) -> OptExpr {
            static constexpr auto OP_PLUS = token::symbol::OP_PLUS;
            static constexpr auto OP_MINUS = token::symbol::OP_MINUS;

            auto e = recur.enter<E>(r);
            if (!e || !r.next()) {
                return std::nullopt;
            }

            auto op = r.symbol();
            if ((op != OP_PLUS && op != OP_MINUS) || !r.next()) {
                return std::nullopt;
            }

            auto t = recur.enter<T>(r);
            if (!t) {
                return std::nullopt;
            }
            if (op == OP_PLUS) {
                return ast::SumExpr{std::move(*e), std::move(*t)};
            } else { assert(op == OP_MINUS);
                return ast::SubtractExpr{std::move(*e), std::move(*t)};
            }
        },
        // T
        [](const recursion_context& recur, reader& r) {
            return recur.enter<T>(r);
        });
}

static OptExpr T(const recursion_context& recur, reader& r)
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
        // "(", E, ")"
        [](const recursion_context& recur, reader& r) -> OptExpr {
            if (r.symbol() != token::symbol::LPAREN || !r.next()) {
                return std::nullopt;
            }

            auto e = recur.enter<E>(r);
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

    auto e = recursion_context{r}.enter<E>(r);
    if (!e) {
        throw std::runtime_error{"no match"};
    }

    if (r.next()) {
        throw std::runtime_error{"unexpected token"};
    }
    return *e;
}

} // namespace fekal
