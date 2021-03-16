#ifndef NM_OTOOL
# define NM_OTOOL

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
# include <ar.h>
# include <mach-o/stab.h>

# define ARCH_32 32
# define ARCH_64 64
# define MAIN_FILE 1
# define SUB_FILE 2
# define NM 1
# define OTOOL 2
# define TRUE 1
# define FALSE 0

typedef struct s_target
{
    char        **name;
    uint32_t    id;
} t_target;

typedef struct s_file
{
    char        *name;
    uint64_t    size;
    uint64_t    subsize;
} t_file;

typedef struct s_fathdr_info
{
    void        *subfile;
    uint32_t    n_arch;
    int8_t      arch;
    int8_t      s_bytes;
} t_fathdr_info;

typedef struct s_lib_obj
{
    void                *addr;
    char                *name;
    struct s_lib_obj    *next;
} t_lib_obj;

typedef struct s_section
{
    char                sectname[16];
    char                segname[16];
    struct s_section    *next;
    uint16_t            id;
} t_section;

typedef struct s_symbol
{
    uint64_t            addr;
    char                *name;
    struct s_symbol     *next;
    char                type;
} t_symbol;

typedef struct s_env {
    int8_t                  prog;
    
    struct mach_header_64   *header;
    int8_t                  arch;
    int8_t                  s_bytes;
    uint16_t                nb_sect;

    t_target                target;
    t_file                  file;
    t_fathdr_info           fathdr;
    t_lib_obj               *lib_objs;
    t_section               *section_list;
    union {
        union {
            struct section          *text_32;
            struct section_64       *text_64;
        } section;
        struct {
            struct symtab_command   *table;
            t_symbol                *list;
        } symbol;
    } data;
} t_env;

/*
** CLEAR.C
*/
void        clearLib(t_env *env);
void        clearSection(t_env *env);
void        clearSymbol(t_env *env);
void        clearAll(t_env *env);
void        deleteSymbol(t_env *env, t_symbol *symbol);

/*
** CONTROL.C
*/
int8_t      isNm(const t_env *env);
int8_t      isOtool(const t_env *env);
int8_t      isTypeDef(char c);
int8_t      controlOverflow(uint64_t size, uint64_t offset);

/*
** DISPLAY.C
*/
void        displaySymbols(const t_env *env, char *obj_name);
void        sortSymbolList(t_env *env);

/*
** EXIT.C
*/
void        errorExit(t_env *env, char *error, char *exec);

/*
** FAT.C
*/
uint32_t    isFatBinary(t_env *env, void *file);
void        getSubFile32(t_env *env, void *file, uint32_t arch_pos);
void        getSubFile64(t_env *env, void *file, uint32_t arch_pos);

/*
** LIB.C
*/
int8_t      isLibrary(void *file);
void        setNextObj(t_env *env);
void        getLibObjList(t_env *env, void *file, int8_t file_type);
void        processLib(t_env *env, void *file, int8_t file_type);

/*
** PARSE.C
*/
void        parseHeader(t_env *env, void *file);
void        parseLoadCommands(t_env *env, void *file);

/*
** RESOURCES.C
*/
void        setNextFile(t_env *env);
void        setNextSubFile(t_env *env);
void        *getMap(t_env *env, char *input);

/*
** SEGMENT.C
*/
void        addSectionList(t_env *env, t_section *new_section);
void        parseSegment32(t_env *env, void *l_cmd, uint64_t g_offset);
void        parseSegment64(t_env *env, void* l_cmd, uint64_t g_offset);

/*
** SWAP.C
*/
uint16_t    ifSwapuInt16(int8_t swap, uint16_t val);
int16_t     ifSwapInt16(int8_t swap, int16_t val);
uint32_t    ifSwapuInt32(int8_t swap, uint32_t val);
int32_t     ifSwapInt32(int8_t swap, int32_t val);
uint64_t    ifSwapuInt64(int8_t swap, uint64_t val);
int64_t     ifSwapInt64(int8_t swap, int64_t val);

/*
** SYMBOL.C
*/
void        addSymbolList(t_env *env, t_symbol *new_symbol);
void        getSymbols32(t_env *env, void *file, struct symtab_command *sym_cmd);
void        getSymbols64(t_env *env, void *file, struct symtab_command *sym_cmd);
void        parseSymtab(t_env *env, void *file, struct load_command *l_cmd);
void        processSymbol(t_env *env, void *addr, char *obj_name);

#endif