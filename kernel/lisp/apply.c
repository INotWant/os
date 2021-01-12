#include "apply.h"
#include "pair.h"
#include "procedure.h"
#include "eval.h"
#include "env.h"
#include "constant.h"

element_t apply(void *procedure, void *arguments) {
    if (is_primitive_procedure(procedure)) {
        return primitive_procedure_impl(procedure)(arguments);
    } else if (is_compound_procedure(procedure)) {
        void *var_names = procedure_parameters(procedure);
        void *old_env = procedure_env(procedure);
        void *new_env = extend_env(var_names, arguments, old_env);
        void *exps = procedure_body(procedure);
        /* 更新 root 表中的 env 指针 -- for GC of pair */
        update_env_point(new_env);
        return eval_sequence(exps, new_env);
    } else 
        eval_error_handler(UNKNOWN_PROC_TYPE);
}
