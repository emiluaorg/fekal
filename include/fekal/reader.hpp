// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/token.hpp>
#include <string_view>

namespace fekal {

// Rationale:
//
// - It doesn't perform syscalls/IO.
//
// - This reader isn't meant to work in a streaming fashion (e.g. input
//   containing half-tokens due to file being read in chunks). It's intended to
//   process file contents that are already fully read in advance. It doesn't
//   signalize "need more input".
//
// - Input doesn't need a NUL sentinel.
//
// - It doesn't allocate memory to match tokens. It might allocate memory to
//   decode tokens.
//
// - It's a pull parser.
//
// - You might make copies of the object to peek ahead.
//
// - A copy is exactly as cheap as a move. There's no need to worry about
//   move-casts.
class reader
{
public:
    reader(std::string_view input);

    reader(const reader&) = default;
    reader& operator=(const reader&) = default;

    token::symbol symbol() const;
    template<token::symbol S> token::value_type<S> value() const;
    unsigned line() const;
    unsigned column() const;
    std::string_view literal() const;
    std::string_view tail() const;

    bool next();

    // Undefined behavior if used against readers that never pointed to the same
    // input (nor were copies of such readers). {{{
    bool operator==(const reader& o) const;
    bool operator!=(const reader& o) const;

    // useful to test if one reader has consumed more tokens than another
    bool operator<(const reader&) const;
    // }}}

private:
    const char* begin;
    const char* end;
    const char* cursor;
    unsigned line_ = 1;
    unsigned column_ = 0;
    token::symbol symbol_;
};

static_assert(std::is_trivially_copy_constructible_v<reader> &&
              std::is_trivially_destructible_v<reader>);

template<>
std::string reader::value<token::symbol::IDENTIFIER>() const;

template<>
std::int64_t reader::value<token::symbol::LIT_BIN>() const;

template<>
std::int64_t reader::value<token::symbol::LIT_OCT>() const;

template<>
std::int64_t reader::value<token::symbol::LIT_DEC>() const;

template<>
std::int64_t reader::value<token::symbol::LIT_HEX>() const;

} // namespace fekal
