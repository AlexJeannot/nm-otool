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

    if (!(env->target.name = (char **)malloc(sizeof(char *) * (argc - 1))))
        errorExit("Target memory allocation\n", NULL);
    bzero(&env->target.name[0], sizeof(char *) * (argc - 1));
    for (int count = 1; count < argc; count++) {
        if (argv[count] && argv[count][0] != '-') {
            env->target.name[pos] = argv[count];
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

void clearSymbol(t_env *env)
{
    t_symbol *prev, *tmp;

    tmp = env->data.symbol.list;
    while (tmp) {
        prev = tmp;
        tmp = tmp->next;
        free(prev);
    }
    env->data.symbol.list = NULL;
}

void setNextFile(t_env *env)
{
    clearSection(env);
    clearSymbol(env);
    bzero(&env->file, sizeof(env->file));
    bzero(&env->data, sizeof(env->data));
    env->arch = 0;
    env->s_bytes = 0;
    env->nb_sect = 1;

}

void processLib(t_env *env, void *file, int8_t file_type)
{
    t_lib_obj *obj;
    
    getLibObjList(env, file, file_type);
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
    env.nb_sect = 1;
    for (env.target.id = 0; env.target.id < argc - 1; env.target.id++) {
        file = getMap(&env, env.target.name[env.target.id]);
        if (isFatBinary(&env, file)) {
            for (uint32_t count = 0; count < env.fathdr.n_arch; count++) {
                (env.fathdr.arch == ARCH_32) ? getSubFile32(&env, file, count) : getSubFile64(&env, file, count);
                if (isLibrary(env.fathdr.subfile))
                    processLib(&env, env.fathdr.subfile, SUB_FILE);
                else
                    processSymbol(&env, env.fathdr.subfile);
                env.nb_sect = 1;
            }
        }
        else {
            if (isLibrary(file))
                processLib(&env, file, MAIN_FILE);
            else
                processSymbol(&env, file);
        }
        setNextFile(&env);
    }
    return (0);
}