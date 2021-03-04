#include <stdio.h>
#include <stdlib.h>
#include <mach-o/loader.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <mach-o/fat.h>
#include <string.h>
#include <mach-o/nlist.h>

void error_exit(char *section)
{

    printf("%s: \n", section);
    perror("");
    exit(1);
}

char *get_macro(uint32_t code)
{
    char *macro;

    switch (code) {
        case LC_REQ_DYLD:        macro = "LC_REQ_DYLD";       break;
        case LC_SEGMENT:      macro = "LC_SEGMENT";       break;
        case LC_SYMTAB:      macro = "\033[38;5;220mLC_SYMTAB\033[0m";       break;
        case LC_SYMSEG:      macro = "LC_SYMSEG";       break;
        case LC_THREAD:      macro = "LC_THREAD";       break;
        case LC_UNIXTHREAD:      macro = "LC_UNIXTHREAD";       break;
        case LC_LOADFVMLIB:      macro = "LC_LOADFVMLIB";       break;
        case LC_IDFVMLIB:      macro = "LC_IDFVMLIB";       break;
        case LC_IDENT:      macro = "LC_IDENT";       break;
        case LC_FVMFILE:      macro = "LC_FVMFILE";       break;
        case LC_PREPAGE:      macro = "LC_PREPAGE";       break;
        case LC_DYSYMTAB:      macro = "LC_DYSYMTAB";       break;
        case LC_LOAD_DYLIB:      macro = "LC_LOAD_DYLIB";       break;
        case LC_ID_DYLIB:      macro = "LC_ID_DYLIB";       break;
        case LC_LOAD_DYLINKER:      macro = "LC_LOAD_DYLINKER";       break;
        case LC_ID_DYLINKER:      macro = "LC_ID_DYLINKER";       break;
        case LC_PREBOUND_DYLIB:      macro = "LC_PREBOUND_DYLIB";       break;
        case LC_ROUTINES:      macro = "LC_ROUTINES";       break;
        case LC_SUB_FRAMEWORK:      macro = "LC_SUB_FRAMEWORK";       break;
        case LC_SUB_UMBRELLA:      macro = "LC_SUB_UMBRELLA";       break;
        case LC_SUB_CLIENT:      macro = "LC_SUB_CLIENT";       break;
        case LC_SUB_LIBRARY:      macro = "LC_SUB_LIBRARY";       break;
        case LC_TWOLEVEL_HINTS:      macro = "LC_TWOLEVEL_HINTS";       break;
        case LC_PREBIND_CKSUM:      macro = "LC_PREBIND_CKSUM";       break;
        case LC_LOAD_WEAK_DYLIB:      macro = "LC_LOAD_WEAK_DYLIB";       break;
        case LC_SEGMENT_64:             macro = "\033[38;5;40mLC_SEGMENT_64\033[0m";       break;
        case LC_ROUTINES_64:            macro = "LC_ROUTINES_64";       break;
        case LC_UUID:                    macro =  "LC_UUID";       break;
        case LC_RPATH:                  macro = "LC_RPATH";       break;
        case LC_CODE_SIGNATURE:         macro = "LC_CODE_SIGNATURE";       break;
        case LC_SEGMENT_SPLIT_INFO:      macro = "LC_SEGMENT_SPLIT_INFO";       break;
        case LC_REEXPORT_DYLIB:      macro = "LC_REEXPORT_DYLIB";       break;
        case LC_LAZY_LOAD_DYLIB:      macro = "LC_LAZY_LOAD_DYLIB";       break;
        case LC_ENCRYPTION_INFO:      macro = "LC_ENCRYPTION_INFO";       break;
        case LC_DYLD_INFO:      macro = "LC_DYLD_INFO";       break;
        case LC_DYLD_INFO_ONLY:      macro = "LC_DYLD_INFO_ONLY";       break;
        case LC_LOAD_UPWARD_DYLIB:      macro = "LC_LOAD_UPWARD_DYLIB";       break;
        case LC_VERSION_MIN_MACOSX:      macro = "LC_VERSION_MIN_MACOSX";       break;
        case LC_VERSION_MIN_IPHONEOS:      macro = "LC_VERSION_MIN_IPHONEOS";       break;
        case LC_FUNCTION_STARTS:      macro = "LC_FUNCTION_STARTS";       break;
        case LC_DYLD_ENVIRONMENT:      macro = "LC_DYLD_ENVIRONMENT";       break;
        case LC_MAIN:      macro = "LC_MAIN";       break;
        case LC_DATA_IN_CODE:      macro = "LC_DATA_IN_CODE";       break;
        case LC_SOURCE_VERSION:      macro = "LC_SOURCE_VERSION";       break;
        case LC_DYLIB_CODE_SIGN_DRS:      macro = "LC_DYLIB_CODE_SIGN_DRS";       break;
        case LC_ENCRYPTION_INFO_64:      macro = "LC_ENCRYPTION_INFO_64";       break;
        case LC_LINKER_OPTION:      macro = "LC_LINKER_OPTION";       break;
        case LC_LINKER_OPTIMIZATION_HINT:      macro = "LC_LINKER_OPTIMIZATION_HINT";       break;
        case LC_VERSION_MIN_TVOS:      macro = "LC_VERSION_MIN_TVOS";       break;
        case LC_VERSION_MIN_WATCHOS:      macro = "LC_VERSION_MIN_WATCHOS";       break;
        case LC_NOTE:      macro = "LC_NOTE";       break;
        case LC_BUILD_VERSION:      macro = "LC_BUILD_VERSION";       break;
        case LC_DYLD_EXPORTS_TRIE:      macro = "LC_DYLD_EXPORTS_TRIE";       break;
        case LC_DYLD_CHAINED_FIXUPS:      macro = "LC_DYLD_CHAINED_FIXUPS";       break;
    }
    return (macro);
}

void process_dyld_info(void *load)
{
    struct dyld_info_command *cmd = (struct dyld_info_command *)load;
    printf("###################################\n");
    printf("cmd->rebase_off = %d\n", cmd->rebase_off);
    printf("cmd->rebase_size = %d\n", cmd->rebase_size);
    printf("cmd->bind_off = %d\n", cmd->bind_off);
    printf("cmd->bind_size = %d\n", cmd->bind_size);
    printf("cmd->lazy_bind_off = %d\n", cmd->lazy_bind_off);
    printf("cmd->lazy_bind_size = %d\n", cmd->lazy_bind_size);
    printf("cmd->export_off = %d\n", cmd->export_off);
    printf("cmd->export_size = %d\n", cmd->export_size);
    printf("###################################\n");
}

void process_segment(void *load)
{
    struct segment_command_64 *segment = (struct segment_command_64 *)load;
    printf("###################################\n");
    printf("segment->segname = \033[38;5;39m%s\033[0m\n", segment->segname);
    printf("segment->vmaddr = %llu\n", segment->vmaddr);
    printf("segment->vmsize = %llu\n", segment->vmsize);
    printf("segment->fileoff = %llu\n", segment->fileoff);
    printf("segment->filesize = %llu\n", segment->filesize);
    printf("segment->maxprot = %d\n", segment->maxprot);
    printf("segment->initprot = %d\n", segment->initprot);
    printf("segment->nsects = %u\n", segment->nsects);
    printf("segment->flags = %u\n", segment->flags);
    printf("###################################\n");

    int offset = sizeof(struct segment_command_64);
    // struct section_64 *section = (struct section_64*)(segment + sizeof(struct segment_command_64));
    struct section_64 section;
    bzero(&section, sizeof(section));
    memcpy(&section, &load[offset], sizeof(section));

    for (int count = 0; count < segment->nsects; count++)
    {
        // section = (struct section_64 *)(&load[offset]);
        printf("\033[38;5;99m+++++++++++++++++++++++++++++++\033[0m\n");
        printf("section->sectname = \033[38;5;220m%s\033[0m\n", section.sectname);
        printf("section->segname = \033[38;5;39m%s\033[0m\n", section.segname);
        printf("section->addr = %llu\n", section.addr);
        printf("section->size = %llu\n", section.size);
        printf("section->offset = %d\n", section.offset);
        printf("section->align = %d\n", section.align);
        printf("section->reloff = %d\n", section.reloff);
        printf("section->nreloc = %d\n", section.nreloc);
        printf("section->flags = %u\n", section.flags);
        printf("section->reserved1 = %d\n", section.reserved1);
        printf("section->reserved2 = %d\n", section.reserved2);
        printf("section->reserved3 = %d\n", section.reserved3);
        printf("\033[38;5;99m+++++++++++++++++++++++++++++++\033[0m\n");
        offset += sizeof(struct section_64);
        bzero(&section, sizeof(section));
        memcpy(&section, &load[offset], sizeof(section));


    }

}

void process_symtab(void *load, void *file)
{
    struct symtab_command *symtab = (struct symtab_command *)load;
    printf("###################################\n");
    printf("symtab->symoff = %u\n", symtab->symoff);
    printf("symtab->nsyms = %u\n", symtab->nsyms);
    printf("symtab->stroff = %u\n", symtab->stroff);
    printf("symtab->strsize = %u\n", symtab->strsize);
    printf("###################################\n");

    int offset = 0;
    struct nlist_64 symbol;
    bzero(&symbol, sizeof(symbol));
    memcpy(&symbol, &file[symtab->symoff + offset], sizeof(symbol));

    for (int count = 0; count < symtab->nsyms; count++)
    {
        // section = (struct section_64 *)(&load[offset]);
        printf("\033[38;5;99m++++++++++++     offset = %d      +++++++++++++++++++\033[0m\n", offset);
        // printf("symbol->n_name = %s\n", symbol.n_name);
        printf("symbol->n_strx = %u\n", symbol.n_un.n_strx);
        if (symbol.n_un.n_strx) {
            char * name = &file[symtab->stroff + symbol.n_un.n_strx];
            printf("name = %s\n", name); 
        }
        printf("symbol->n_type = 0x%x\n", symbol.n_type);
        printf("symbol->n_sect = %u\n", symbol.n_sect);
        printf("symbol->n_desc = %u\n", symbol.n_desc);
        printf("symbol->n_value = %llu\n", symbol.n_value);

        printf("\033[38;5;99m+++++++++++++++++++++++++++++++\033[0m\n");
        offset += sizeof(struct nlist_64);
        bzero(&symbol, sizeof(symbol));
        memcpy(&symbol, &file[symtab->symoff + offset], sizeof(symbol));


    }
}

int main()
{
    int fd;
    struct stat sb;
    void *file;

    if ((fd = open("macos_exec", O_RDONLY)) == -1)
        error_exit("OPEN");

    if (fstat(fd, &sb) == -1)
        error_exit("FSTAT");

   switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:  printf("block device\n");            break;
    case S_IFCHR:  printf("character device\n");        break;
    case S_IFDIR:  printf("directory\n");               break;
    case S_IFIFO:  printf("FIFO/pipe\n");               break;
    case S_IFLNK:  printf("symlink\n");                 break;
    case S_IFREG:  printf("regular file\n");            break;
    case S_IFSOCK: printf("socket\n");                  break;
    default:       printf("unknown?\n");                break;
    }

    if ((file = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        error_exit("MMAP");

    struct mach_header_64 *header = (struct mach_header_64 *)file;
    printf("header->magic = 0x%x\n", header->magic);
    switch (header->magic) {
        case MH_MAGIC:      printf("MH_MAGIC\n");       break;
        case MH_CIGAM:      printf("MH_CIGAM\n");       break;
        case MH_MAGIC_64:   printf("MH_MAGIC_64\n");    break;
        case MH_CIGAM_64:   printf("MH_CIGAM_64\n");    break;
        default:            printf("unknown?\n");       break;
    }

    printf("header->magic type: \n");
    printf("header->cputype = %d\n", header->cputype);
    printf("header->cpusubtype = %d\n", header->cpusubtype);
    printf("header->filetype = %d\n", header->filetype);
    printf("header->ncmds = %d\n", header->ncmds);
    printf("header->sizeofcmds = %d\n", header->sizeofcmds);
    printf("header->flags = %d\n", header->flags);

    struct load_command *load;
    printf("\n===============================\n\n");
    size_t size = sizeof(struct mach_header_64);
    printf("SIZEOF HEADER = %lu\n", size);
    int offset = size;
    for (int count = 0; count < header->ncmds; count++)
    {
        printf("\n\033[38;5;88m--------------------------\033[0m\n");
        printf("OFFSET = %d\n", offset);
        load = (struct load_command *)&file[offset];
        printf("load->cmd = 0x%x // %s\n", load->cmd, get_macro(load->cmd));
        printf("load->cmdsize = %d\n", load->cmdsize);
        if (load->cmd == LC_DYLD_INFO_ONLY)
            process_dyld_info(load);
        else if (load->cmd == LC_SEGMENT_64)
            process_segment(load);
        else if (load->cmd == LC_SYMTAB)
            process_symtab(load, file);
        offset += load->cmdsize;
        printf("\033[38;5;88m--------------------------\033[0m\n");

    }

    return (0);
}