#include "../../incs/nm_otool.h"

int8_t isNm(const t_env *env) {
    return (env->prog == NM);
}

int8_t isOtool(const t_env *env) {
    return (env->prog == OTOOL);
}

int8_t  isTypeDef(char c) {
    return (c != 'U' && c != '?');
}

int8_t controlOverflow(uint64_t size, uint64_t offset) {
    return (offset <= size);
}