#include "../../incs/nm_otool.h"

/*
** Init variables
*/
void init(t_env *env)
{
    bzero(env, sizeof(t_env));
    env->info.nsect = 1;
    env->file.fd = -1;
    env->info.prog = NM;
}

/*
** Parse arguments
** Allocate an array of char pointer
** if no args then a.out is use by default
*/
void parseArgs(int argc, char **argv, t_env *env)
{
    uint16_t    nargs, pos;

    pos = 0;
    nargs = (argc == 1) ? argc : (argc - 1);
    if (!(env->target.name = (char **)malloc(sizeof(char *) * nargs)))
        errorExit(env, "Target memory allocation\n");
    bzero(&env->target.name[0], sizeof(char *) * nargs);

    if (argc == 1) {
        env->target.name[0] = argv[0];
        return ;
    }
    for (int count = 1; count < argc; count++) {
        if (argv[count] && argv[count][0] != '-') {
            env->target.name[pos] = argv[count];
            pos++;
        }
        else if (argv[count] && argv[count][0] == '-')
            errorExit(env, "This program does not accept options");
    }
}

/*
** Main function for nm program
** Init variables
** Parse arguments
** For every file given in argument
** -- Open and get map of file
** -- If FAT binary
** --- For every subfile
** ---- Process library or executable
** -- If usual binary
** --- Process library or executable
** -- Set variables for next file
** Clear all resources
*/
int main(int argc, char **argv)
{
    void    *file;
    t_env   env;

    init(&env);
    parseArgs(argc, argv, &env);
    for (env.target.id = 0; env.target.id < argc - 1; env.target.id++) {
        file = getMap(&env, env.target.name[env.target.id]);
        if (isFatBinary(&env, file)) {
            for (uint32_t count = 0; count < env.fathdr.n_arch; count++) {
                (env.fathdr.arch == ARCH_32) ? getSubFile32(&env, file, count) : getSubFile64(&env, file, count);
                if (isLibrary(env.fathdr.subfile))
                    processLib(&env, env.fathdr.subfile, SUB_FILE);
                else
                    processSymbol(&env, env.fathdr.subfile, NULL);
                setNextSubFile(&env);
            }
        }
        else {
            if (isLibrary(file))
                processLib(&env, file, MAIN_FILE);
            else
                processSymbol(&env, file, NULL);
        }
        setNextFile(&env);
    }
    clearAll(&env);
    return (0);
}