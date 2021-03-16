#include "../../incs/nm_otool.h"

int8_t  isNm(const t_env *env) {
    return (env->info.prog == NM);
}

int8_t  isOtool(const t_env *env) {
    return (env->info.prog == OTOOL);
}

int8_t  isArch32(const t_env *env) {
    return (env->info.arch == ARCH_32);
}

int8_t  isTypeDef(const char c) {
    return (c != 'U' && c != '?');
}

int8_t  isTextSection(const char *section) {
    return ((strncmp(section, "__text", 6) == 0) ? TRUE : FALSE);
}

int8_t  controlOverflow(uint64_t size, uint64_t offset) {
    return (offset <= size);
}