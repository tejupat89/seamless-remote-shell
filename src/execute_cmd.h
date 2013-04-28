#pragma once

#include "command.h"

void execute_command(COMMAND *command);
void execute_command_locally(COMMAND *);
void execute_command_remotely(COMMAND *);
void execute_simple_command(COMMAND *);
void execute_simple_command_locally(SIMPLE_COM *);
void execute_simple_command_remotely(COMMAND *, int);
void execute_pipe_command_remotely(COMMAND *, int, int);
void execute_pipe_command(COMMAND *);
void exec_execve(char **);
void do_redirections(REDIRECT *);
void execute_scp(COMMAND *);
void execute_scp_simple(COMMAND *, int);
void execute_scp_files(FILE_LIST *, COMMAND *, int);
void execute_scp_redirects_input(REDIRECT *, COMMAND *, int);
void execute_scp_redirects_output(REDIRECT *, COMMAND *, int);
void execute_scp_after(COMMAND *command);
int find_num_of_execution_grps();
void execute_grps(EXECUTION_GRP *, int);
void execute_single_grp(EXECUTION_GRP *);
void execute_single_grp_local(EXECUTION_GRP*);
void execute_single_grp_remote(EXECUTION_GRP*);
void create_hidden_directories(HOST_LIST *);

