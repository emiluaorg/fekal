// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <optional>
#include <ranges>

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
    using fn_type = std::optional<T>(*)(const basic_recursion_context&, R&);

    basic_recursion_context(const R& reader) : reader{reader} {}

    template<fn_type Fn>
    std::optional<T> enter(R& reader) const
    {
        basic_recursion_context inner{reader};
        if (this->reader == reader) { // left-recursion
            auto it = limits.find(Fn);
            if (it != limits.end() && it->second == 0) {
                return std::nullopt;
            }

            // inherit and enforce current limits
            inner = *this;

            if (it != limits.end()) {
                inner.limits.find(Fn)->second -= 1;
                return Fn(inner, reader);
            }
        } else {
            assert(this->reader < reader);
        }

        auto& limit = inner.limits.emplace(Fn, 0).first->second;
        auto backup = reader;
        auto last_res = Fn(inner, reader);
        if (!last_res) {
            return last_res;
        }

        auto last_reader = reader;

        for (unsigned lim : std::views::iota(1)) {
            limit = lim;
            reader = backup;
            auto res = Fn(inner, reader);
            assert(res);
            if (last_reader < reader) {
                // we found more tokens in the new deeper iteration
                using std::swap;
                swap(last_res, res);
                last_reader = reader;
            } else {
                break;
            }
        }

        reader = last_reader;
        return last_res;
    }

    R reader;
    boost::container::flat_map<fn_type, unsigned> limits;
};

} // namespace fekal
