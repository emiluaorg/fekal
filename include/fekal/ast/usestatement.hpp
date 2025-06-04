// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <string>

namespace fekal::ast {

struct UseStatement
{
    std::string policy;
    std::string version;
};

} // namespace fekal::ast
