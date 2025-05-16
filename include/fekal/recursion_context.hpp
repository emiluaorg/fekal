// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <optional>
#include <utility>
#include <ranges>

#include <boost/coroutine2/coroutine.hpp>
#include <boost/container/flat_map.hpp>

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
template<class T, class R>
struct basic_recursion_context
{
    using coro_type = boost::coroutines2::coroutine<std::pair<T, R>>;
    using coro_pull_type = coro_type::pull_type;
    using coro_push_type = coro_type::push_type;

    using fn_type = void(*)(const basic_recursion_context&, R, coro_push_type&);

    template<fn_type Fn>
    struct wrap_result;

    basic_recursion_context(const R& reader)
        : reader{reader}
    {}

    template<fn_type Fn>
    wrap_result<Fn> wrap(const R& reader) const
    {
        basic_recursion_context ret{reader};
        if (this->reader == reader) { // left-recursion
            // inherit and apply current limits
            ret = *this;
        } else {
            assert(this->reader < reader);
        }
        ret.levels.emplace(Fn, 0).first->second += 1;
        return {std::move(ret)};
    }

    template<fn_type Fn>
    unsigned level() const
    {
        if (auto it = levels.find(Fn) ; it != levels.end()) {
            return it->second;
        } else {
            return 0;
        }
    }

    template<fn_type Fn>
    std::optional<unsigned> limit() const
    {
        if (auto it = limits.find(Fn) ; it != limits.end()) {
            return it->second;
        } else {
            return std::nullopt;
        }
    }

    template<fn_type Fn>
    bool limit_reached(unsigned lim) const
    {
        return level<Fn>() > lim;
    }

    R reader;
    boost::container::flat_map<fn_type, unsigned> levels;
    boost::container::flat_map<fn_type, unsigned> limits;
};

template<class T, class R>
template<basic_recursion_context<T, R>::fn_type Fn>
struct basic_recursion_context<T, R>::wrap_result
{
    wrap_result(basic_recursion_context<T, R> context)
        : context{std::move(context)}
    {}

    bool limit_reached(unsigned lim) const
    {
        return context.limit_reached<Fn>(lim);
    }

    void operator()(coro_push_type& yield) const
    {
        if (auto lim = limit() ; lim) {
            if (limit_reached(*lim)) {
                return;
            }

            Fn(context, context.reader, yield);
        } else {
            coro_pull_type last_coro{limit(level())};
            if (!last_coro) {
                return;
            }

            auto last_res = last_coro.get();

            for (unsigned lim : std::views::iota(level() + 1)) {
                coro_pull_type coro{limit(lim)};
                assert(coro);
                auto res = coro.get();
                if (last_res.second < res.second) {
                    // we found more tokens in the new deeper iteration
                    using std::swap;
                    swap(last_coro, coro);
                    swap(last_res, res);
                } else {
                    break;
                }
            }

            yield(std::move(last_res));
            last_coro();

            for (auto& v : last_coro) {
                yield(std::move(v));
            }
        }
    }

    wrap_result limit(unsigned n) const
    {
        auto ctx2{context};
        if (auto old = ctx2.limits.find(Fn) ; old != ctx2.limits.end()) {
            if (old->second > n) {
                old->second = n;
            }
        } else {
            ctx2.limits.emplace(Fn, n);
        }
        return {std::move(ctx2)};
    }

    unsigned level() const
    {
        return context.level<Fn>();
    }

    std::optional<unsigned> limit() const
    {
        return context.limit<Fn>();
    }

    basic_recursion_context<T, R> context;
};

} // namespace fekal
