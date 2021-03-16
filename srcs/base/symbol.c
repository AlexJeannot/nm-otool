#include "../../incs/nm_otool.h"

void addSymbolList(t_env *env, t_symbol *new_symbol)
{
    t_symbol *tmp;

    if (!(tmp = env->data.symbol.list))
        env->data.symbol.list = new_symbol;
    else {
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_symbol;
    }
}

char getSymbolType(t_env *env, t_symbol *symbol, uint8_t s_type, uint8_t section, uint64_t value)
{
    t_section   *tmp;
    char        type;

    tmp = NULL;
    type = '?';
    if (s_type & N_STAB)
        type = '-';
    else if ((s_type & N_TYPE) == N_UNDF && s_type & N_EXT)
        type = (value) ? 'C' : 'U';
    else if ((s_type & N_TYPE) == N_ABS)
        type = 'A';
    else if ((s_type & N_TYPE) == N_SECT) {
        tmp = env->section_list;
        while (tmp && tmp->id != section)
            tmp = tmp->next;
        if (!(tmp) || tmp->id != section)
            errorExit(env, "Symbol section ID not corresponding", env->target.name[env->target.id]);
        if (!(strncmp(tmp->segname, "__TEXT", 6)) && !(strncmp(tmp->sectname, "__text", 6)))
            type = 'T';
        else if (!(strncmp(tmp->segname, "__DATA", 6)) && !(strncmp(tmp->sectname, "__data", 6)))
            type = 'D';
        else if (!(strncmp(tmp->segname, "__DATA", 6)) && !(strncmp(tmp->sectname, "__bss", 5)))
            type = 'B';
        else
            type = 'S';
        if (!(s_type & N_EXT))
            type += 32;
    }
    else if ((s_type & N_TYPE) == N_INDR)
        type = 'I';
    return (type);
}

void getSymbols32(t_env *env, void *file, struct symtab_command *sym_cmd)
{
    struct nlist    *symbol;
    t_symbol        *new_symbol;
    uint64_t        offset;
    uint32_t        nsyms, stroff, n_strx;

    nsyms = ifSwapuInt32(env->s_bytes, sym_cmd->nsyms);
    offset = ifSwapuInt32(env->s_bytes, sym_cmd->symoff);
    stroff = ifSwapuInt32(env->s_bytes, sym_cmd->stroff);

    for (int count = 0; count < nsyms; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit(env, "Overflow on 32-bits symbol table parsing", env->target.name[env->target.id]);
        symbol = (struct nlist *)&(file[offset]);

        if (!(new_symbol = (t_symbol *)malloc(sizeof(t_symbol))))
            errorExit(env, "Symbol memory allocation", env->target.name[env->target.id]);
        bzero(new_symbol, sizeof(t_symbol));
        new_symbol->addr = ifSwapuInt32(env->s_bytes, symbol->n_value);
        new_symbol->type = getSymbolType(env, new_symbol, symbol->n_type, symbol->n_sect, new_symbol->addr);
        n_strx = ifSwapuInt32(env->s_bytes, symbol->n_un.n_strx);
        if (!(controlOverflow(env->file.size, stroff + n_strx)))
            errorExit(env, "Overflow on 32-bits string table parsing", env->target.name[env->target.id]);
        new_symbol->name = &file[stroff + n_strx];
        new_symbol->next = NULL;
        addSymbolList(env, new_symbol);

        offset += sizeof(struct nlist);
    }
}

void getSymbols64(t_env *env, void *file, struct symtab_command *sym_cmd)
{
    struct nlist_64 *symbol;
    t_symbol        *new_symbol;
    uint64_t        offset;
    uint32_t        nsyms, stroff, n_strx;

    nsyms = ifSwapuInt32(env->s_bytes, sym_cmd->nsyms);
    offset = ifSwapuInt32(env->s_bytes, sym_cmd->symoff);
    stroff = ifSwapuInt32(env->s_bytes, sym_cmd->stroff);

    for (int count = 0; count < nsyms; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit(env, "Overflow on 64-bits symbol table parsing", env->target.name[env->target.id]);
        symbol = (struct nlist_64 *)&(file[offset]);

        if (!(new_symbol = (t_symbol *)malloc(sizeof(t_symbol))))
            errorExit(env, "Symbol memory allocation", env->target.name[env->target.id]);
        bzero(new_symbol, sizeof(t_symbol));
        new_symbol->addr = symbol->n_value;
        new_symbol->type = getSymbolType(env, new_symbol, symbol->n_type, symbol->n_sect, symbol->n_value);
        n_strx = ifSwapuInt32(env->s_bytes, symbol->n_un.n_strx);
        if (!(controlOverflow(env->file.size, stroff + n_strx)))
            errorExit(env, "Overflow on 64-bits string table parsing", env->target.name[env->target.id]);
        new_symbol->name = &file[stroff + n_strx];
        new_symbol->next = NULL;
        addSymbolList(env, new_symbol);

        offset += sizeof(struct nlist_64);
    }
}

void parseSymtab(t_env *env, void *file, struct load_command *l_cmd)
{
    struct symtab_command *sym_cmd;

    if (isOtool(env)) return ;
    sym_cmd = (struct symtab_command *)l_cmd;
    (env->arch == ARCH_32) ? getSymbols32(env, file, sym_cmd) : getSymbols64(env, file, sym_cmd);
}

void processSymbol(t_env *env, void *addr, char *obj_name)
{
    parseHeader(env, addr);
    parseLoadCommands(env, addr);
    sortSymbolList(env);
    displaySymbols(env, obj_name);
}