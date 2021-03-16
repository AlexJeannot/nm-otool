#include "../../incs/nm_otool.h"

int8_t isLibrary(void *file)
{
    char    *signature;

    signature = file;
    if (strncmp(signature, ARMAG, 8) == 0)
        return (TRUE);
    return (FALSE);
}

void setNextObj(t_env *env)
{
    t_symbol_list *prev, *tmp;

    env->info.arch = 0;
    env->info.nsect = 1;
    (isNm(env)) ? clearSymbol(env) : clearTextSect(env);
    clearSection(env);
}

int32_t getObjSize(t_env *env, struct ar_hdr *header)
{
    char        hdr_size[11];
    int32_t     size = 0;

    bzero(&hdr_size, 11);
    strncpy(&hdr_size[0], &header->ar_size[0], 10);
    if ((size = atoi(hdr_size)) < 0)
        errorExit(env, "library info size < 0");
    return (size);
}

uint32_t getNameSize(char *name)
{
    int16_t factor = 0;

    factor = strlen(name) / 8;
    if ((strlen(name) % 8) != 0)
        factor += 1;
    return ((factor * 8) + 4);
}

void addLibObjList(t_env *env, t_lib_obj *new_obj)
{
    t_lib_obj *tmp;

    if (!(tmp = env->lib_objs))
        env->lib_objs = new_obj;
    else {
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_obj;
    }
}

void getLibObjList(t_env *env, void *file, int8_t file_type)
{
    struct ar_hdr   *lib_header;
    struct ar_hdr   *obj_header;
    t_lib_obj       *new_obj;
    uint64_t        size;
    uint32_t        offset;

    offset = 8;
    size = (file_type == MAIN_FILE) ? env->file.size : env->file.subsize;
    lib_header = (struct ar_hdr *)&file[offset];
    offset += (sizeof(struct ar_hdr) + getObjSize(env, lib_header));

    while (controlOverflow(size, offset)) {
        obj_header = (struct ar_hdr *)&file[offset];
        if (!(controlOverflow(size, offset += sizeof(struct ar_hdr))))
            break ;

        if (!(new_obj = (t_lib_obj *)malloc(sizeof(t_lib_obj))))
            errorExit(env, "Library object memory allocation");
        bzero(new_obj, sizeof(t_lib_obj));
        new_obj->addr = &file[offset + getNameSize(&file[offset])];
        new_obj->name = &file[offset];
        new_obj->next = NULL;
        addLibObjList(env, new_obj);

        offset += getObjSize(env, obj_header);
    }
}

void processLib(t_env *env, void *file, int8_t file_type)
{
    t_lib_obj *obj;
    
    getLibObjList(env, file, file_type);
    obj = env->lib_objs;
    while (obj) {
        (isNm(env)) ? processSymbol(env, obj->addr, obj->name) : processText(env, obj->addr, obj->name);
        setNextObj(env);
        obj = obj->next;
    }
    clearLib(env);
}