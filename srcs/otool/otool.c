#include "../../incs/nm_otool.h"

/*
** Init variables
*/
void init(t_env *env)
{
    bzero(env, sizeof(t_env));
    env->info.nsect = 1;
    env->file.fd = -1;
    env->info.prog = OTOOL;
}

/*
** Parse arguments
** Allocate an array of char pointer
** if no args then error exit
*/
void parseArgs(t_env *env, int argc, char **argv)
{
    uint16_t    pos;

    pos = 0;
    if (argc == 1)
        errorExit(env, "at least one file must be specified");
    if (!(env->target.name = (char **)malloc(sizeof(char *) * (argc - 1))))
        errorExit(env, "Target memory allocation\n");
    bzero(&env->target.name[0], sizeof(char *) * (argc - 1));

    if (argc == 1) {
        env->target.name[0] = argv[0];
        return ;
    }
    for (int32_t count = 1; count < argc; count++) {
        if (argv[count] && argv[count][0] != '-') {
            env->target.name[pos] = argv[count];
            pos++;
        }
        else if (argv[count] && argv[count][0] == '-')
            errorExit(env, "This program does not accept options");
    }
}

/*
** Main function for otool program
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
    parseArgs(&env, argc, argv);
    for (env.target.id = 0; env.target.id < (uint32_t)(argc - 1); env.target.id++) {
        file = getMap(&env, env.target.name[env.target.id]);
        if (isFatBinary(&env, file)) {
            for (uint32_t count = 0; count < env.fathdr.n_arch; count++) {
                (env.fathdr.arch == ARCH_32) ? getSubFile32(&env, file, count) : getSubFile64(&env, file, count);
                if (isLibrary(env.fathdr.subfile))
                    processLib(&env, env.fathdr.subfile, SUB_FILE);
                else
                    processText(&env, env.fathdr.subfile, NULL);
                setNextSubFile(&env);
            }
        }
        else {
            if (isLibrary(file))
                processLib(&env, file, MAIN_FILE);
            else
                processText(&env, file, NULL);
        }
        setNextFile(&env);
    }
    clearAll(&env);
    return (0);
}