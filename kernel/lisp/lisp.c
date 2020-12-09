#include "lisp.h"
#include "primitive_procedure.h"
#include "env.h"
#include "eval.h"

void *env;

void lisp_init() {
    void *pp_names = primitive_procedure_names();
    void *pp_objects = primitive_procedure_objects();
    env = extend_env(pp_names, pp_objects, 0);
    /* 更新 root 表中的 env 指针 -- for GC of pair */
    update_env_point(env);
}

element_t lisp_exec(void *exp) {
    return eval(exp, env);
}
