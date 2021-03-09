#include "../../incs/base.h"

int8_t isNm(const t_env *env) {
    return (env->prog == NM);
}

int8_t isOtool(const t_env *env) {
    return (env->prog == OTOOL);
}