#include "../../incs/nm_otool.h"

void displayAddr(const t_env *env, uint64_t addr)
{
    if (isArch32(env))
        printf("%08llx", addr);
    else
        printf("%016llx", addr);
}

void displayHexText(t_env *env, void *file, char *obj_name)
{
    uint64_t    offset;
    uint64_t    final_offset;
    char        *hex;

    final_offset = env->data.text.offset + env->data.text.size;
    if (!(controlOverflow(env->file.size, final_offset)))
        errorExit(env, "Overflow on __text section");
    printf("%s", env->file.name);
    (obj_name) ? printf("(%s):\n", obj_name) : printf(":\n");
    printf("(__TEXT,__text) section");
    for (uint64_t bit = 0; bit < env->data.text.size; bit++) {
        if ((bit % 16) == 0) {
            printf("\n");
            displayAddr(env, env->data.text.addr + bit);
        }
        hex = (char *)&file[env->data.text.offset + bit];
        printf(" %02x", hex[0] & 0xFF);
    }
    printf("\n");
}

void displaySymbols(const t_env *env, char *obj_name)
{
    t_symbol_list *tmp;

    tmp = env->data.symbol.list;
    printf("\n%s", env->target.name[env->target.id]);
    (obj_name) ? printf("(%s):\n", obj_name) : printf(":\n");
    while (tmp)
    {
        if (isTypeDef(tmp->type))
            displayAddr(env, tmp->addr);
        else
            (isArch32(env)) ? printf("        ") : printf("                ");
        printf(" %c ", tmp->type);
    printf("%s\n", tmp->name);
        tmp = tmp->next;
    }
}

/*
** 
** p_sym == previous symbol elem (n - 1)
** f_sym == first symbol elem (n)
** s_sym == second symbol elem (n + 1)
*/
void    sortSymbolList(t_env *env)
{
    t_symbol_list    *p_sym;
    t_symbol_list *f_sym;
    t_symbol_list *s_sym;

    p_sym = NULL;
    f_sym = env->data.symbol.list;
    if (f_sym)
        s_sym = f_sym->next;
    while (f_sym && s_sym) {
        if (f_sym->type == '-') {
            deleteSymbol(env, f_sym);
            f_sym = s_sym;
            if (s_sym)
                s_sym = s_sym->next;
            continue ;
        }
        if (strncmp(f_sym->name, s_sym->name, strlen(f_sym->name)) > 0) {
            f_sym->next = s_sym->next;
            s_sym->next = f_sym;
            if (p_sym)
                p_sym->next = s_sym;
            else
                env->data.symbol.list = s_sym;
            p_sym = NULL;
            f_sym = env->data.symbol.list;
        }
        else {
            p_sym = f_sym;
            f_sym = f_sym->next;
        }
        s_sym = f_sym->next;
    }
}