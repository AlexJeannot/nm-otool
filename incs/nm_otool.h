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

/*
** Structure of general informations
*/
typedef struct  s_info
{
    int8_t      prog;       // nm or otool
    int8_t      arch;       // 32 or 64 bits
    int8_t      s_bytes;    // need to swap endianness or not
    uint16_t    nsect;      // number of section (start at 1 and then incremented)
}               t_info;

/*
** Structure of targets (arguments)
*/
typedef struct  s_target
{
    char        **name;
    uint32_t    id;
    char        def_arg[6];
}               t_target;

/*
** Structure of file informations
*/
typedef struct s_file
{
    void        *addr;  // File first byte address
    char        *name;
    uint64_t    size;
    int32_t     fd;
}               t_file;

/*
** Structure of FAT header informations
*/
typedef struct  s_fathdr_info
{
    void        *subfile;   // Subfile first byte address
    uint64_t    subsize;
    uint32_t    n_arch;     // Number of architectures
    int8_t      arch;       // Type of architecture
    int8_t      s_bytes;    // need to swap endianness or not
}               t_fathdr_info;

/*
** Structure of linked list for library object
*/
typedef struct  s_lib_obj
{
    void                *addr;  // Object first byte address
    char                *name;  
    struct s_lib_obj    *next;
}               t_lib_obj;

/*
** Structure of linked list for section
*/
typedef struct  s_section
{
    char                sectname[16];
    char                segname[16];
    struct s_section    *next;
    uint16_t            id;
}               t_section;

/*
** Structure of linked list for symbol
*/
typedef struct  s_symbol_list
{
    uint64_t                addr;
    char                    *name;
    struct s_symbol_list    *next;
    char                    type;
}               t_symbol_list;

/*
** Structure of symbol data
*/
typedef struct  s_symbol
{
    struct symtab_command   *table;
    t_symbol_list           *list;
}               t_symbol;

/*
** Structure of __TEXT, __text section data
*/
typedef struct  s_text
{
    uint64_t    addr;
    uint64_t    size;
    uint32_t    offset;
}               t_text;

/*
** General structure
*/
typedef struct  s_env
{
    struct mach_header_64   *header;

    t_info                  info;
    t_target                target;
    t_file                  file;
    t_fathdr_info           fathdr;
    t_lib_obj               *lib_objs;
    t_section               *section_list;

    union {
        t_text              text;
        t_symbol            symbol;
    } data;
}               t_env;


/*
** CLEAR.C
*/
void        clearLib(t_env *env);
void        clearSection(t_env *env);
void        clearSymbol(t_env *env);
void        clearTextSect(t_env *env);
void        clearFile(t_env *env);
void        clearAll(t_env *env);
void        deleteSymbol(t_env *env, t_symbol_list *symbol);

/*
** CONTROL.C
*/
int8_t      isNm(const t_env *env);
int8_t      isOtool(const t_env *env);
int8_t      isArch32(const t_env *env);
int8_t      isTypeDef(char c);
int8_t      isTextSection(const char *section);
int8_t      controlOverflow(uint64_t size, uint64_t offset);

/*
** DISPLAY.C
*/
void        displayHexText(t_env *env, void *file, char *obj_name);
void        displaySymbols(const t_env *env, char *obj_name);
void        sortSymbolList(t_env *env);

/*
** EXIT.C
*/
void        errorExit(t_env *env, char *error);

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
void        addSymbolList(t_env *env, t_symbol_list *new_symbol);
void        getSymbols32(t_env *env, void *file, struct symtab_command *sym_cmd);
void        getSymbols64(t_env *env, void *file, struct symtab_command *sym_cmd);
void        parseSymtab(t_env *env, void *file, struct load_command *l_cmd);
void        processSymbol(t_env *env, void *addr, char *obj_name);

/*
** TEXT.C
*/
void        processText(t_env *env, void *addr, char *obj_name);

#endif