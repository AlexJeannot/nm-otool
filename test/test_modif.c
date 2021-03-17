#include "../incs/nm_otool.h"

void errorExit(t_env *env, char *error)
{
    fprintf(stderr, "Error: ");
    if (env->file.name)
        fprintf(stderr, "%s: ", env->file.name);
    fprintf(stderr, "%s\n", error);
    exit(1);
}

void *getMap(t_env *env, char *input)
{
    struct stat sb;
    void        *file;

    env->file.name = input;
    if ((env->file.fd = open(input, O_RDWR)) == -1)
        errorExit(env, "No such file or directory");
    if (fstat(env->file.fd, &sb) == -1)
        errorExit(env, "File access");
    if ((file = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, env->file.fd, 0)) == MAP_FAILED)
        errorExit(env, "Map access");

    env->file.size = sb.st_size;
    env->file.addr = file;
    return (file);
}


int main()
{
    void *file;
    t_env env;
    struct load_command *l_cmd;
    uint64_t            g_offset;
    uint64_t            seg_offset;
    struct mach_header_64 *header;
    struct segment_command_64   *segment;
    struct section_64           *section;
    struct symtab_command       *sym_cmd;

    file = getMap(&env, "b_modif");
    header = file;
    g_offset = sizeof(struct mach_header_64);

    // header->ncmds = 1;
    for (uint32_t lc_count = 0; lc_count < header->ncmds; lc_count++) {
        l_cmd = &file[g_offset];
        if (l_cmd->cmd == LC_SEGMENT_64) {
            segment = &file[g_offset];
            if (strcmp("__TEXT", segment->segname) == 0) {
                seg_offset = g_offset + sizeof(struct segment_command_64);
                for (uint32_t sect_count = 0; sect_count < segment->nsects; sect_count++) {
                    section = &file[seg_offset];
                    if (strcmp("__text", section->sectname) == 0) {
                        printf("IN SECTION __TEXT, __text\n");
                        section->offset = 999999;
                    }
                    seg_offset += sizeof(struct segment_command_64);
                }
            }
        }
        else if (l_cmd->cmd == LC_SYMTAB) {
            printf("IN LC_SYMTAB\n");
            sym_cmd = (struct symtab_command *)l_cmd;
            sym_cmd->stroff = 999999;
        }
        g_offset += l_cmd->cmdsize;
    }
    munmap(file, env.file.size);
    close(env.file.fd);
    return (0);
}