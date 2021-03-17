#include "../../incs/nm_otool.h"

/*
** Process __TEXT, __text section
*/
void processText(t_env *env, void *addr, char *obj_name)
{
    parseHeader(env, addr);
    parseLoadCommands(env, addr);
    displayHexText(env, addr, obj_name);
}