#pragma once

#include "command.h"

#define NUM_OF_CMDS 4
extern COMMAND *global_command;
extern REMOTE *global_remote;
extern PLAN *global_plan;
extern CMD_INFO **cmd_info_array;
extern int fork_flag;
extern int script_flag;
extern HOST_LIST *global_host_list;
