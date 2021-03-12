#include "../../incs/base.h"

void parseHeader(t_env *env, void *file)
{
    env->header = (struct mach_header_64 *)file;
    if (env->header->magic == MH_MAGIC || env->header->magic == MH_CIGAM)
        env->arch = ARCH_32;
    else if (env->header->magic == MH_MAGIC_64 || env->header->magic == MH_CIGAM_64)
        env->arch = ARCH_64;

    if (env->header->magic == MH_CIGAM || env->header->magic == MH_CIGAM_64)
        env->s_bytes = TRUE;

    if (env->arch == 0)
        errorExit("Invalid magic number\n", env->target.name[env->target.id]);
}

void addSectionList(t_env *env, t_section *new_section)
{
    t_section *tmp;

    if (!(tmp = env->section_list))
        env->section_list = new_section;
    else {
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_section;
    }
}

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

void parseSegment32(t_env *env,void* l_cmd, uint64_t g_offset)
{
    struct segment_command  *segment;
    struct section          *section;
    t_section               *new_section;
    static uint16_t         nb_sect = 1;
    uint64_t                offset;

    offset = sizeof(struct segment_command);
    segment = (struct segment_command *)l_cmd;
    for (int count = 0; count < segment->nsects; count++) {
        if (!(controlOverflow(env->file.size, g_offset + offset)))
            errorExit("Overflow on 32-bits segment parsing", env->target.name[env->target.id]);
        section = (struct section *)&(l_cmd[offset]);
        if (isNm(env)) {
            if (!(new_section = (t_section *)malloc(sizeof(t_section))))
                errorExit("Section memory allocation", env->target.name[env->target.id]);
            bzero(new_section, sizeof(t_section));
            new_section->id = env->nb_sect++;
            strncpy(new_section->segname, section->segname, 15);
            strncpy(new_section->sectname, section->sectname, 15);
            new_section->next = NULL;
            addSectionList(env, new_section);
        }
        else if (strncmp(section->sectname, "__text", 6) == 0) {
            env->data.section.text_32 = section;
            break ;
        }
        offset += sizeof(struct section);
    }
}

void parseSegment64(t_env *env, void* l_cmd, uint64_t g_offset)
{
    struct segment_command_64  *segment;
    struct section_64          *section;
    t_section                   *new_section;
    static uint16_t             nb_sect = 1;
    uint64_t                    offset;

    offset = sizeof(struct segment_command_64);
    segment = (struct segment_command_64 *)l_cmd;
    for (int count = 0; count < segment->nsects; count++) {
        if (!(controlOverflow(env->file.size, g_offset + offset)))
            errorExit("Overflow on 64-bits segment parsing", env->target.name[env->target.id]);
        section = (struct section_64 *)&(l_cmd[offset]);
        if (isNm(env)) {
            if (!(new_section = (t_section *)malloc(sizeof(t_section))))
                errorExit("Section memory allocation", env->target.name[env->target.id]);
            bzero(new_section, sizeof(t_section));
            new_section->id = env->nb_sect++;
            strncpy(new_section->segname, section->segname, 15);
            strncpy(new_section->sectname, section->sectname, 15);
            new_section->next = NULL;
            addSectionList(env, new_section);
        }
        else if (strncmp(section->sectname, "__text", 6) == 0) {
            env->data.section.text_64 = section;
            break ;
        }
        offset += sizeof(struct section_64);
    }
}

char getSymbolType(const t_env *env, uint8_t s_type, uint8_t section, uint64_t value)
{
    char type;
    t_section *tmp;

    tmp = NULL;
    type = '?';
    if (s_type & N_STAB)
        return ('-');
    if ((s_type & N_TYPE) == N_UNDF && s_type & N_EXT)
        type = (value) ? 'C' : 'U';
    if ((s_type & N_TYPE) == N_ABS)
        type = 'A';
    if ((s_type & N_TYPE) == N_SECT) {
        tmp = env->section_list;
        while (tmp && tmp->id != section)
            tmp = tmp->next;
        if (!(tmp))
            errorExit("Symbol section ID not corresponding", env->target.name[env->target.id]);
        if (!(strncmp(tmp->segname, "__TEXT", 6)))
            type = 'T';
        else if (!(strncmp(tmp->segname, "__DATA", 6)) && !(strncmp(tmp->sectname, "__data", 6)))
            type = 'D';
        else if (!(strncmp(tmp->segname, "__DATA", 6)) && !(strncmp(tmp->sectname, "__bss", 5)))
            type = 'B';
        else if (!(strncmp(tmp->segname, "__DATA", 6)) && !(strncmp(tmp->sectname, "__common", 8)))
            type = 'S';
        if (!(s_type & N_EXT))
            type += 32;
    }
    if ((s_type & N_TYPE) == N_INDR)
        type = 'I';
    return (type);
}

void getSymbols32(t_env *env, void *file, struct symtab_command *sym_cmd)
{
    struct nlist *symbol;
    t_symbol        *new_symbol;
    uint64_t        offset;

    offset = sym_cmd->symoff;
    for (int count = 0; count < sym_cmd->nsyms; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit("Overflow on 32-bits symbol table parsing", env->target.name[env->target.id]);
        symbol = (struct nlist *)&(file[offset]);
        if (!(new_symbol = (t_symbol *)malloc(sizeof(t_symbol))))
            errorExit("Symbol memory allocation", env->target.name[env->target.id]);
        bzero(new_symbol, sizeof(t_symbol));
        new_symbol->addr = (uint64_t)symbol->n_value;
        new_symbol->type = getSymbolType(env, symbol->n_type, symbol->n_sect, (uint64_t)symbol->n_value);
        if (!(controlOverflow(env->file.size, sym_cmd->stroff + symbol->n_un.n_strx)))
            errorExit("Overflow on 32-bits string table parsing", env->target.name[env->target.id]);
        new_symbol->name = &file[sym_cmd->stroff + symbol->n_un.n_strx];
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

    offset = sym_cmd->symoff;
    for (int count = 0; count < sym_cmd->nsyms; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit("Overflow on 64-bits symbol table parsing", env->target.name[env->target.id]);

        symbol = (struct nlist_64 *)&(file[offset]);
        if (!(new_symbol = (t_symbol *)malloc(sizeof(t_symbol))))
            errorExit("Symbol memory allocation", env->target.name[env->target.id]);
        bzero(new_symbol, sizeof(t_symbol));
        new_symbol->addr = symbol->n_value;
        new_symbol->type = getSymbolType(env, symbol->n_type, symbol->n_sect, symbol->n_value);
        if (!(controlOverflow(env->file.size, sym_cmd->stroff + symbol->n_un.n_strx)))
            errorExit("Overflow on 64-bits string table parsing", env->target.name[env->target.id]);
        new_symbol->name = &file[sym_cmd->stroff + symbol->n_un.n_strx];
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

void parseLoadCommands(t_env *env, void *file)
{
    struct load_command *l_cmd;
    uint64_t offset;

    offset = (env->arch == ARCH_32) ? sizeof(struct mach_header) : sizeof(struct mach_header_64);
    for (int count = 0; count < env->header->ncmds; count++) {

        if (!(controlOverflow(env->file.size, offset)))
            errorExit("Overflow on load command parsing", env->target.name[env->target.id]);
        l_cmd = (struct load_command *)&(file[offset]);
        switch (l_cmd->cmd) {
            case (LC_SEGMENT):      parseSegment32(env, l_cmd, offset);   break;
            case (LC_SEGMENT_64):   parseSegment64(env, l_cmd, offset);   break;
            case (LC_SYMTAB):       parseSymtab(env, file, l_cmd);       break;
            default:                                           break;
        }
        offset += l_cmd->cmdsize;
    }
}