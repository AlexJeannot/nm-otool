#include "../../incs/nm_otool.h"

/*
** Return TRUE if program is nm
*/
int8_t  isNm(const t_env *env) {
    return (env->info.prog == NM);
}

/*
** Return TRUE if program is otool
*/
int8_t  isOtool(const t_env *env) {
    return (env->info.prog == OTOOL);
}

/*
** Return TRUE if architecture is 32 bits
*/
int8_t  isArch32(const t_env *env) {
    return (env->info.arch == ARCH_32);
}

/*
** Return TRUE if symbol type is defined
*/
int8_t  isTypeDef(const char c) {
    return (c != 'U' && c != '?');
}

/*
** Return TRUE if section is __TEXT, __text
*/
int8_t  isTextSection(const char *section) {
    return ((strncmp(section, "__text", 6) == 0) ? TRUE : FALSE);
}

/*
** Return TRUE if offset is not superior of file/subfile size
*/
int8_t  controlOverflow(uint64_t size, uint64_t offset) {
    return (offset <= size);
}