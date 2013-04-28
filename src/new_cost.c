#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include "new_cost.h"
#include "shared.h"
#include "debug.h"
#include "copy_cmd.h"
#include "print_cmd.h"
#include "convert.h"
#include "dispose_cmd.h"

void cost_calculate(COMMAND *command)
{
	if(command==NULL)
		return;
	switch(command->type)
	{
	  case cm_simple:
	  {
		cost_calculate_simple(command);
		break;
	  }
	  case cm_pipe:
	  {
		cost_calculate_pipe(command);
		break;
	  }
	}
}

void cost_calculate_simple(COMMAND *command)
{
	for(int i =0; i<global_host_list->num_of_hosts + 1; i++)
	{
		*(command->cost->input[i]) = get_input_cost(command, i);
	}
	
	command->cost->output = decide_output(command);
	
	for(int i =0; i<global_host_list->num_of_hosts + 1; i++)
	{
		*(command->cost->redir_output[i]) = get_redir_cost(command, i);
	} 

	for(int i =0; i<global_host_list->num_of_hosts + 1; i++)
	{
		*(command->cost->exec_cost[i]) = get_exec_cost(command, i);
	} 	
	
}

long get_input_cost(COMMAND *command, int host)
{
	long cost=0;
	FILE_LIST *files = command->value.Simple->files;
	REDIRECT *redirects = command->value.Simple->redirects;
	while(files)
	{
		if(files->file->host == host)
			cost += files->file->size;
		files = files->next;
	}
	while(redirects)
	{
		if(redirects->redirector == 0 && redirects->redirectee.file->host == host)
			cost += redirects->redirectee.file->size;
		redirects = redirects->next;
	}
	return cost;
} 

long get_redir_cost(COMMAND *command, int host)
{	
	long cost=0;
	REDIRECT *redirects = command->value.Simple->redirects;
	while(redirects)
	{
		if(redirects->redirector==1 && redirects->redirectee.file->host == host){
			cost = command->cost->output;
			break;
		}
		redirects = redirects->next;
	}
	return cost;
}

long get_exec_cost(COMMAND *command, int host)
{
	long cost = 0;
	for(int i=0; i<global_host_list->num_of_hosts+1; i++)
	{
		if(i!=host)
			cost += *(command->cost->input[i]); 
	}
	return cost;
}

long decide_output(COMMAND *command)
{
	long cost= 0;

	for(int i =0; i<global_host_list->num_of_hosts+1; i++)
		cost += *(command->cost->input[i]);

	for(int i=0; i<NUM_OF_CMDS; i++)
	{
		if(strcmp(command->value.Simple->words->word->word, cmd_info_array[i]->cmd)==0)
		{
			if(cmd_info_array[i]->output >1 )
				cost = cmd_info_array[i]->output;
			else
				cost *= cmd_info_array[i]->output ;

		}
	}

	return cost;
}

void cost_calculate_pipe(COMMAND *command)
{
	cost_calculate(command->value.Pipe->com);
	copy_cost(command->value.Pipe->com->cost, command->cost, global_host_list->num_of_hosts+1);
	cost_calculate(command->value.Pipe->next);
	if(command->value.Pipe->next){
		calculate_piped_input(command->value.Pipe->next);
		calculate_output_from_piped_input(command->value.Pipe->next);
	}
}

void calculate_piped_input(COMMAND *command)
{
	COMMAND *prev = command->value.Pipe->prev;
	if(NotRedirectionOutput(prev->value.Pipe->com))
		command->cost->piped_input = prev->cost->output; 
}

int NotRedirectionOutput(COMMAND *command)
{
	long cost = 0;
	for(int i =0; i<global_host_list->num_of_hosts+1; i++)
	{
		cost += *(command->cost->redir_output[i]);
	}
	if(cost==0)
		return 1;
	else
		return 0;
}

void calculate_output_from_piped_input(COMMAND *command)
{
	if(command->cost->output>0)
		return;
	for(int i=0; i<NUM_OF_CMDS; i++)
	{
		if(strcmp(command->value.Pipe->com->value.Simple->words->word->word, cmd_info_array[i]->cmd)==0){
			if(cmd_info_array[i]->output >1 )
				command->cost->output = cmd_info_array[i]->output;
			else
				command->cost->output = cmd_info_array[i]->output * command->cost->piped_input;
		}
	}
	
}


