// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <cstdint>
#include <string>

#include <boost/describe/enum.hpp>

#include <boost/hana/integral_constant.hpp>
#include <boost/hana/at_key.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/type.hpp>
#include <boost/hana/map.hpp>

namespace fekal::token {

BOOST_DEFINE_ENUM_CLASS(
    symbol,

    END, // EOF

    LPAREN, // (
    RPAREN, // )
    LBRACK, // [
    RBRACK, // ]
    LBRACE, // {
    RBRACE, // }
    COMMA, // ,
    AT, // @
    OP_NEG, // !
    OP_AND, // &&
    OP_OR, // ||
    OP_PLUS, // +
    OP_MINUS, // -
    OP_MUL, // *
    OP_DIV, // /
    OP_LSHIFT, // <<
    OP_RSHIFT, // >>
    OP_BAND, // &
    OP_BOR, // |
    OP_BXOR, // ^
    OP_EQ, // ==
    OP_NE, // !=
    OP_LT, // <
    OP_LTE, // <=
    OP_GT, // >
    OP_GTE, // >=
    LIT_BIN,
    LIT_OCT,
    LIT_DEC,
    LIT_HEX,
    KW_USE,
    KW_POLICY,
    KW_DEFAULT,
    KW_ALLOW,
    KW_LOG,
    KW_KILL_PROCESS,
    KW_KILL_THREAD,
    KW_USER_NOTIF,
    KW_ERRNO,
    KW_TRAP,
    KW_TRACE,
    IDENTIFIER,
)

template<token::symbol S>
using value_type = decltype(+boost::hana::make_map(
    boost::hana::make_pair(
        boost::hana::int_c<static_cast<int>(symbol::IDENTIFIER)>,
        boost::hana::type_c<std::string>),
    boost::hana::make_pair(
        boost::hana::int_c<static_cast<int>(symbol::LIT_BIN)>,
        boost::hana::type_c<std::int64_t>),
    boost::hana::make_pair(
        boost::hana::int_c<static_cast<int>(symbol::LIT_OCT)>,
        boost::hana::type_c<std::int64_t>),
    boost::hana::make_pair(
        boost::hana::int_c<static_cast<int>(symbol::LIT_DEC)>,
        boost::hana::type_c<std::int64_t>),
    boost::hana::make_pair(
        boost::hana::int_c<static_cast<int>(symbol::LIT_HEX)>,
        boost::hana::type_c<std::int64_t>)
)[boost::hana::int_c<static_cast<int>(S)>])::type;

} // namespace fekal::token
