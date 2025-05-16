// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/ast.hpp>
#include <boost/hana/functional/overload.hpp>

namespace fekal {

namespace hana = boost::hana;

int eval(const ast::Expr& e)
{
    return std::visit(hana::overload(
        [](const ast::IntLit& e) -> int { return e.value; },
        [](const ast::SumExpr& e) { return eval(*e.left) + eval(*e.right); },
        [](const ast::SubtractExpr& e) {
            return eval(*e.left) - eval(*e.right); },
        [](const ast::MulExpr& e) { return eval(*e.left) * eval(*e.right); }
    ), e);
}

} // namespace fekal
