; Copyright (c) 2025 Vinícius dos Santos Oliveira
; SPDX-License-Identifier: MIT-0

(comment) @comment
(integer) @constant.builtin
(syscall_filter syscall: (identifier) @function body: (_))
(policy name: (identifier) @namespace)

[
 "("
 ")"
 "{"
 "}"
] @punctuation.bracket

[
 ","
] @punctuation.delimiter

[
 "+"
 "-"
 "*"
 "/"
 "=="
 "!="
 "<"
 ">"
 "<="
 ">="
 "!"
 "&&"
 "||"
 "|"
 "&"
 "^"
 ">>"
 "<<"
] @operator

[
 "DEFAULT"
 "POLICY"
 (action_allow)
 (action_log)
 (action_kill_process)
 (action_kill_thread)
 (action_user_notif)
 "ERRNO"
 "TRAP"
 "TRACE"
] @keyword
