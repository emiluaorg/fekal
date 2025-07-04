// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <type_traits>
#include <cassert>
#include <variant>
#include <bitset>
#include <array>

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/list.hpp>

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

template<class Reader, class Rules>
struct basic_recursion_context
{
    template<auto Fn>
    using return_type = recursion_context_return_type<Reader, Rules, Fn>;

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    using cache_type = Rules::cache_type;

    template<auto Fn>
    std::unordered_map<
        const void*, std::vector<std::pair<return_type<Fn>, Reader>>
    >& cache_for() const
    {
        return std::get<index<Fn>()>(cache->container);
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

    template<class... Ts>
    static bool test(const std::variant<std::monostate, Ts...>& v)
    {
        return v.index() != 0;
    }

    template<class T>
    static bool test(const T& v)
    {
        return static_cast<bool>(v);
    }

    template<auto Fn>
    return_type<Fn> enter_impl(Reader& reader, boost::mp11::mp_true) const
    {
        return Fn(*this, reader);
    }

    template<auto Fn>
    return_type<Fn> enter_impl(Reader& reader, boost::mp11::mp_false) const
    {
#if defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        basic_recursion_context inner{reader};
#else // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        basic_recursion_context inner{*cache, reader};
#endif // defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        if (this->reader == reader) { // left-recursion
            // inherit and enforce current limits
            inner = *this;

            if (inner.is_limited<Fn>()) {
                auto& lim = inner.limit<Fn>();
                if (lim == 0) {
                    return {};
                }

                --lim;

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
                if (
                    auto& c = cache_for<Fn>()[reader.literal().data()] ;
                    lim < c.size()
                ) {
                    reader = c[lim].second;
                    return c[lim].first;
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

        auto& limit = inner.emplace_limit<Fn>(0);
        auto backup = reader;
        auto last_res = Fn(inner, reader);
#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        c.emplace_back(last_res, reader);
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
        if (!test(last_res)) {
            return last_res;
        }

        for (limit = 1 ;; ++limit) {
            auto r2 = backup;
            auto res = Fn(inner, r2);
            assert(test(res));
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

    template<auto Fn>
    return_type<Fn> enter(Reader& reader) const
    {
        using Tag = boost::mp11::mp_bool<index<Fn>() == Rules::mp_fn_list_size>;
        return enter_impl<Fn>(reader, Tag{});
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
        inner.emplace_limit<Fn>(0);
        return Fn(inner, reader);
    }

    template<auto Fn>
    static constexpr std::size_t index()
    {
        using L = Rules::mp_fn_list;
        return boost::mp11::mp_find<L, boost::mp11::mp_value<Fn>>{};
    }

    template<auto Fn>
    bool is_limited() const
    {
        return rules.used_limits.test(index<Fn>());
    }

    template<auto Fn>
    unsigned& limit()
    {
        return rules.limits[index<Fn>()];
    }

    template<auto Fn>
    unsigned& emplace_limit(unsigned value)
    {
        static constexpr auto Idx = index<Fn>();
        rules.used_limits.set(Idx);
        rules.limits[Idx] = value;
        return rules.limits[Idx];
    }

#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    cache_type* cache;
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    Reader reader;
    Rules rules;
};

template<class Reader, class Rules, auto... Fns>
struct basic_recursion_context_rules
{
#if !defined(FEKAL_DISABLE_PEG_MEMOIZATION)
    struct cache_type
    {
        template<class T>
        using container_type = std::unordered_map<
            const void*, std::vector<std::pair<T, Reader>>>;

        std::tuple<container_type<
            recursion_context_return_type<Reader, Rules, Fns>>...> container;
    };
#endif // !defined(FEKAL_DISABLE_PEG_MEMOIZATION)

    using mp_fn_list = boost::mp11::mp_list<boost::mp11::mp_value<Fns>...>;
    static constexpr auto mp_fn_list_size = sizeof...(Fns);

    std::array<unsigned, sizeof...(Fns)> limits;
    std::bitset<sizeof...(Fns)> used_limits;
};

} // namespace fekal::peg
