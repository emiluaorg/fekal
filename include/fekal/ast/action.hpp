// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <cstdint>
#include <variant>

namespace fekal::ast {

struct ActionAllow {};
struct ActionLog {};
struct ActionKillProcess {};
struct ActionKillThread {};
struct ActionUserNotif {};

struct ActionErrno
{
    int errnum;
};

struct ActionTrap
{
    std::int64_t code;
};

struct ActionTrace
{
    std::int64_t code;
};

using Action = std::variant<
    ActionAllow, ActionLog, ActionKillProcess, ActionKillThread,
    ActionUserNotif, ActionErrno, ActionTrap, ActionTrace>;

} // namespace fekal::ast
