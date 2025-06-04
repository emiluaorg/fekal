/**
 * @file Seccomp policy language
 * @author Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>
 * @license MIT-0
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: "fekal",

  word: $ => $.identifier,

  rules: {
    source_file: $ => repeat($._program_statement),
    _program_statement: $ => choice(
      $.policy, $._policy_statement, $.default_statement),
    policy: $ => seq(
      'POLICY',
      field('name', $.identifier),
      field('version', choice($.integer, $.identifier)),
      '{',
      field('body', repeat($._policy_statement)),
      '}'),
    _policy_statement: $ => choice($.use_statement, $.action_block),
    use_statement: $ => seq(
      'USE',
      field('name', $.identifier),
      field('version', choice($.integer, $.identifier))),
    default_statement: $ => seq('DEFAULT', $._action),
    action_block: $ => seq(
      field('action', $._action),
      '{',
      repeat(seq(field('filter', $.syscall_filter), ',')),
      optional(field('filter', $.syscall_filter)),
      '}'),
    _action: $ => choice(
      $.action_allow,
      $.action_log,
      $.action_kill_process,
      $.action_kill_thread,
      $.action_user_notif,
      $.action_errno,
      $.action_trap,
      $.action_trace,
    ),
    action_allow: $ => 'ALLOW',
    action_log: $ => 'LOG',
    action_kill_process: $ => 'KILL_PROCESS',
    action_kill_thread: $ => 'KILL_THREAD',
    action_user_notif: $ => 'USER_NOTIF',
    action_errno: $ => seq('ERRNO', '(', choice($.identifier, $.integer), ')'),
    action_trap: $ => seq('TRAP', '(', $.integer, ')'),
    action_trace: $ => seq('TRACE', '(', $.integer, ')'),
    syscall_filter: $ => seq(
      field('syscall', $.identifier),
      optional(seq(
        field('parameter_list', $.parameter_list),
        '{',
        field(
          'body',
          seq(
            repeat(seq($.bool_expr, ',')),
            optional($.bool_expr))),
        '}'))),
    parameter_list: $ => seq(
      '(',
      optional(seq(
        repeat(seq(field('parameter', $.identifier), ',')),
        field('parameter', $.identifier))),
      ')'),
    bool_expr: $ => choice(
      prec.left(1, seq($.bool_expr, '||', $.bool_expr)),
      prec.left(2, seq($.bool_expr, '&&', $.bool_expr)),
      seq(
        $.int_expr,
        choice('==', '!=', '<', '>', '<=', '>='),
        $.int_expr),
      seq(optional('!'), '(', $.bool_expr, ')'),
    ),
    int_expr: $ => choice(
      prec.left(1, seq($.int_expr, '|', $.int_expr)),
      prec.left(2, seq($.int_expr, '^', $.int_expr)),
      prec.left(3, seq($.int_expr, '&', $.int_expr)),
      prec.left(4, seq($.int_expr, choice('<<', '>>'), $.int_expr)),
      prec.left(5, seq($.int_expr, choice('+', '-'), $.int_expr)),
      prec.left(6, seq($.int_expr, choice('*', '/'), $.int_expr)),
      $.integer,
      $.identifier,
      seq('(', $.int_expr, ')'),
    ),
    identifier: $ => /[a-zA-Z_][a-zA-Z_0-9]*/,
    integer: $ => choice(
      /-?0[bB][01]+/,
      /-?0[oO][0-7]+/,
      '0',
      /[1-9][0-9]*/,
      /-?0[xX][0-9a-fA-F]+/,
    ),
    comment: _ => token(choice(
      seq('//', /[^\n]*/),
      seq(
        '/*',
        /[^*]*\*+([^/*][^*]*\*+)*/,
        '/',
      ),
    )),
  },

  extras: $ => [
    $.comment,
    /\s/,
  ],
});
