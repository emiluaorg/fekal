// Copyright (c) 2025 Vinícius dos Santos Oliveira
// SPDX-License-Identifier: MIT-0

#pragma once

#include <cstdint>
#include <variant>

namespace fekal::ast {

struct ActionAllow {
    bool operator==(const ActionAllow&) const = default;
};

struct ActionLog {
    bool operator==(const ActionLog&) const = default;
};

struct ActionKillProcess {
    bool operator==(const ActionKillProcess&) const = default;
};

struct ActionKillThread {
    bool operator==(const ActionKillThread&) const = default;
};

struct ActionUserNotif {
    bool operator==(const ActionUserNotif&) const = default;
};

struct ActionErrno
{
    int errnum;

    bool operator==(const ActionErrno&) const = default;
};

struct ActionTrap
{
    std::int64_t code;

    bool operator==(const ActionTrap&) const = default;
};

struct ActionTrace
{
    std::int64_t code;

    bool operator==(const ActionTrace&) const = default;
};

using Action = std::variant<
    ActionAllow, ActionLog, ActionKillProcess, ActionKillThread,
    ActionUserNotif, ActionErrno, ActionTrap, ActionTrace>;

} // namespace fekal::ast
