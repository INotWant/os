;; util
(define (or arg1 arg2)
    (if arg1
        'true
        (if arg2
            'true
            'false)))

(define (and arg1 arg2)
    (if arg1
        (if arg2
            'true
            'false)
        'false))

(define (not arg)
    (if arg
        'false
        'true))

(define (caar p)
    (car (car p)))

(define (cadr p)
    (car (cdr p)))

(define (cddr p)
    (cdr (cdr p)))

(define (caddr p)
    (car (cddr p)))

(define (caadr p)
    (car (cadr p)))

(define (cdadr p)
    (cdr (cadr p)))

(define (cdddr p)
    (cdr (cddr p)))

(define (cadddr p)
    (car (cdddr p)))

(define (caaadr p)
    (car (caadr p)))

(define (tagged-list-car? exp tag)
    (if (pair? exp)
        (eq? (car exp) tag)
        'false))

(define (tagged-list-caar? exp tag)
    (if (pair? exp)
        (eq? (caar exp) tag)
        'false))

(define (length p)
    (if (null? p)
        0
        (+ 1 (length (cdr p)))))

(define (map f p)
    (if (null? p)
        '()
        (cons (f (car p)) (map f (cdr p)))))

;; 表达式
(define (self-evaluating? exp)
    (or (number? (car exp))
        (string? (car exp))))

(define (variable? exp)
    (symbol? (car exp)))

(define (text-of-quotation exp)
    (quotation-text exp)

(define (assignment? exp)
    (tagged-list-caar? exp 'set!))

(define (assignment-variable exp) (caadr exp))

(define (assignment-value exp) (caddr exp))

(define (definition? exp)
    (tagged-list-caar? exp 'define))

(define (definition-variable exp)
    (if (symbol? (caadr exp))
        (caadr exp)
        (caaadr exp)))

(define (definition-value exp)
    (if (symbol? (caadr exp))
        (caddr exp)
        (make-lambda (cdadr exp)
                     (cddr exp))))

(define (lambda? exp) (tagged-list-caar? exp 'lambda))

(define (lambda-parameters-helper parameters)
    (if (null? parameters)
        '()
        (cons (caar parameters) (lambda-parameters-helper (cdr parameters)))))

(define (lambda-parameters exp) (lambda-parameters-helper (cadr exp)))

(define (lambda-body exp) (cddr exp))

(define (make-lambda parameters body)
    (cons (cons 'lambda '()) (cons parameters body)))

(define (if? exp) (tagged-list-caar? exp 'if))

(define (if-predicate exp) (cadr exp))

(define (if-consequent exp) (caddr exp))

(define (if-alternative exp)
    (if (not (null? (cdddr exp)))
        (cadddr exp)
        'false))

(define (make-if predicate consequent alternative)
    (list (cons 'if '()) predicate consequent alternative))

(define (begin? exp) (tagged-list-caar? exp 'begin))

(define (begin-actions exp) (cdr exp))

(define (last-exp? seq) (null? (cdr seq)))

(define (first-exp seq) (car seq))

(define (rest-exps seq) (cdr seq))

(define (sequence->exp seq)
    (cond ((null? seq) seq)
        ((last-exp? seq) (first-exp seq))
        (else (make-begin seq))))

(define (make-begin seq) (cons (cons 'begin '()) seq))

(define (application? exp) (pair? exp))

(define (operator exp) (car exp))

(define (operands exp) (cdr exp))

(define (no-operands? ops) (null? ops))

(define (first-operand ops) (car ops))

(define (rest-operands ops) (cdr ops))

(define (eval exp env)
    ((analyze exp) env))

(define (analyze exp)
    (cond ((self-evaluating? exp) (analyze-self-evaluating (car exp)))
           ((variable? exp) (analyze-variable (car exp)))
           ((quoted? (car exp)) (analyze-quoted (car exp)))
           ((assignment? exp) (analyze-assignment exp))
           ((definition? exp) (analyze-definition exp))
           ((if? exp) (analyze-if exp))
           ((lambda? exp) (analyze-lambda exp))
           ((begin? exp) (analyze-sequence (begin-actions exp)))
           ((application? exp) (analyze-application exp))
           (else (display "Unknown expression type -- EVAL") (newline))))

(define (analyze-self-evaluating exp) (lambda (env) exp))

(define (analyze-quoted exp)
    (let ((qval (text-of-quotation exp)))
        (lambda (env) qval)))

(define (analyze-variable exp)
    (lambda (env) (lookup-variable-value exp env)))

(define (analyze-assignment exp)
    (let ((var (assignment-variable exp))
         (vproc (analyze (assignment-value exp))))
        (lambda (env)
            (set-variable-value! var (vproc env) env)
            'ok)))

(define (analyze-definition exp)
    (let ((var (definition-variable exp))
         (vproc (analyze (definition-value exp))))
        (lambda (env)
            (define-variable! var (vproc env) env)
            'ok)))

(define (analyze-if exp)
    (let ((pproc (analyze (if-predicate exp)))
          (cproc (analyze (if-consequent exp)))
          (aproc (analyze (if-alternative exp))))
        (lambda (env)
            (if (pproc env)
                (cproc env)
                (aproc env)))))

(define (analyze-lambda exp)
    (let ((vars (lambda-parameters exp))
          (bproc (analyze-sequence (lambda-body exp))))
        (lambda (env) (make-procedure vars bproc env))))

(define (analyze-sequence exps)
    (define (sequentially proc1 proc2)
        (lambda (env) (proc1 env) (proc2 env)))
    (define (loop first-proc rest-procs)
        (if (null? rest-procs)
            first-proc
            (loop (sequentially first-proc (car rest-procs))
                  (cdr rest-procs))))
    (let ((procs (map analyze exps)))
        (if (null? procs)
            (display "Empty sequence -- ANALYZE"))
            (loop (car procs) (cdr procs))))

(define (analyze-application exp)
    (let ((fproc (analyze (operator exp)))
          (aprocs (map analyze (operands exp))))
        (lambda (env)
            (execute-application (fproc env)
                                 (map (lambda (aproc) (aproc env))
                                      aprocs)))))

(define apply-in-underlying-scheme apply)

(define (execute-application proc args)
  (cond ((primitive-procedure? proc)
         (apply-primitive-procedure proc args))
        ((compound-procedure? proc)
         ((procedure-body proc)
          (extend-environment (procedure-parameters proc)
                              args
                              (procedure-environment proc))))
        (else (display "Unknown procedure type -- EXECUTE-APPLICATION"))))

;; env
(define (make-procedure parameters body env)
    (list 'procedure parameters body env))

(define (compound-procedure? p)
    (tagged-list-car? p 'procedure))

(define (procedure-parameters p) (cadr p))

(define (procedure-body p) (caddr p))

(define (procedure-environment p) (cadddr p))

(define (enclosing-environment env) (cdr env))

(define (first-frame env) (car env))

(define the-empty-environment '())

(define (make-frame variables values)
    (cons variables values))

(define (frame-variables frame) (car frame))

(define (frame-values frame) (cdr frame))

(define (add-binding-to-frame! var val frame)
    (set-car! frame (cons var (car frame)))
    (set-cdr! frame (cons val (cdr frame))))

(define (extend-environment vars vals base-env)
    (if (= (length vars) (length vals))
        (cons (make-frame vars vals) base-env)
        (if (< (length vars) (length vals))
            (begin (display "Too many arguments supplied") (newline))
            (begin (display "Too few arguments supplied") (newline)))))

(define (lookup-variable-value var env)
    (define (env-loop env)
        (define (scan vars vals)
            (cond ((null? vars) (env-loop (enclosing-environment env)))
                  ((eq? var (car vars)) (car vals))
                  (else (scan (cdr vars) (cdr vals)))))
        (if (eq? env the-empty-environment)
            (begin (display "Unbound variable") (newline))
            (let ((frame (first-frame env)))
                (scan (frame-variables frame)
                      (frame-values frame)))))
    (env-loop env))

(define (set-variable-value! var val env)
    (define (env-loop env)
        (define (scan vars vals)
            (cond ((null? vars) (env-loop (enclosing-environment env)))
                ((eq? var (car vars)) (set-car! vals val))
                (else (scan (cdr vars) (cdr vals)))))
        (if (eq? env the-empty-environment)
            (begin (display "Unbound variable -- SET!") (newline))
            (let ((frame (first-frame env)))
                (scan (frame-variables frame)
                    (frame-values frame)))))
    (env-loop env))

(define (define-variable! var val env)
    (let ((frame (first-frame env)))
        (define (scan vars vals)
            (cond ((null? vars) (add-binding-to-frame! var val frame))
                ((eq? var (car vars)) (set-car! vals val))
                (else (scan (cdr vars) (cdr vals)))))
        (scan (frame-variables frame)
            (frame-values frame))))

;; 
(define (setup-environment)
    (let ((initial-env
            (extend-environment (primitive-procedure-names)
                                (primitive-procedure-objects)
                                the-empty-environment)))
        initial-env))

(define (primitive-procedure? proc)
    (tagged-list-car? proc 'primitive))

(define (primitive-implementation proc) (cadr proc))

(define primitive-procedures
    (list 
          (list '+ +)
          (list '- -)
          (list '* *)
          (list '/ /)
          (list '= =)
          (list 'car car)
          (list 'cdr cdr)
          (list 'cons cons)
          (list 'null? null?)
          (list 'display display)
          (list 'newline newline)))

(define (primitive-procedure-names)
    (map car primitive-procedures))

(define (primitive-procedure-objects)
    (map (lambda (proc) (list 'primitive (cadr proc))) primitive-procedures))

(define (apply-primitive-procedure proc args)
    (apply-in-underlying-scheme (primitive-implementation proc) args))

(define input-prompt ";;; M-Eval input:")

(define output-prompt ";;; M-Eval value:")

(define (driver-loop)
    (prompt-for-input input-prompt)
    (let ((input (read)))
        (let ((output (eval input the-global-environment)))
        (announce-output output-prompt)
        (display output)))
    (driver-loop))

(define (prompt-for-input string)
    (newline) (newline) (display string) (newline))

(define (announce-output string)
    (newline) (display string) (newline))

(define the-global-environment (setup-environment))

(driver-loop)
