#pragma once

#include "command.h"

void cost_calculate(COMMAND *command);
void cost_simple_command(COMMAND *command);
long get_input_cost(COMMAND *command, int host);
long get_redir_cost(COMMAND *command, int host);
long get_exec_cost(COMMAND* command, int host);
long decide_output(COMMAND *command);
void cost_calculate_simple(COMMAND *command);
void cost_calculate_pipe(COMMAND *command);
void calculate_piped_input(COMMAND *command);
int NotRedirectionOutput(COMMAND *command);
void calculate_output_from_piped_input(COMMAND *command);

