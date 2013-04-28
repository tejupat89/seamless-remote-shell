#pragma once

#include "command.h"

void print_word(WORD_DESC *);
void print_word_list(WORD_LIST *);
void print_file(FILEE *);
void print_file_list(FILE_LIST *);
void print_remote_file(REMOTE_FILE *);
void print_simple_command(SIMPLE_COM *);
void print_command(COMMAND *);
void print_redirection(REDIRECT *);
void print_pipe_command(PIPE_COM *);
void print_remote(REMOTE *);
void print_cost(COST *);
void print_long_array(long**);
void print_plan(PLAN *plan);
void print_execution_grp(EXECUTION_GRP *);
void print_cmd_info_array(CMD_INFO** , int);
void print_cmd_info(CMD_INFO *);
void print_host_list(HOST_LIST *host_list);
void print_host(HOST *host);
void print_exec_cost(EXEC_COST *);


