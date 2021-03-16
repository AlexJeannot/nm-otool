#include "../../incs/nm_otool.h"

void errorExit(t_env *env, char *error, char *exec)
{
    (env->prog == NM) ? printf("ft_nm") : printf("ft_otool");
    printf(": error: ");
    if (exec)
        printf("%s: ", exec);
    printf("%s\n", error);
    clearAll(env);
    exit(1);
}