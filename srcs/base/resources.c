#include "../../incs/nm_otool.h"

/*
** Set next file variables
** File architecture
** File swap bytes bool
** Number of section at 1
** clear linked lists
*/
void setNextFile(t_env *env)
{
    env->info.arch = 0;
    env->info.s_bytes = 0;
    env->info.nsect = 1;
    clearSection(env);
    (isNm(env)) ? clearSymbol(env) : clearTextSect(env);
    clearFile(env);
    bzero(&env->data, sizeof(env->data));
}

/*
** Set next file variables
** File architecture
** File swap bytes bool
** Number of section at 1
** Subfile size
** clear linked lists
*/
void setNextSubFile(t_env *env)
{
    env->info.arch = 0;
    env->info.s_bytes = 0;
    env->info.nsect = 1;
    env->fathdr.subsize = 0;
    clearSection(env);
    (isNm(env)) ? clearSymbol(env) : clearTextSect(env);
    bzero(&env->data, sizeof(env->data));
}

/*
** Open file and get map of it
*/
void *getMap(t_env *env, char *input)
{
    struct stat sb;
    void        *file;

    env->file.name = input;
    if ((env->file.fd = open(input, O_RDONLY)) == -1)
        errorExit(env, "No such file or directory");
    if (fstat(env->file.fd, &sb) == -1)
        errorExit(env, "File access");
    if ((file = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, env->file.fd, 0)) == MAP_FAILED)
        errorExit(env, "Map access");

    env->file.size = sb.st_size;
    env->file.addr = file;
    return (file);
}