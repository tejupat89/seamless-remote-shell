#pragma once

#include "command.h"

WORD_DESC* make_bare_word (char *string);
WORD_DESC* make_word (char *);
WORD_LIST* make_word_list (WORD_DESC *, WORD_LIST *);
FILEE* make_file (WORD_DESC *);
FILEE* make_redir_file (WORD_DESC *);
FILE_LIST* make_file_list (FILEE *, FILE_LIST *);
REMOTE_FILE* make_remote_file (char *);
REMOTE_FILE *make_remote_file_given(char *, char *, char *);
ELEMENT* make_element ();
REDIRECT* make_redirection (int , enum r_instruction , REDIRECTEE );
REMOTE* make_remote ();
COST* make_cost();
PLAN* make_plan(int);
COMMAND *make_bare_simple_command ();
COMMAND* make_simple_command (ELEMENT, COMMAND*);
COMMAND* make_pipe_command (COMMAND*, COMMAND*);
COMMAND* make_bare_pipe_command (COMMAND*);
CMD_INFO** make_cmd_info_array(int);
HOST *make_host(int host_num, char *server);
HOST_LIST *make_host_list();
long** make_long_array();






