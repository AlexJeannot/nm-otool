#include "../../incs/nm_otool.h"

/*
** If error then
** Display error, clear all resources and exit
*/
void errorExit(t_env *env, char *error)
{
    (isNm(env)) ? fprintf(stderr, "ft_nm") : fprintf(stderr, "ft_otool");
    fprintf(stderr, ": error: ");
    if (env->file.name)
        fprintf(stderr, "%s: ", env->file.name);
    fprintf(stderr, "%s\n", error);
    clearAll(env);
    exit(1);
}