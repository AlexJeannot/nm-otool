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
    t_symbol *prev, *tmp;

    tmp = env->data.symbol.list;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->data.symbol.list = NULL;
}

void clearAll(t_env *env)
{
    clearLib(env);
    clearSection(env);
    clearSymbol(env);
    if (env->target.name)
        free(env->target.name);
}

void deleteSymbol(t_env *env, t_symbol *symbol)
{
    t_symbol *prev, *tmp;

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