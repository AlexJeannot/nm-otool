#include "../../incs/nm_otool.h"

void clearLib(t_env *env)
{
    t_lib_obj *prev, *tmp;

    tmp = env->lib_objs;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->lib_objs = NULL;
}

void clearSection(t_env *env)
{
    t_section *prev, *tmp;

    tmp = env->section_list;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->section_list = NULL;
}

void clearSymbol(t_env *env)
{
    t_symbol_list *prev, *tmp;

    tmp = env->data.symbol.list;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->data.symbol.list = NULL;
}

void clearTextSect(t_env *env)
{
    bzero(&env->data.text, sizeof(t_text));
}

void clearFile(t_env *env)
{
    munmap(env->file.addr, env->file.size);
    if (env->file.fd >= 0)
        close(env->file.fd);
    bzero(&env->file, sizeof(t_file));
    env->file.fd = -1;
}

void clearAll(t_env *env)
{
    clearLib(env);
    clearSection(env);
    (isNm(env)) ? clearSymbol(env) : clearTextSect(env);
    clearFile(env);
    if (env->target.name)
        free(env->target.name);
}

void deleteSymbol(t_env *env, t_symbol_list *symbol)
{
    t_symbol_list *prev, *tmp;

    tmp = env->data.symbol.list;
    if (tmp == symbol) {
        env->data.symbol.list = symbol->next;
        free(tmp);
        return ;
    }
    while (tmp) {
        if (tmp == symbol) {
            if (prev)
                prev->next = tmp->next;
            free(tmp);
            return ;
        }
        prev = tmp;
        tmp = tmp->next;
    }
}