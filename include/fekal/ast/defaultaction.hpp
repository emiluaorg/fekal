// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/action.hpp>

namespace fekal::ast {

struct DefaultAction : NodeBase, Action
{
    using Action::Action;
};

} // namespace fekal::ast
