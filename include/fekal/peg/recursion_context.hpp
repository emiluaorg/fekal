// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <type_traits>
#include <optional>
#include <cassert>

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
#include <unordered_map>
#include <vector>
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)

namespace fekal::peg {

// Handles left-recursion. Idea from the paper Left recursion in Parsing
// Expression Grammars by Medeiros et al.: <http://arxiv.org/pdf/1207.0443>.
//
// Summary:
//
// 1. Limit recursion to some bound.
// 2. Try again with a greater bound.
// 3. If nothing changes (i.e. amount of consumed tokens is the same), stop.
// 4. Otherwise, repeat from #2.
template<class Reader, class Rules>
struct basic_recursion_context;

template<class Reader, class Rules, auto Fn>
using recursion_context_return_type = std::invoke_result_t<
    decltype(Fn), const basic_recursion_context<Reader, Rules>&, Reader&>;

template<class Reader, class Rules, auto, class Type>
struct recursion_context_tagged_type : Type
{};

template<class Reader, class Rules, auto Fn>
using recursion_context_tagged_optional = recursion_context_tagged_type<
    Reader, Rules, Fn, std::optional<unsigned>>;

template<class Reader, class Rules>
struct basic_recursion_context
{
    template<auto Fn>
    using return_type = recursion_context_return_type<Reader, Rules, Fn>;

    template<auto Fn, class T>
    using tagged_type = recursion_context_tagged_type<Reader, Rules, Fn, T>;

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    using cache_type = Rules::cache_type;

    template<auto Fn>
    std::unordered_map<
        const void*, std::vector<std::pair<return_type<Fn>, Reader>>
    >& cache_for() const
    {
        return static_cast<tagged_type<Fn, std::unordered_map<
            const void*, std::vector<std::pair<return_type<Fn>, Reader>>
        >>&>(*cache);
    }
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)

#if defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    basic_recursion_context(const Reader& reader) : reader{reader} {}
#else // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    basic_recursion_context(cache_type& cache, const Reader& reader)
        : cache{&cache}
        , reader{reader}
    {}
#endif // defined(FEKAL_DISABLE_PEG_MEMOIZATION)

    template<auto Fn>
    return_type<Fn> enter(Reader& reader) const
    {
#if defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        basic_recursion_context inner{reader};
#else // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        basic_recursion_context inner{*cache, reader};
#endif // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        if (this->reader == reader) { // left-recursion
            // inherit and enforce current limits
            inner = *this;

            if (auto& lim = inner.limit<Fn>() ; lim) {
                if (*lim == 0) {
                    return nullptr;
                }

                --*lim;

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
                if (
                    auto& c = cache_for<Fn>()[reader.literal().data()] ;
                    *lim < c.size()
                ) {
                    reader = c[*lim].second;
                    return c[*lim].first;
                }
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)

                return Fn(inner, reader);
            }
        } else {
            assert(this->reader < reader);
        }

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        auto& c = cache_for<Fn>()[reader.literal().data()];
        if (c.size() > 0) {
            reader = c.back().second;
            return c.back().first;
        }
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)

        auto& limit = inner.limit<Fn>().emplace(0);
        auto backup = reader;
        auto last_res = Fn(inner, reader);
#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        c.emplace_back(last_res, reader);
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        if (!last_res) {
            return last_res;
        }

        for (limit = 1 ;; ++limit) {
            auto r2 = backup;
            auto res = Fn(inner, r2);
            assert(res);
            if (reader < r2) {
                // we found more tokens in the new deeper iteration
                using std::swap;
                swap(last_res, res);
                reader = r2;
#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
                c.emplace_back(last_res, reader);
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
            } else {
                break;
            }
        }

        return last_res;
    }

    // useful to define left-to-right associativity
    template<auto Fn>
    return_type<Fn> right1(Reader& reader) const
    {
        assert(this->reader < reader);
#if defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        basic_recursion_context inner{reader};
#else // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        basic_recursion_context inner{*cache, reader};
#endif // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        inner.limit<Fn>().emplace(0);
        return Fn(inner, reader);
    }

    template<auto Fn>
    std::optional<unsigned>& limit()
    {
        return static_cast<tagged_type<Fn, std::optional<unsigned>>&>(limits);
    }

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    cache_type* cache;
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    Reader reader;
    Rules limits;
};

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
template<class Reader, class Rules, auto Tag, auto... Tail>
struct recursion_context_cache_type;

template<class Reader, class Rules, auto Tag>
struct recursion_context_cache_type<Reader, Rules, Tag>
    : recursion_context_tagged_type<
        Reader, Rules, Tag,
        std::unordered_map<
            const void*,
            std::vector<
                std::pair<
                    recursion_context_return_type<Reader, Rules, Tag>,
                    Reader>>>>
{};

template<class Reader, class Rules, auto Tag1, auto Tag2, auto... Tail>
struct recursion_context_cache_type<Reader, Rules, Tag1, Tag2, Tail...>
    : recursion_context_tagged_type<
        Reader, Rules, Tag1,
        std::unordered_map<
            const void*,
            std::vector<
                std::pair<
                    recursion_context_return_type<Reader, Rules, Tag1>,
                    Reader>>>>
    , recursion_context_cache_type<Reader, Rules, Tag2, Tail...>
{};
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)

template<class Reader, class Rules, auto Tag, auto... Tail>
struct basic_recursion_context_rules;

template<class Reader, class Rules, auto Tag>
struct basic_recursion_context_rules<Reader, Rules, Tag>
    : recursion_context_tagged_optional<Reader, Rules, Tag>
{
#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    using cache_type = recursion_context_cache_type<Reader, Rules, Tag>;
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
};

template<class Reader, class Rules, auto Tag1, auto Tag2, auto... Tail>
struct basic_recursion_context_rules<Reader, Rules, Tag1, Tag2, Tail...>
    : recursion_context_tagged_optional<Reader, Rules, Tag1>
    , basic_recursion_context_rules<Reader, Rules, Tag2, Tail...>
{
#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    using cache_type = recursion_context_cache_type<
        Reader, Rules, Tag1, Tag2, Tail...>;
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
};

} // namespace fekal::peg
