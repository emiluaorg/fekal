// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#include <fekal/ast.hpp>
#include <fekal/printer.hpp>
#include <boost/hana/functional/overload.hpp>
#include <boost/algorithm/string/join.hpp>
#include <ostream>
#include <format>

namespace fekal {

namespace hana = boost::hana;

struct Printer : fekal::Traverser<Printer>
{
    Printer(std::ostream& stream)
        : indentChar("  ")
        , stream(stream) {}

    void print(const std::vector<ast::ProgramStatement>& ast)
    {
        writeln("Program {\n");
        traverse(ast);
        writeln("}\n");
    }

private:
    unsigned level = 1;
    const char* indentChar;
    std::ostream& stream;
    friend struct fekal::Traverser<Printer>;

    void visit(const ast::IntExpr& expr)
    {
        std::visit(hana::overload(
            [&](const ast::IntLit& e) {
                return writeln(std::format("{}\n", e.value));
            },
            [&](const ast::Identifier& e) {
                return writeln(std::format("{}\n", e.value));
            },
            [&](const ast::SumExpr&) {
                return writeln("(+\n");
            },
            [&](const ast::SubtractExpr&) {
                return writeln("(-\n");
            },
            [&](const ast::MulExpr&) {
                return writeln("(*\n");
            },
            [&](const ast::DivExpr&) {
                return writeln("(/\n");
            },
            [&](const ast::LshiftExpr&) {
                return writeln("(<<\n");
            },
            [&](const ast::RshiftExpr&) {
                return writeln("(>>\n");
            },
            [&](const ast::BitAndExpr&) {
                return writeln("(&\n");
            },
            [&](const ast::BitXorExpr&) {
                return writeln("(^\n");
            },
            [&](const ast::BitOrExpr&) {
                return writeln("(|\n");
            }
        ), expr);
    }

    void visit(const ast::BoolExpr& expr)
    {
        level++;
        std::visit(hana::overload(
            [&](const ast::EqExpr&) {
                return writeln("(==\n");
            },
            [&](const ast::NeqExpr&) {
                return writeln("(!=\n");
            },
            [&](const ast::LtExpr&) {
                return writeln("(<\n");
            },
            [&](const ast::GtExpr&) {
                return writeln("(>\n");
            },
            [&](const ast::LteExpr&) {
                return writeln("(<=\n");
            },
            [&](const ast::GteExpr&) {
                return writeln("(>=\n");
            },
            [&](const ast::NegExpr&) {
                return writeln("!(\n");
            },
            [&](const ast::AndExpr&) {
                return writeln("AndExpr{\n");
            },
            [&](const ast::OrExpr&) {
                return writeln("OrExpr{\n");
            }
        ), expr);
        level++;
    }

    void visit_leave(const ast::BoolExpr& expr)
    {
        level--;
        std::visit(hana::overload(
            [&](const ast::NegExpr& e) {
                return writeln("),\n");
            },
            [&](const ast::AndExpr& e) {
                return writeln("},\n");
            },
            [&](const ast::OrExpr& e) {
                return writeln("},\n");
            },
            [&](const auto& e) {
                return writeln("),\n");
            }
        ), expr);
        level--;
    }

    void visit(const ast::SyscallFilter& filter)
    {
        level++;
        writeln(std::format("{}", filter.syscall));
        if (filter.body.size() > 0) {
            std::vector<std::string> names;
            names.reserve(filter.params.size());
            for (const auto& id : filter.params) {
                names.push_back(id.value);
            }
            std::string params = boost::algorithm::join(names, ", ");
            write(std::format("({}) {{\n", params));
        } else {
            write(",\n");
            level--;
        }
    }

    void visit_leave(const ast::SyscallFilter& filter)
    {
        if (filter.body.size() > 0) {
            writeln("},\n");
            level--;
        }
    }

    bool visit(const ast::ActionBlock& action)
    {
        level++;
        std::string label = std::visit<std::string>(hana::overload(
            [](const ast::ActionAllow&) { return "ALLOW";},
            [](const ast::ActionLog&) { return "LOG";},
            [](const ast::ActionKillProcess&) { return "KILL_PROCESS";},
            [](const ast::ActionKillThread&) { return "KILL_THREAD";},
            [](const ast::ActionUserNotif&) { return "USER_NOTIF";},
            [](const ast::ActionErrno& a) {
                return std::format("ERRNO{{{}}}", a.errnum);},
            [](const ast::ActionTrap& a) {
                return std::format("TRAP{{{}}}", a.code);},
            [](const ast::ActionTrace& a) {
                return std::format("TRACE{{{}}}", a.code); }
        ), action.action);
        writeln(std::format("{} {{\n", label));
        return true;
    }

    void visit_leave(const ast::ActionBlock& action)
    {
        writeln("},\n");
        level--;
    }

    void visit(const ast::UseStatement& stmt)
    {
        level++;
        writeln(std::format("UseStatement{{{}}}\n", stmt.id()));
        level--;
    }

    void visit(const ast::Policy& policy)
    {
        level++;
        writeln(std::format("Policy {} {{\n", policy.id()));
    }

    void visit_leave(const ast::Policy& policy)
    {
        writeln("},\n");
        level--;
    }

    void writeln(std::string text)
    {
        indent();
        write(text);
    }

    void write(std::string text)
    {
        stream << text;
    }

    void indent()
    {
        for (int i = 0; i < level-1; i++) {
            stream << indentChar;
        }
    }
};

void print(std::ostream& stream, const std::vector<ast::ProgramStatement>& ast)
{
    Printer{stream}.print(ast);
}

} // namespace fekal
