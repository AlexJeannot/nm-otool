#include "../../incs/nm_otool.h"

void errorExit(t_env *env, char *error)
{
    (isNm(env)) ? printf("ft_nm") : printf("ft_otool");
    printf(": error: ");
    if (env->file.name)
        printf("%s: ", env->file.name);
    printf("%s\n", error);
    clearAll(env);
    exit(1);
}