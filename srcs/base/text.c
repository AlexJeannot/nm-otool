#include "../../incs/nm_otool.h"

void processText(t_env *env, void *addr, char *obj_name)
{
    parseHeader(env, addr);
    parseLoadCommands(env, addr);
    displayHexText(env, addr, obj_name);
}