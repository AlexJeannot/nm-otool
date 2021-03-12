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
# include <ar.h>

# define ARCH_32 32
# define ARCH_64 64
# define NM 1
# define OTOOL 2
# define TRUE 1
# define FALSE 0
# define SECTION_LIST 1
# define SYMBOL_LIST 2
# define MH_LIB 738142165265366049

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
    struct s_symbol *next;
} t_symbol;

typedef struct s_lib_obj
{
    void *addr;
    struct s_lib_obj *next;
} t_lib_obj;

typedef struct s_fathdr_info
{
    uint32_t    n_arch;
    int8_t arch;
    int8_t s_bytes;
    void *subfile;
} t_fathdr_info;

typedef struct s_file
{
    char *name;
    uint64_t size;
    uint64_t subsize;
} t_file;

typedef struct s_env {

    struct mach_header_64 *header;
    char **target;
    int8_t arch;
    int8_t s_bytes;
    int8_t prog;
    t_fathdr_info fathdr;
    union {
        union {
            struct section *text_32;
            struct section_64 *text_64;
        } section;
        struct {
            struct symtab_command *table;
            char *obj_name;
            t_symbol *list;
        } symbol;
    } data;
    t_section *section_list;
    t_lib_obj *lib_objs;
    t_file file;
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
int8_t  isTypeDef(char c);
int8_t controlOverflow(uint64_t size, uint64_t offset);

//Swap
uint16_t ifSwapuInt16(int8_t swap, uint16_t val);
int16_t ifSwapInt16(int8_t swap, int16_t val);
uint32_t ifSwapuInt32(int8_t swap, uint32_t val);
int32_t ifSwapInt32(int8_t swap, int32_t val);
uint64_t ifSwapuInt64(int8_t swap, uint64_t val);
int64_t ifSwapInt64(int8_t swap, int64_t val);


uint32_t isFatBinary(t_env *env, void *file);
void setNextFatSubfile(t_env *env);
void getSubFile32(t_env *env, void *file, uint32_t arch_pos);
void getSubFile64(t_env *env, void *file, uint32_t arch_pos);

int8_t isLibrary(void *file);
void getLibObjList(t_env *env, void *file);
// int32_t getObjSize(struct ar_hdr *header);

void clearLib(t_env *env);
void setNextObj(t_env *env);
#endif