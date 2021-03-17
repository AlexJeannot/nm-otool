#include "../../incs/nm_otool.h"

/*
** Parse file/subfile header
** Set variable depending on architecture
** Set variable depending on endianness
*/
void parseHeader(t_env *env, void *file)
{
    env->header = (struct mach_header_64 *)file;
    if (env->header->magic == MH_MAGIC || env->header->magic == MH_CIGAM)
        env->info.arch = ARCH_32;
    else if (env->header->magic == MH_MAGIC_64 || env->header->magic == MH_CIGAM_64)
        env->info.arch = ARCH_64;

    if (env->header->magic == MH_CIGAM || env->header->magic == MH_CIGAM_64)
        env->info.s_bytes = TRUE;

    if (!(env->info.arch))
        errorExit(env, "The file was not recognized as a valid object file\n");
}

/*
** Parse load commands
** Increment offset by the size of mach-o header depending on architecture
** While load commands
** -- Control overflow
** -- If 32 bits segment then parse it
** -- If 64 bits segment then parse it
** -- If symbol table then parse it
** -- Increment offset by load command size
*/
void parseLoadCommands(t_env *env, void *file)
{
    struct load_command *l_cmd;
    uint64_t            offset;
    uint32_t            ncmds;

    ncmds = ifSwapuInt32(env->info.s_bytes, env->header->ncmds);
    offset = (isArch32(env)) ? sizeof(struct mach_header) : sizeof(struct mach_header_64);

    for (uint32_t count = 0; count < ncmds; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit(env, "Overflow on load command parsing");
        l_cmd = (struct load_command *)&(file[offset]);

        switch (l_cmd->cmd) {
            case (LC_SEGMENT):      parseSegment32(env, l_cmd, offset);     break;
            case (LC_SEGMENT_64):   parseSegment64(env, l_cmd, offset);     break;
            case (LC_SYMTAB):       parseSymtab(env, file, l_cmd);          break;
            default:                                                        break;
        }
        offset += l_cmd->cmdsize;
    }
}