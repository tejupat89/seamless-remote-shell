#pragma once

#include "command.h"

char **word_list_to_argv (WORD_LIST *);
int word_list_length(WORD_LIST *);
int file_list_length(FILE_LIST *);
char **file_list_to_argv (FILE_LIST *);
char **word_file_to_argv (WORD_LIST *, FILE_LIST *);
void com_string_to_argv(char *, char **);
char *extract_filename (char *);
char *extract_directory(char *, char *);
char *simple_command_to_remote(SIMPLE_COM *);
char *pipe_command_to_remote(COMMAND *, int);
int word_list_len(WORD_LIST *);
int file_list_len(FILE_LIST *);
int redirects_len(REDIRECT *);
int num_of_commands(COMMAND *);
COMMAND *getCommand(COMMAND *, int);
REDIRECT *getFinalOutputRedirection(COMMAND *command);
void load_cmd_info_array(CMD_INFO** , int);
void fill_host_list(COMMAND *);
void fill_host_list_simple(SIMPLE_COM *);
void fill_host_list_pipe(COMMAND *);
int notPresent(char **, int, char *);
void fill_files_with_host_nums(COMMAND *command);
void fill_files_with_host_nums_simple(SIMPLE_COM*);
void fill_files_with_host_nums_pipe(PIPE_COM *);
void fill_commands_with_cost_array(COMMAND *command);
void pre_processing(COMMAND *command);
char *simple_command_to_remote_in_pipe(COMMAND *command, int index);
