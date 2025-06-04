// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

namespace fekal::ast {

inline bool NegExpr::operator==(const NegExpr& o) const
{
    return std::tie(base(), *inner) == std::tie(o.base(), *o.inner);
}

} // namespace fekal::ast
