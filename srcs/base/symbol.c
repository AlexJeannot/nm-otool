#include "../../incs/nm_otool.h"

/*
** Add symbol linked list elem
*/
void addSymbolList(t_env *env, t_symbol_list *new_symbol)
{
    t_symbol_list *tmp;

    if (!(tmp = env->data.symbol.list))
        env->data.symbol.list = new_symbol;
    else {
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_symbol;
    }
}

/*
** Get symbol type depending on bit masks
** For mask N_SECT, use section index
** If exterior symbol then capital letter
*/
char getSymbolType(t_env *env, t_symbol_list *symbol, uint8_t s_type, uint8_t section, uint64_t value)
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
            errorExit(env, "Symbol section ID not corresponding");
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

/*
** Create symbol linked list element for 32 bits architecture
** nsyms is the number of symbols
** offset is the symbol table offset
** stroff is the string table offset
** While symbols
** -- Control overflow
** -- Allocate memory for element
** -- Get symbol address
** -- Get symbol type
** -- Get symbol name using n_strx value which is the name offset in the string table
** -- Increment offset by a nlist header size
*/
void getSymbols32(t_env *env, void *file, struct symtab_command *sym_cmd)
{
    struct nlist    *symbol;
    t_symbol_list   *new_symbol;
    uint64_t        offset;
    uint32_t        nsyms, stroff, n_strx;

    nsyms = ifSwapuInt32(env->info.s_bytes, sym_cmd->nsyms);
    offset = ifSwapuInt32(env->info.s_bytes, sym_cmd->symoff);
    stroff = ifSwapuInt32(env->info.s_bytes, sym_cmd->stroff);

    for (int count = 0; count < nsyms; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit(env, "Overflow on 32-bits symbol table parsing");
        symbol = (struct nlist *)&(file[offset]);

        if (!(new_symbol = (t_symbol_list *)malloc(sizeof(t_symbol_list))))
            errorExit(env, "Symbol memory allocation");
        bzero(new_symbol, sizeof(t_symbol_list));
        new_symbol->addr = ifSwapuInt32(env->info.s_bytes, symbol->n_value);
        new_symbol->type = getSymbolType(env, new_symbol, symbol->n_type, symbol->n_sect, new_symbol->addr);
        n_strx = ifSwapuInt32(env->info.s_bytes, symbol->n_un.n_strx);
        if (!(controlOverflow(env->file.size, stroff + n_strx)))
            errorExit(env, "Overflow on 32-bits string table parsing");
        new_symbol->name = &file[stroff + n_strx];
        new_symbol->next = NULL;
        addSymbolList(env, new_symbol);

        offset += sizeof(struct nlist);
    }
}

/*
** Create symbol linked list element for 64 bits architecture
** nsyms is the number of symbols
** offset is the symbol table offset
** stroff is the string table offset
** While symbols
** -- Control overflow
** -- Allocate memory for element
** -- Get symbol address
** -- Get symbol type
** -- Get symbol name using n_strx value which is the name offset in the string table
** -- Increment offset by a nlist_64 header size
*/
void getSymbols64(t_env *env, void *file, struct symtab_command *sym_cmd)
{
    struct nlist_64 *symbol;
    t_symbol_list        *new_symbol;
    uint64_t        offset;
    uint32_t        nsyms, stroff, n_strx;

    nsyms = ifSwapuInt32(env->info.s_bytes, sym_cmd->nsyms);
    offset = ifSwapuInt32(env->info.s_bytes, sym_cmd->symoff);
    stroff = ifSwapuInt32(env->info.s_bytes, sym_cmd->stroff);

    for (int count = 0; count < nsyms; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit(env, "Overflow on 64-bits symbol table parsing");
        symbol = (struct nlist_64 *)&(file[offset]);

        if (!(new_symbol = (t_symbol_list *)malloc(sizeof(t_symbol_list))))
            errorExit(env, "Symbol memory allocation");
        bzero(new_symbol, sizeof(t_symbol_list));
        new_symbol->addr = symbol->n_value;
        new_symbol->type = getSymbolType(env, new_symbol, symbol->n_type, symbol->n_sect, symbol->n_value);
        n_strx = ifSwapuInt32(env->info.s_bytes, symbol->n_un.n_strx);
        if (!(controlOverflow(env->file.size, stroff + n_strx)))
            errorExit(env, "Overflow on 64-bits string table parsing");
        new_symbol->name = &file[stroff + n_strx];
        new_symbol->next = NULL;
        addSymbolList(env, new_symbol);

        offset += sizeof(struct nlist_64);
    }
}

/*
** Introduction function to symbol table parsing
** if otool we do not need symbols
*/
void parseSymtab(t_env *env, void *file, struct load_command *l_cmd)
{
    struct symtab_command *sym_cmd;

    if (isOtool(env)) return ;
    sym_cmd = (struct symtab_command *)l_cmd;
    (isArch32(env)) ? getSymbols32(env, file, sym_cmd) : getSymbols64(env, file, sym_cmd);
}

/*
** Process symbol
*/
void processSymbol(t_env *env, void *addr, char *obj_name)
{
    parseHeader(env, addr);
    parseLoadCommands(env, addr);
    sortSymbolList(env);
    displaySymbols(env, obj_name);
}