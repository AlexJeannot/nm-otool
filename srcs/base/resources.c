#include "../../incs/nm_otool.h"

void setNextFile(t_env *env)
{
    env->arch = 0;
    env->s_bytes = 0;
    env->nb_sect = 1;
    clearSection(env);
    clearSymbol(env);
    clearFile(env);
    bzero(&env->data, sizeof(env->data));
}

void setNextSubFile(t_env *env)
{
    env->arch = 0;
    env->s_bytes = 0;
    env->nb_sect = 1;
    env->file.subsize = 0;
    clearSection(env);
    clearSymbol(env);
    bzero(&env->data, sizeof(env->data));
}

void *getMap(t_env *env, char *input)
{
    struct stat sb;
    void        *file;

    if ((env->file.fd = open(input, O_RDONLY)) == -1)
        errorExit(env, "No such file or directory", input);
    if (fstat(env->file.fd, &sb) == -1)
        errorExit(env, "File access", input);
    if ((file = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, env->file.fd, 0)) == MAP_FAILED)
        errorExit(env, "Map access", input);

    env->file.size = sb.st_size;
    env->file.name = input;
    env->file.addr = file;
    return (file);
}