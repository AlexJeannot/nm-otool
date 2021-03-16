#include "../../incs/nm_otool.h"

void addSectionList(t_env *env, t_section *new_section)
{
    t_section *tmp;

    if (!(tmp = env->section_list))
        env->section_list = new_section;
    else {
        while (tmp->next)
            tmp = tmp->next;
        tmp->next = new_section;
    }
}

void parseSegment32(t_env *env, void *l_cmd, uint64_t g_offset)
{
    struct segment_command  *segment;
    struct section          *section;
    t_section               *new_section;
    uint64_t                offset;
    uint32_t                nsects;

    offset = sizeof(struct segment_command);
    segment = (struct segment_command *)l_cmd;
    nsects = ifSwapuInt32(env->s_bytes, segment->nsects);

    for (int count = 0; count < nsects; count++) {
        if (!(controlOverflow(env->file.size, g_offset + offset)))
            errorExit(env, "Overflow on 32-bits segment parsing", env->target.name[env->target.id]);
        section = (struct section *)&(l_cmd[offset]);

        if (isNm(env)) {
            if (!(new_section = (t_section *)malloc(sizeof(t_section))))
                errorExit(env, "Section memory allocation", env->target.name[env->target.id]);
            bzero(new_section, sizeof(t_section));
            new_section->id = env->nb_sect++;
            strncpy(new_section->segname, section->segname, 15);
            strncpy(new_section->sectname, section->sectname, 15);
            new_section->next = NULL;
            addSectionList(env, new_section);
        }
        else if (strncmp(section->sectname, "__text", 6) == 0) {
            env->data.section.text_32 = section;
            break ;
        }
        offset += sizeof(struct section);
    }
}

void parseSegment64(t_env *env, void* l_cmd, uint64_t g_offset)
{
    struct segment_command_64   *segment;
    struct section_64           *section;
    t_section                   *new_section;
    uint64_t                    offset;
    uint32_t                    nsects;

    offset = sizeof(struct segment_command_64);
    segment = (struct segment_command_64 *)l_cmd;
    nsects = ifSwapuInt32(env->s_bytes, segment->nsects);

    for (int count = 0; count < nsects; count++) {
        if (!(controlOverflow(env->file.size, g_offset + offset)))
            errorExit(env, "Overflow on 64-bits segment parsing", env->target.name[env->target.id]);
        section = (struct section_64 *)&(l_cmd[offset]);

        if (isNm(env)) {
            if (!(new_section = (t_section *)malloc(sizeof(t_section))))
                errorExit(env, "Section memory allocation", env->target.name[env->target.id]);
            bzero(new_section, sizeof(t_section));
            new_section->id = env->nb_sect++;
            strncpy(new_section->segname, section->segname, 15);
            strncpy(new_section->sectname, section->sectname, 15);
            new_section->next = NULL;
            addSectionList(env, new_section);
        }
        else if (strncmp(section->sectname, "__text", 6) == 0) {
            env->data.section.text_64 = section;
            break ;
        }
        offset += sizeof(struct section_64);
    }
}