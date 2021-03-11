#include "../../incs/nm.h"
#include "../../incs/otool.h"

void display_header(const t_env * env)
{

    printf("header->magic = 0x%x\n", env->header->magic);
    switch (env->header->magic) {
        case MH_MAGIC:      printf("MH_MAGIC\n");       break;
        case MH_CIGAM:      printf("MH_CIGAM\n");       break;
        case MH_MAGIC_64:   printf("MH_MAGIC_64\n");    break;
        case MH_CIGAM_64:   printf("MH_CIGAM_64\n");    break;
        default:            printf("unknown?\n");       break;
    }
    printf("header->cputype = %d\n", env->header->cputype);
    printf("header->cpusubtype = %d\n", env->header->cpusubtype);
    printf("header->filetype = %d\n", env->header->filetype);
    printf("header->ncmds = %d\n", env->header->ncmds);
    printf("header->sizeofcmds = %d\n", env->header->sizeofcmds);
}

void display_section(const t_env * env)
{
    printf("\033[38;5;99m+++++++++++++++++++++++++++++++\033[0m\n");
    printf("section->sectname = \033[38;5;220m%s\033[0m\n", env->data.section.text_64->sectname);
    printf("section->segname = \033[38;5;39m%s\033[0m\n", env->data.section.text_64->segname);
    printf("section->addr = %llu\n", env->data.section.text_64->addr);
    printf("section->size = %llu\n", env->data.section.text_64->size);
    printf("section->offset = %d\n", env->data.section.text_64->offset);
    printf("section->align = %d\n", env->data.section.text_64->align);
    printf("section->reloff = %d\n", env->data.section.text_64->reloff);
    printf("section->nreloc = %d\n", env->data.section.text_64->nreloc);
    printf("section->flags = %u\n", env->data.section.text_64->flags);
    printf("section->reserved1 = %d\n", env->data.section.text_64->reserved1);
    printf("section->reserved2 = %d\n", env->data.section.text_64->reserved2);
    printf("section->reserved3 = %d\n", env->data.section.text_64->reserved3);
    printf("\033[38;5;99m+++++++++++++++++++++++++++++++\033[0m\n");
}

void displaySymbols(const t_env *env)
{
    printf("\n===================================================================\n");

    t_symbol *tmp;
    tmp = env->data.symbol.list;
    while (tmp)
    {
        if (isTypeDef(tmp->type))
            printf("%016llx", tmp->addr);
        else
            printf("                ");
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
void    sortSymbolList(t_env * env)
{
    t_symbol    *p_sym;
    t_symbol *f_sym;
    t_symbol *s_sym;

    p_sym = NULL;
    f_sym = env->data.symbol.list;
    s_sym = f_sym->next;
    while (f_sym && s_sym) {
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