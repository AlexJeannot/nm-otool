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
        errorExit("Target malloc\n", NULL);
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

void *getMap(char *input)
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
    return (file);
}

int main(int argc, char **argv)
{
    void *file;
    t_env env;

    bzero(&env, sizeof(env));
    env.prog = NM;

    parse_args(argc, argv, &env);

    file = getMap(env.target[0]);

    parseHeader(&env, file);
    display_header(&env);

    parseLoadCommands(&env, file);

    sortSymbolList(&env);
    displaySymbols(&env);

    return (0);    
}