// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

namespace fekal::ast {

struct NodeBase
{
    unsigned line = 0;
    unsigned column = 0;
};

} // namespace fekal::ast
