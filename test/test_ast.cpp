// Copyright (c) 2025 César Augusto D. Azevedo
// SPDX-License-Identifier: MIT-0

#define BOOST_TEST_MODULE AST

#include <fekal/parser.hpp>
#include <fekal/printer.hpp>
#include <boost/test/included/unit_test.hpp>

using namespace fekal;

BOOST_AUTO_TEST_CASE(assert_ast_structure)
{
    std::string input = R"(
        POLICY Aio 0 {
            ALLOW {
                io_cancel, io_destroy, io_getevents, io_pgetevents, io_setup, io_submit,
            }
        }
        POLICY BasicIo 0 {
            USE Aio 0
        }
        POLICY Clock 0 {}
        POLICY CompatX86 0 {
            ALLOW {
                personality(persona) {
                    persona == 0 ||
                    persona == 8 ||
                    persona == 16 ||
                    persona == 20 ||
                    persona == 24
                }
            }
        }
    )";
    std::vector<ast::ProgramStatement> parsed = fekal::parse(input);
    auto p = Printer(std::cout);
    p.print(parsed);

    std::vector<ast::ProgramStatement> expected = {
        ast::Policy(2, 19, "Aio", "0", {
            ast::ActionBlock{
                ast::Action{fekal::ast::ActionAllow{}},
                {
                    ast::SyscallFilter(4, 25, "io_cancel"),
                    ast::SyscallFilter(4, 37, "io_destroy"),
                    ast::SyscallFilter(4, 51, "io_getevents"),
                    ast::SyscallFilter(4, 66, "io_pgetevents"),
                    ast::SyscallFilter(4, 76, "io_setup"),
                    ast::SyscallFilter(4, 87, "io_submit"),
                }
            }
        }),
        ast::Policy(7, 23, "BasicIo", "0", {
            ast::UseStatement{8, 20, "Aio", "0"}
        }),
        ast::Policy(10, 21, "Clock", "0", {}),
        ast::Policy(11, 25, "CompatX86", "0", {
            ast::ActionBlock{
                ast::Action{fekal::ast::ActionAllow{}},
                {
                    ast::SyscallFilter(13, 27, "personality", {ast::Identifier(13, 28, "persona")}, {
                        ast::make_bool_expr<ast::OrExpr>(
                            17,
                            34,
                            ast::make_bool_expr<ast::OrExpr>(
                                16,
                                34,
                                ast::make_bool_expr<ast::OrExpr>(
                                    15,
                                    33,
                                    ast::make_bool_expr<ast::OrExpr>(
                                        14,
                                        33,
                                        ast::make_bool_expr<ast::EqExpr>(
                                            14,
                                            28,
                                            ast::make_int_expr<ast::Identifier>(14, 20, "persona"),
                                            ast::make_int_expr<ast::IntLit>(14, 31, 0)
                                        ),
                                        ast::make_bool_expr<ast::EqExpr>(
                                            15,
                                            28,
                                            ast::make_int_expr<ast::Identifier>(15, 20, "persona"),
                                            ast::make_int_expr<ast::IntLit>(15, 31, 8)
                                        )
                                    ),
                                    ast::make_bool_expr<ast::EqExpr>(
                                        16,
                                        28,
                                        ast::make_int_expr<ast::Identifier>(16, 20, "persona"),
                                        ast::make_int_expr<ast::IntLit>(16, 31, 16)
                                    )
                                ),
                                ast::make_bool_expr<ast::EqExpr>(
                                    17,
                                    28,
                                    ast::make_int_expr<ast::Identifier>(17, 20, "persona"),
                                    ast::make_int_expr<ast::IntLit>(17, 31, 20)
                                )
                            ),
                            ast::make_bool_expr<ast::EqExpr>(
                                18,
                                28,
                                ast::make_int_expr<ast::Identifier>(18, 20, "persona"),
                                ast::make_int_expr<ast::IntLit>(18, 31, 24)
                            )
                        )
                    }),
                }
            }
        }),
    };
    BOOST_TEST(parsed == expected);
}
