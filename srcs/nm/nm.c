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
    env->file.size = sb.st_size;
    return (file);
}

void processSymbol(t_env *env, void *addr)
{
    parseHeader(env, addr);
    parseLoadCommands(env, addr);
    sortSymbolList(env);
    displaySymbols(env);
}

void clearSection(t_env *env)
{
    t_section *prev, *tmp;

    tmp = env->section_list;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->section_list = NULL;
}

void setNextFile(t_env *env)
{
    clearSection(env);
    bzero(&env->file, sizeof(env->file));
    bzero(&env->data, sizeof(env->data));
    env->arch = 0;
    env->s_bytes = 0;
}

void processLib(t_env *env, void *file)
{
    t_lib_obj *obj;
    
    getLibObjList(env, file);
    obj = env->lib_objs;
    while (obj) {
        processSymbol(env, obj->addr);
        setNextObj(env);
        obj = obj->next;
    }
    clearLib(env);
}


int main(int argc, char **argv)
{
    void *file;
    t_env env;

    bzero(&env, sizeof(env));
    env.prog = NM;

    parse_args(argc, argv, &env);

    for (int nb_files = 0; nb_files < argc - 1; nb_files++) {
        file = getMap(&env, env.target[nb_files]);
        if (isFatBinary(&env, file)) {
            for (uint32_t count = 0; count < env.fathdr.n_arch; count++) {
                (env.fathdr.arch == ARCH_32) ? getSubFile32(&env, file, count) : getSubFile64(&env, file, count);
                if (isLibrary(env.fathdr.subfile))
                    processLib(&env, env.fathdr.subfile);
                else
                    processSymbol(&env, env.fathdr.subfile);
            }
        }
        else {
            if (isLibrary(file))
                processLib(&env, file);
            else
                processSymbol(&env, file);
        }
        setNextFile(&env);
    }
    return (0);
}