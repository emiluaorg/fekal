// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <boost/coroutine2/coroutine.hpp>

#include <fekal/recursion_context.hpp>
#include <fekal/parser.hpp>
#include <fekal/reader.hpp>

namespace fekal {

using coro_type = boost::coroutines2::coroutine<std::pair<ast::Expr, reader>>;
using coro_pull_type = coro_type::pull_type;
using coro_push_type = coro_type::push_type;

using recursion_context = basic_recursion_context<ast::Expr, reader>;

static inline
void choice(const recursion_context&, const reader&, coro_push_type&)
{}

template<class F1, class... F2>
static inline
void choice(const recursion_context& recur, const reader& r,
            coro_push_type& yield, F1&& f1, F2&&... f2)
{
    f1(recur, r, yield);
    choice(recur, r, yield, std::forward<F2>(f2)...);
}

// E = E, "+", T | E, "-", T | T;
static void E(const recursion_context& recur, reader r, coro_push_type& yield);

// T = int | "(", E, ")";
static void T(const recursion_context& recur, reader r, coro_push_type& yield);

static void E(const recursion_context& recur, reader r, coro_push_type& yield)
{
    return choice(
        recur, r, yield,
        // E "+" T | E "-" T
        [](const recursion_context& recur, const reader& r, auto& yield) {
            static constexpr auto OP_PLUS = token::symbol::OP_PLUS;
            static constexpr auto OP_MINUS = token::symbol::OP_MINUS;

            for (auto& [e, r] : coro_pull_type{recur.wrap<E>(r)}) {
                if (!r.next()) {
                    continue;
                }

                auto op = r.symbol();
                if ((op != OP_PLUS && op != OP_MINUS) || !r.next()) {
                    continue;
                }
                if (op == OP_PLUS) {
                    for (auto& [t, r] : coro_pull_type{recur.wrap<T>(r)}) {
                        yield(std::make_pair(ast::SumExpr{e, std::move(t)}, r));
                    }
                } else { assert(op == OP_MINUS);
                    for (auto& [t, r] : coro_pull_type{recur.wrap<T>(r)}) {
                        yield(std::make_pair(
                            ast::SubtractExpr{e, std::move(t)}, r));
                    }
                }
            }
        },
        // T
        [](const recursion_context& recur, const reader& r, auto& yield) {
            recur.wrap<T>(r)(yield);
        });
}

static void T(const recursion_context& recur, reader r, coro_push_type& yield)
{
    return choice(
        recur, r, yield,
        // int
        [](const recursion_context& recur, const reader& r, auto& yield) {
            if (r.symbol() == token::symbol::LIT_INT) {
                yield(std::make_pair(
                    ast::IntLit{r.value<token::symbol::LIT_INT>()}, r));
            }
        },
        // "(", E, ")"
        [](const recursion_context& recur, reader r, auto& yield) {
            if (r.symbol() != token::symbol::LPAREN || !r.next()) {
                return;
            }

            for (auto& e : coro_pull_type{recur.wrap<E>(r)}) {
                auto& r = e.second;
                if (!r.next() || r.symbol() != token::symbol::RPAREN) {
                    continue;
                }
                yield(std::move(e));
            }
        });
}

ast::Expr parse(std::string_view input)
{
    reader r{input};
    if (!r.next()) {
        throw std::runtime_error{"empty tree"};
    }

    for (auto& [e, r] : coro_pull_type{recursion_context{r}.wrap<E>(r)}) {
        if (!r.next()) {
            return e;
        }
    }

    throw std::runtime_error{"no match"};
}

} // namespace fekal
