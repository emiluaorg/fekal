/**
 * @file Seccomp policy language
 * @author Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com>
 * @license MIT-0
 */

/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
  name: "fekal",

  rules: {
    // TODO: add the actual grammar rules
    source_file: $ => "hello"
  }
});
