#include "lisp_evaluator.h"
#include "../kernel/lisp/input.h"
#include "../kernel/lisp/lisp.h"
#include "../kernel/lisp/pair.h"
#include "../libc/string.h"
#include "../drivers/screen.h"
#include <stddef.h>

static size_t exps_len = 84;
static char *exps[] = {
    "(define (or arg1 arg2) (if arg1 'true (if arg2 'true 'false)))",
    "(define (and arg1 arg2) (if arg1 (if arg2 'true 'false) 'false))",
    "(define (not arg) (if arg 'false 'true))",
    "(define (caar p) (car (car p)))",
    "(define (cadr p) (car (cdr p)))",
    "(define (cddr p) (cdr (cdr p)))",
    "(define (caddr p) (car (cddr p)))",
    "(define (caadr p) (car (cadr p)))",
    "(define (cdddr p) (cdr (cddr p)))",
    "(define (cadddr p) (car (cdddr p)))",
    "(define (caaadr p) (car (caadr p)))",
    "(define (tagged-list-car? exp tag) (if (pair? exp) (eq? (car exp) tag) 'false))",
    "(define (tagged-list-caar? exp tag) (if (pair? exp) (eq? (caar exp) tag) 'false))",
    "(define (length p) (if (null? p) 0 (+ 1 (length (cdr p)))))",
    "(define (map f p) (if (null? p) '() (cons (f (car p)) (map f (cdr p)))))",
    "(define (self-evaluating? exp) (or (number? (car exp)) (string? (car exp))))",
    "(define (variable? exp) (symbol? (car exp)))",
    "(define (text-of-quotation exp) (car exp))",
    "(define (assignment? exp) (tagged-list-caar? exp 'set!))",
    "(define (assignment-variable exp) (caadr exp))",
    "(define (assignment-value exp) (caddr exp))",
    "(define (definition? exp) (tagged-list-caar? exp 'define))",
    "(define (definition-variable exp) (if (symbol? (caadr exp)) (caadr exp) (caaadr exp)))",
    "(define (definition-value exp) (if (symbol? (caadr exp)) (caadr exp) (make-lambda (cdadr exp) (cddr exp))))",
    "(define (lambda? exp) (tagged-list-caar? exp 'lambda))",
    "(define (lambda-parameters-helper parameters) (cons (caar parameters) (lambda-parameters-helper (cdr parameters))))",
    "(define (lambda-parameters exp) (lambda-parameters-helper (cadr exp)))",
    "(define (lambda-body exp) (cddr exp))",
    "(define (make-lambda parameters body) (cons (cons 'lambda '()) (cons parameters body)))",
    "(define (if? exp) (tagged-list-caar? exp 'if))",
    "(define (if-predicate exp) (cadr exp))",
    "(define (if-consequent exp) (caddr exp))",
    "(define (if-alternative exp) (if (not (null? (cdddr exp))) (cadddr exp) 'false))",
    "(define (make-if predicate consequent alternative) (list (cons 'if '()) predicate consequent alternative))",
    "(define (begin? exp) (tagged-list-caar? exp 'begin))",
    "(define (begin-actions exp) (cdr exp))",
    "(define (last-exp? seq) (null? (cdr seq)))",
    "(define (first-exp seq) (car seq))",
    "(define (rest-exps seq) (cdr seq))",
    "(define (sequence->exp seq) (cond ((null? seq) seq) ((last-exp? seq) (first-exp seq)) (else (make-begin seq))))",
    "(define (make-begin seq) (cons (cons 'begin '()) seq))",
    "(define (application? exp) (pair? exp))",
    "(define (operator exp) (car exp))",
    "(define (operands exp) (cdr exp))",
    "(define (no-operands? ops) (null? ops))",
    "(define (first-operand ops) (car ops))",
    "(define (rest-operands ops) (cdr ops))",
    "(define (eval exp env) (cond ((self-evaluating? exp) (car exp)) ((variable? exp) (lookup-variable-value (car exp) env)) ((quoted? (car exp)) (text-of-quotation exp)) ((assignment? exp) (eval-assignment exp env)) ((definition? exp) (eval-definition exp env)) ((if? exp) (eval-if exp env)) ((lambda? exp) (make-procedure (lambda-parameters exp) (lambda-body exp) env)) ((begin? exp) (eval-sequence (begin-actions exp) env)) ((application? exp) (apply (eval (operator exp) env) (list-of-values (operands exp) env))) (else (display \"Unknown expression type -- EVAL\"))))",
    "(define apply-in-underlying-scheme apply)",
    "(define (apply procedure arguments) (cond ((primitive-procedure? procedure) (apply-primitive-procedure procedure arguments)) ((compound-procedure? procedure) (eval-sequence (procedure-body procedure) (extend-environment (procedure-parameters procedure) arguments (procedure-environment procedure)))) (else (display \"Unknown procedure type -- APPLY\"))))",
    "(define (list-of-values exps env) (if (no-operands? exps) '() (cons (eval (first-operand exps) env) (list-of-values (rest-operands exps) env))))",
    "(define (eval-if exp env) (if (eval (if-predicate exp) env) (eval (if-consequent exp) env) (eval (if-alternative exp) env)))",
    "(define (eval-sequence exps env) (cond ((last-exp? exps) (eval (first-exp exps) env)) (else (eval (first-exp exps) env) (eval-sequence (rest-exps exps) env))))",
    "(define (eval-assignment exp env) (set-variable-value! (assignment-variable exp) (eval (assignment-value exp) env) env) 'ok)",
    "(define (eval-definition exp env) (define-variable! (definition-variable exp) (eval (definition-value exp) env) env) 'ok)",
    "(define (make-procedure parameters body env) (list 'procedure parameters body env))",
    "(define (compound-procedure? p) (tagged-list-car? p 'procedure))",
    "(define (procedure-parameters p) (cadr p))",
    "(define (procedure-body p) (caddr p))",
    "(define (procedure-environment p) (cadddr p))",
    "(define (enclosing-environment env) (cdr env))",
    "(define (first-frame env) (car env))",
    "(define the-empty-environment '())",
    "(define (make-frame variables values) (cons variables values))",
    "(define (frame-variables frame) (car frame))",
    "(define (frame-values frame) (cdr frame))",
    "(define (add-binding-to-frame! var val frame) (set-car! frame (cons var (car frame))) (set-cdr! frame (cons val (cdr frame))))",
    "(define (extend-environment vars vals base-env) (if (= (length vars) (length vals)) (cons (make-frame vars vals) base-env) (if (< (length vars) (length vals)) (display \"Too many arguments supplied\") (display \"Too few arguments supplied\"))))",
    "(define (lookup-variable-value var env) (define (env-loop env) (define (scan vars vals) (cond ((null? vars) (env-loop (enclosing-environment env))) ((eq? var (car vars)) (car vals)) (else (scan (cdr vars) (cdr vals))))) (if (eq? env the-empty-environment) (display \"Unbound variable\") (let ((frame (first-frame env))) (scan (frame-variables frame) (frame-values frame))))) (env-loop env))",
    "(define (set-variable-value! var val env) (define (env-loop env) (define (scan vars vals) (cond ((null? vars) (env-loop (enclosing-environment env))) ((eq? var (car vars)) (set-car! vals val)) (else (scan (cdr vars) (cdr vals))))) (if (eq? env the-empty-environment) (display \"Unbound variable -- SET!\") (let ((frame (first-frame env))) (scan (frame-variables frame) (frame-values frame))))) (env-loop env))",
    "(define (define-variable! var val env) (let ((frame (first-frame env))) (define (scan vars vals) (cond ((null? vars) (add-binding-to-frame! var val frame)) ((eq? var (car vars)) (set-car! vals val)) (else (scan (cdr vars) (cdr vals))))) (scan (frame-variables frame) (frame-values frame))))",
    "(define (setup-environment) (let ((initial-env (extend-environment (primitive-procedure-names) (primitive-procedure-objects) the-empty-environment))) initial-env))",
    "(define (primitive-procedure? proc) (tagged-list-car? proc 'primitive))",
    "(define (primitive-implementation proc) (cadr proc))",
    "(define primitive-procedures (list (list '+ +) (list '- -) (list '* *) (list '/ /) (list '= =) (list 'car car) (list 'cdr cdr) (list 'cons cons) (list 'null? null?)))",
    "(define (primitive-procedure-names) (map car primitive-procedures))",
    "(define (primitive-procedure-objects) (map (lambda (proc) (list 'primitive (cadr proc))) primitive-procedures))",
    "(define (apply-primitive-procedure proc args) (apply-in-underlying-scheme (primitive-implementation proc) args))",
    "(define input-prompt \";;; M-Eval input:\")",
    "(define output-prompt \";;; M-Eval value:\")",
    "(define (driver-loop) (prompt-for-input input-prompt) (let ((input (read))) (let ((output (eval input the-global-environment))) (announce-output output-prompt) (display output))) (driver-loop))",
    "(define (prompt-for-input string) (newline) (newline) (display string) (newline))",
    "(define (announce-output string) (newline) (display string) (newline))",
    "(define the-global-environment (setup-environment))",
};

void load_lisp_evaluator() {
    kprint("load lisp evaluator==>START\n");
    for (size_t i = 0; i < exps_len; i++) {
        char *str = exps[i];
        size_t len = strlen(str);
        void *exp = save_str_to_pair(str, len);
        element_t ele = lisp_exec(exp);
        print_element(ele);
        kprint("\n");
    }
    kprint("load lisp evaluator<==END\n");
}