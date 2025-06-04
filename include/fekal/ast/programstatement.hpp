// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/policystatement.hpp>
#include <fekal/ast/defaultaction.hpp>
#include <fekal/ast/policy.hpp>

namespace fekal::ast {

using ProgramStatement = boost::mp11::mp_append<
    std::variant<Policy, DefaultAction>,
    PolicyStatement>;

} // namespace fekal::ast
