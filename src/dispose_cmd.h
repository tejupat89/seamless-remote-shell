#pragma once

#include "command.h"

void dispose_word (WORD_DESC *);
void dispose_word_list (WORD_LIST *);
void dispose_remote_file (REMOTE_FILE *remote_file);
void dispose_file (FILEE *file);
void dispose_file_list (FILE_LIST *);
void dispose_command (COMMAND *command);
void dispose_simple_command (SIMPLE_COM *simple);
void dispose_pipe_command (PIPE_COM *pipe);
void dispose_redirectee(REDIRECTEE );
void dispose_redirection (REDIRECT *);
void dispose_argv(char **);
void dispose_remote(REMOTE *);
void dispose_cost(COST *);
void dispose_plan(PLAN *);
void dispose_array(int **, int);
void dispose_cmd_info_array(CMD_INFO **, int);
void dispose_cmd_info(CMD_INFO *);
void dispose_host_list(HOST_LIST *);
void dispose_exec_cost(EXEC_COST *exec_cost);
void clean_up_hidden_directories(HOST_LIST *list);

