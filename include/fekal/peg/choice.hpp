// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

namespace fekal::peg {

template<class Recur, class Reader, class F>
static inline
auto choice(const Recur& recur, Reader& r, F&& f)
{
    auto backup = r;
    auto res = f(recur, r);
    if (!recur.test(res)) {
        r = backup;
    }
    return res;
}

template<class Recur, class Reader, class F1, class F2, class... F3>
static inline
auto choice(const Recur& recur, Reader& reader, F1&& f1, F2&& f2, F3&&... f3)
{
    auto backup = reader;
    if (auto res = f1(recur, reader) ; recur.test(res)) {
        return res;
    } else {
        reader = backup;
        return choice(
            recur, reader, std::forward<F2>(f2), std::forward<F3>(f3)...);
    }
}

} // namespace fekal::peg
