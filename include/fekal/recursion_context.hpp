// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <optional>
#include <cassert>
#include <ranges>

namespace fekal {

// Handles left-recursion. Idea from the paper Left recursion in Parsing
// Expression Grammars by Medeiros et al.: <http://arxiv.org/pdf/1207.0443>.
//
// Summary:
//
// 1. Limit recursion to some bound.
// 2. Try again with a greater bound.
// 3. If nothing changes (i.e. amount of consumed tokens is the same), stop.
// 4. Otherwise, repeat from #2.
template<class T, class Reader, class Rules>
struct basic_recursion_context;

template<class T, class Reader, class Rules>
using recursion_context_fn_type =
    std::optional<T>(*)(
        const basic_recursion_context<T, Reader, Rules>&, Reader&);

template<
    class T, class Reader, class Rules,
    recursion_context_fn_type<T, Reader, Rules>
>
struct recursion_context_tagged_optional : std::optional<unsigned>
{};

template<class T, class Reader, class Rules>
struct basic_recursion_context
{
    using fn_type = recursion_context_fn_type<T, Reader, Rules>;

    basic_recursion_context(const Reader& reader) : reader{reader} {}

    template<fn_type Fn>
    std::optional<T> enter(Reader& reader) const
    {
        basic_recursion_context inner{reader};
        if (this->reader == reader) { // left-recursion
            if (const auto& lim = limit<Fn>() ; lim && *lim == 0) {
                return std::nullopt;
            }

            // inherit and enforce current limits
            inner = *this;

            if (auto& lim = inner.limit<Fn>() ; lim) {
                --*lim;
                return Fn(inner, reader);
            }
        } else {
            assert(this->reader < reader);
        }

        auto& limit = inner.limit<Fn>().emplace(0);
        auto backup = reader;
        auto last_res = Fn(inner, reader);
        if (!last_res) {
            return last_res;
        }

        for (unsigned lim : std::views::iota(1)) {
            limit = lim;
            auto r2 = backup;
            auto res = Fn(inner, r2);
            assert(res);
            if (reader < r2) {
                // we found more tokens in the new deeper iteration
                using std::swap;
                swap(last_res, res);
                reader = r2;
            } else {
                break;
            }
        }

        return last_res;
    }

    template<fn_type Fn>
    std::optional<unsigned>& limit()
    {
        using tagged_type = recursion_context_tagged_optional<
            T, Reader, Rules, Fn>;
        return static_cast<tagged_type&>(limits);
    }

    template<fn_type Fn>
    const std::optional<unsigned>& limit() const
    {
        using tagged_type = recursion_context_tagged_optional<
            T, Reader, Rules, Fn>;
        return static_cast<const tagged_type&>(limits);
    }

    Reader reader;
    Rules limits;
};

template<
    class T, class Reader, class Rules,
    recursion_context_fn_type<T, Reader, Rules> Tag,
    recursion_context_fn_type<T, Reader, Rules>... Tail
>
struct basic_recursion_context_rules;

template<
    class T, class Reader, class Rules,
    recursion_context_fn_type<T, Reader, Rules> Tag
>
struct basic_recursion_context_rules<T, Reader, Rules, Tag>
    : recursion_context_tagged_optional<T, Reader, Rules, Tag>
{};

template<
    class T, class Reader, class Rules,
    recursion_context_fn_type<T, Reader, Rules> Tag1,
    recursion_context_fn_type<T, Reader, Rules> Tag2,
    recursion_context_fn_type<T, Reader, Rules>... Tail
>
struct basic_recursion_context_rules<T, Reader, Rules, Tag1, Tag2, Tail...>
    : recursion_context_tagged_optional<T, Reader, Rules, Tag1>
    , basic_recursion_context_rules<T, Reader, Rules, Tag2, Tail...>
{};

} // namespace fekal
