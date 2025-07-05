// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/ast.hpp>
#include <boost/hana/functional/overload.hpp>

namespace fekal {

namespace hana = boost::hana;

static std::int64_t eval_int(const ast::IntExpr& e)
{
    return std::visit(hana::overload(
        [](const ast::IntLit& e) { return e.value; },
        [](const ast::Identifier&) -> std::int64_t { return 0; },
        [](const ast::SumExpr& e) {
            return eval_int(*e.left) + eval_int(*e.right); },
        [](const ast::SubtractExpr& e) {
            return eval_int(*e.left) - eval_int(*e.right); },
        [](const ast::MulExpr& e) {
            return eval_int(*e.left) * eval_int(*e.right); },
        [](const ast::DivExpr& e) {
            return eval_int(*e.left) / eval_int(*e.right); },
        [](const ast::LshiftExpr& e) {
            return eval_int(*e.left) << eval_int(*e.right); },
        [](const ast::RshiftExpr& e) {
            return eval_int(*e.left) >> eval_int(*e.right); },
        [](const ast::BitAndExpr& e) {
            return eval_int(*e.left) & eval_int(*e.right); },
        [](const ast::BitXorExpr& e) {
            return eval_int(*e.left) ^ eval_int(*e.right); },
        [](const ast::BitOrExpr& e) {
            return eval_int(*e.left) | eval_int(*e.right); }
    ), e);
}

bool eval(const ast::BoolExpr& e)
{
    return std::visit(hana::overload(
        [](const ast::EqExpr& e) {
            return eval_int(*e.left) == eval_int(*e.right); },
        [](const ast::NeqExpr& e) {
            return eval_int(*e.left) != eval_int(*e.right); },
        [](const ast::LtExpr& e) {
            return eval_int(*e.left) < eval_int(*e.right); },
        [](const ast::GtExpr& e) {
            return eval_int(*e.left) > eval_int(*e.right); },
        [](const ast::LteExpr& e) {
            return eval_int(*e.left) <= eval_int(*e.right); },
        [](const ast::GteExpr& e) {
            return eval_int(*e.left) >= eval_int(*e.right); },
        [](const ast::NegExpr& e) {
            return !eval(*e.inner); },
        [](const ast::AndExpr& e) {
            return (eval(*e.left) && eval(*e.right)); },
        [](const ast::OrExpr& e) {
            return (eval(*e.left) || eval(*e.right)); }
    ), e);
}

} // namespace fekal
