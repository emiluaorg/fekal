// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

namespace fekal::ast {

inline bool LshiftExpr::operator==(const LshiftExpr& o) const
{
    return std::tie(base(), *left, *right) ==
        std::tie(o.base(), *o.left, *o.right);
}

} // namespace fekal::ast
