#include "../../incs/nm_otool.h"

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
        errorExit(env, "Invalid magic number\n", env->target.name[env->target.id]);
}

void parseLoadCommands(t_env *env, void *file)
{
    struct load_command *l_cmd;
    uint64_t            offset;
    uint32_t            ncmds;

    ncmds = ifSwapuInt32(env->s_bytes, env->header->ncmds);
    offset = (env->arch == ARCH_32) ? sizeof(struct mach_header) : sizeof(struct mach_header_64);

    for (int count = 0; count < ncmds; count++) {
        if (!(controlOverflow(env->file.size, offset)))
            errorExit(env, "Overflow on load command parsing", env->target.name[env->target.id]);
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