#ifndef SHARED_LIBS
# define SHARED_LIBS

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/mman.h>
# include <mach-o/loader.h>
# include <mach-o/fat.h>
# include <mach-o/nlist.h>

# define ARCH_32 32
# define ARCH_64 64
# define R_BITS 1
# define NM 1
# define OTOOL 2
# define TRUE 1
# define FALSE 0
# define SECTION_LIST 1
# define SYMBOL_LIST 2

typedef struct s_section
{

    char        sectname[16];
    char        segname[16];
    uint16_t    id;
    struct s_section *next;
} t_section;

typedef struct s_symbol
{
    uint64_t        addr;
    char         type;
    char *          name;
    int8_t       displayed;
    struct s_symbol *next;
} t_symbol;

typedef struct s_env {

    struct mach_header_64 *header;
    char **target;
    int8_t arch;
    int8_t r_bits;
    int8_t prog;

    union {
        union {
            struct section *text_32;
            struct section_64 *text_64;
        } section;
        struct {
            struct symtab_command *table;
            t_symbol *list;
        } symbol;
    } data;
    t_section *section_list;

} t_env;

void parseHeader(t_env *env, void *file);
void display_header(const t_env * env);
void parseLoadCommands(t_env *env, void *file);
void errorExit(char *section, char *exec);
void display_section(const t_env * env);

void displaySymbols(const t_env *env);
void    sortSymbolList(t_env * env);
int8_t isNm(const t_env *env);
int8_t isOtool(const t_env *env);

#endif