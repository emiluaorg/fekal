// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#pragma once

#include <fekal/ast/nodebase.hpp>
#include <iostream>
#include <ostream>
#include <ranges>
#include <unistd.h>

namespace fekal {

struct Position
{
    unsigned line = 0;
    unsigned column = 0;
};

struct Range
{
    Position start;
    Position end;
};

enum Severity
{
    Error = 1,
    Warning = 2,
    Information = 3,
    Hint = 4,
};

struct Log
{
    Severity severity;
    std::string message;
    Range range;
    unsigned line;
    unsigned column;
};

struct Diagnostics
{
    std::vector<Log> logs;

    Diagnostics()
        : maxErrors{100}
        , maxWarnings{100}
    {}

    Diagnostics(bool stdout_has_colors)
        : stdout_has_colors{stdout_has_colors}
        , maxErrors{100}
        , maxWarnings{100}
    {}

    Diagnostics(unsigned maxErrors, unsigned maxWarnings)
        : maxErrors{maxErrors}
        , maxWarnings{maxWarnings}
    {}

    void reset()
    {
        logs.clear();
    }

    void error(const std::string& msg, Range range)
    {
        auto log = Log{Severity::Error, msg, range};
        logs.push_back(std::move(log));
    }

    void warning(const std::string& msg, Range range)
    {
        auto log = Log{Severity::Warning, msg, range};
        logs.push_back(std::move(log));
    }

    void print()
    {
        std::ranges::for_each(
            logs
            | std::views::filter([](const auto& log) { return log.severity == Severity::Warning; })
            | std::views::take(maxWarnings),
            [this](const auto& log) {
                auto severity = this->stdout_has_colors ? "\033[33mWarning:\033[0m " : "Warning ";
                std::cerr << severity << log.message << std::endl;
            }
        );
        std::ranges::for_each(
            logs
            | std::views::filter([](const auto& log) { return log.severity == Severity::Error; })
            | std::views::take(maxErrors),
            [this](const auto& log) {
                auto severity = this->stdout_has_colors ? "\033[31mError:\033[0m " : "Warning ";
                std::cerr << severity << log.message << std::endl;
            }
        );
    }

    template<class Node>
    Range rangeFromName(const Node& node, std::string name) const
    {
        return Range{
            .start{node.line, node.column},
            .end{node.line, static_cast<unsigned int>(node.column + name.size())}
        };
    }

private:
    bool stdout_has_colors = false;
    unsigned maxErrors;
    unsigned maxWarnings;
};

} // namespace fekal
