// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#include <fekal/ast.hpp>
#include <boost/hana/functional/overload.hpp>

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

} // namespace fekal
