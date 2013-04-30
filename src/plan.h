#pragma once

#include "command.h"

void plan(COMMAND *);
void create_plans(COMMAND *command);
void create_plans_simple(COMMAND *command);
void create_plans_pipe(COMMAND *command);
void create_single_plan(COMMAND *, EXEC_COST **, int);
void compute_minimum(COMMAND *command, int receiver, EXEC_COST **plans, int cmd);
long send_output(COMMAND *command, int sender, int receiver);
int get_min_total_cost_index(EXEC_COST **plans);
int get_min_final_total_cost_index(EXEC_COST **plans);
void write_to_global_plan(EXEC_COST **plans);
void findExhaustivePatterns(COMMAND *command, int size, int hosts);
long compute_pattern_cost(int *pattern, int size, COMMAND *command);
void check_remote_threshold_grp_array(EXECUTION_GRP *array, int size);
int check_remote_threshold_simple(COMMAND *command);
int check_remote_threshold_grp(EXECUTION_GRP *grp);
