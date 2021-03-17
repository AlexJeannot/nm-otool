#include "../../incs/nm_otool.h"

/*
** Verify if file is a library
*/
int8_t isLibrary(void *file)
{
    char    *signature;

    signature = file;
    if (strncmp(signature, ARMAG, 8) == 0)
        return (TRUE);
    return (FALSE);
}

/*
** Setup variables for next library object processing
*/
void setNextObj(t_env *env)
{
    env->info.arch = 0;
    env->info.nsect = 1;
    (isNm(env)) ? clearSymbol(env) : clearTextSect(env);
    clearSection(env);
}

/*
** Get library object size
** ASCII in file -> need to atoi() size field
*/
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

/*
** Get library object name size
** Need to increment the offset with this size field to find first byte address of library object
** Get superior multiple of string lenth and add 4
*/
uint32_t getNameSize(char *name)
{
    int16_t factor = 0;

    factor = strlen(name) / 8;
    if ((strlen(name) % 8) != 0)
        factor += 1;
    return ((factor * 8) + 4);
}

/*
** Add elem to library object linked list
*/
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

/*
** Create library object linked list
** Offset start at 8 (size of signature)
** Size if file/subfile size depending if USUAL/FAT binary
** Offset is incremented by size of library header and size of other information to get to first object header
** While we do not overflow file/subfile size
** -- Increment offset by header size (and control overflow)
** -- Allocate memory for linked list elem
** -- Set first byte object adress by adding name size
** -- Set object name
** -- Add element in linked list
** -- Increment offset by object size to get to next object header
*/
void getLibObjList(t_env *env, void *file, int8_t file_type)
{
    struct ar_hdr   *lib_header;
    struct ar_hdr   *obj_header;
    t_lib_obj       *new_obj;
    uint64_t        size;
    uint32_t        offset;

    offset = 8;
    size = (file_type == MAIN_FILE) ? env->file.size : env->fathdr.subsize;
    lib_header = (struct ar_hdr *)&file[offset];
    offset += (sizeof(struct ar_hdr) + getObjSize(env, lib_header));

    while (controlOverflow(size, offset)) {
        obj_header = (struct ar_hdr *)&file[offset];
        offset += sizeof(struct ar_hdr);
        if (!(controlOverflow(size, offset)))
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

/*
** Process library
** Create a linked list with all library object informations
** While object
** -- If program is nm then process symbol
** -- If program is otool process __TEXT, __text setion
** -- Set value for next object
** Clear library object linked list
*/
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