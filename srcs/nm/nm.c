#include "../../incs/nm.h"

void errorExit(char *error, char *exec)
{
    printf("nm: error: ");
    if (exec)
        printf("%s: ", exec);
    printf("%s\n", error);
    exit(1);
}

void parse_args(int argc, char **argv, t_env *env)
{
    int pos = 0;

    if (!(env->target = (char **)malloc(sizeof(char *) * (argc - 1))))
        errorExit("Target memory allocation\n", NULL);
    bzero(&env->target[0], sizeof(char *) * (argc - 1));
    for (int count = 1; count < argc; count++) {
        if (argv[count] && argv[count][0] != '-') {
            env->target[pos] = argv[count];
            pos++;
        }
    }
    // int test = 0;
    // while (env->target[test])
    // {
    //     printf("target[%d] = %s\n", test, env->target[test]);
    //     test++;
    // }
}

void *getMap(t_env *env, char *input)
{
    struct stat sb;
    void *file;
    int fd;

    if ((fd = open(input, O_RDONLY)) == -1)
        errorExit("No such file or directory", input);
    if (fstat(fd, &sb) == -1)
        errorExit("File access", input);
    if ((file = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        errorExit("Map access", input);
    env->file_size = sb.st_size;
    return (file);
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
    printf("name = %s\n", name);
    printf("(factor * 8) + 4 = %d\n", (factor * 8) + 4);
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

int8_t verifyOverflow(const t_env *env, uint64_t offset) {
    return (offset < env->file_size);
}

int verifyLibrary(t_env *env, void *file)
{
    char*       signature;
    uint32_t    offset = 8;
    struct ar_hdr    *lib_header;
    struct ar_hdr    *obj_header;
    t_lib_obj        *new_obj;

    signature = file;
    if (strncmp(signature, ARMAG, 8) == 0) {

        lib_header = (struct ar_hdr *)&file[offset];
        offset += (sizeof(struct ar_hdr) + getObjSize(lib_header));
        while (verifyOverflow(env, offset)) {
            obj_header = (struct ar_hdr *)&file[offset];
            offset += sizeof(struct ar_hdr); // ADD verify overflow ????????
            if (!(new_obj = (t_lib_obj *)malloc(sizeof(t_lib_obj))))
                errorExit("Library object memory allocation", NULL);
            bzero(new_obj, sizeof(t_lib_obj));
            new_obj->addr = &file[offset + getNameSize(&file[offset])];
            new_obj->next = NULL;
            addLibObjList(env, new_obj);
            offset += getObjSize(obj_header);
        }
        return (TRUE);
    }
    return (FALSE);
}

int main(int argc, char **argv)
{
    void *file;
    t_env env;

    bzero(&env, sizeof(env));
    env.prog = NM;

    parse_args(argc, argv, &env);

    file = getMap(&env, env.target[0]);

    if (verifyLibrary(&env, file))
    {
        printf("IS A LIB\n");
        t_lib_obj *tmp;

        tmp = env.lib_objs;
        while (tmp) {
            parseHeader(&env, tmp->addr);
            display_header(&env);

            parseLoadCommands(&env, tmp->addr);

            sortSymbolList(&env);
            displaySymbols(&env);
            tmp = tmp->next;
        }
    }
    else {
        parseHeader(&env, file);
        display_header(&env);

        parseLoadCommands(&env, file);

        sortSymbolList(&env);
        displaySymbols(&env);
    }

    return (0);    
}