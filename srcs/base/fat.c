#include "../../incs/nm_otool.h"

/*
** Verify if file is a FAT binary
** If it is then parse FAT header
** Set variable depending on FAT architecture
** Set variable depending on endianness
** Set variable depending on number of sub architecture
*/
uint32_t isFatBinary(t_env *env, void *file)
{
    struct fat_header *f_header;

    f_header = (struct fat_header *)file;
    
    if (f_header->magic == FAT_MAGIC || f_header->magic == FAT_CIGAM)
        env->fathdr.arch = ARCH_32;
    else if (f_header->magic == FAT_MAGIC_64 || f_header->magic == FAT_CIGAM_64)
        env->fathdr.arch = ARCH_64;
    else
        return (FALSE);

    if (f_header->magic == FAT_CIGAM || f_header->magic == FAT_CIGAM_64)
            env->fathdr.s_bytes = TRUE;
    env->fathdr.n_arch = ifSwapuInt32(env->fathdr.s_bytes, f_header->nfat_arch);
    return (TRUE);
}

/*
** Get address of subfile first byte for 32 bits architecture
** arch_header_offset is increment by FAT header and FAT arch header multiple by number of arch
** Set subfile address and subfile size
*/
void getSubFile32(t_env *env, void *file, uint32_t arch_pos)
{
    struct fat_arch *arch_header;
    uint32_t        arch_header_offset, subfile_offset;

    arch_header_offset = sizeof(struct fat_header) + (sizeof(struct fat_arch) * arch_pos);
    arch_header = (struct fat_arch *)&file[arch_header_offset];

    subfile_offset = ifSwapuInt32(env->fathdr.s_bytes, arch_header->offset);
    if (!(controlOverflow(env->file.size, subfile_offset)))
        errorExit(env, "Overflow on 32-bits FAT binary subfile header offset");

    env->fathdr.subsize = (uint64_t)ifSwapuInt32(env->fathdr.s_bytes, arch_header->size);
    if (!(controlOverflow(env->file.size, subfile_offset + env->fathdr.subsize)))
        errorExit(env, "Overflow on 32-bits FAT binary subfile size");

    env->fathdr.subfile = &file[subfile_offset];
}

/*
** Get address of subfile first byte for 64 bits architecture
** arch_header_offset is increment by FAT header and FAT arch header multiple by number of arch
** Set subfile address and subfile size
*/
void getSubFile64(t_env *env, void *file, uint32_t arch_pos)
{
    struct fat_arch_64  *arch_header;
    uint64_t            arch_header_offset, subfile_offset;

    arch_header_offset = sizeof(struct fat_header) + (sizeof(struct fat_arch_64) * arch_pos);
    arch_header = (struct fat_arch_64 *)&file[arch_header_offset];

    subfile_offset = ifSwapuInt64(env->fathdr.s_bytes, arch_header->offset);
    if (!(controlOverflow(env->file.size, subfile_offset)))
        errorExit(env, "Overflow on 64-bits FAT binary subfile header offset");

    env->fathdr.subsize = ifSwapuInt64(env->fathdr.s_bytes, arch_header->size);
    if (!(controlOverflow(env->file.size, subfile_offset + env->fathdr.subsize)))
        errorExit(env, "Overflow on 64-bits FAT binary subfile size");

    env->fathdr.subfile = &file[subfile_offset];
}