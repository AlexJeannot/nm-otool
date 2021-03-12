#include "../../incs/nm.h"

int8_t isLibrary(void *file)
{
    char    *signature;

    signature = file;
    if (strncmp(signature, ARMAG, 8) == 0)
        return (TRUE);
    return (FALSE);
}

void clearLib(t_env *env)
{
    t_lib_obj *prev, *tmp;

    tmp = env->lib_objs;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->lib_objs = NULL;
}

void setNextObj(t_env *env)
{
    t_symbol *prev, *tmp;

    env->arch = 0;
    tmp = env->data.symbol.list;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->data.symbol.list = NULL;
}

int32_t getObjSize(struct ar_hdr *header)
{
    char        hdr_size[11];
    int32_t     size = 0;

    bzero(&hdr_size, 11);
    strncpy(&hdr_size[0], &header->ar_size[0], 10);
    if ((size = atoi(hdr_size)) < 0)
        errorExit("library info size < 0", NULL);
    return (size);
}

uint32_t getNameSize(char *name)
{
    int16_t factor = 0;

    factor = strlen(name) / 8;
    if ((strlen(name) % 8) != 0)
        factor += 1;
    // printf("name = %s\n", name);
    // printf("(factor * 8) + 4 = %d\n", (factor * 8) + 4);
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

void getLibObjList(t_env *env, void *file)
{
    struct ar_hdr   *lib_header;
    struct ar_hdr   *obj_header;
    t_lib_obj       *new_obj;
    uint32_t        offset;

    offset = 8;
    lib_header = (struct ar_hdr *)&file[offset];
    offset += (sizeof(struct ar_hdr) + getObjSize(lib_header));
    while (controlOverflow(env->file.subsize, offset)) {
        obj_header = (struct ar_hdr *)&file[offset];
        if (!(controlOverflow(env->file.subsize, offset += sizeof(struct ar_hdr))))
            break ;
        if (!(new_obj = (t_lib_obj *)malloc(sizeof(t_lib_obj))))
            errorExit("Library object memory allocation", NULL);
        bzero(new_obj, sizeof(t_lib_obj));
        new_obj->addr = &file[offset + getNameSize(&file[offset])];
        new_obj->next = NULL;
        addLibObjList(env, new_obj);
        offset += getObjSize(obj_header);
    }
}