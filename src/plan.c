#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include "plan.h"
#include "new_cost.h"
#include "shared.h"
#include "debug.h"
#include "copy_cmd.h"
#include "print_cmd.h"
#include "convert.h"
#include "dispose_cmd.h"


void plan(COMMAND *command)
{
	switch(command->type)
	{
		case cm_simple:
			create_plans(command);
			break;

		case cm_pipe:
			create_plans(command);
			break;	
	}
}

void create_plans(COMMAND *command)
{
	switch(command->type)
	{
		case cm_simple:
			create_plans_simple(command);
			break;

		case cm_pipe:
			create_plans_pipe(command);
			break;
	}
}


void create_plans_simple(COMMAND *command)
{
	int size = global_plan->num_of_coms;
	int size_exec = global_host_list->num_of_hosts + 1;
	EXEC_COST *plans[size];
	for(int i=0; i<size; i++)
	{
		plans[i] = (EXEC_COST*)calloc(sizeof(EXEC_COST), sizeof(char));
		plans[i]->execution = (HOST_COST**)calloc(sizeof(HOST_COST*)*size_exec, sizeof(char));	
		for(int j =0; j<size_exec ; j++)
		{
			plans[i]->execution[j] = (HOST_COST *)calloc(sizeof(HOST_COST), sizeof(char));
			plans[i]->execution[j]->host = 0;
			plans[i]->execution[j]->cost = 0;
		}
	}

	create_single_plan(command, plans, 0);

	for(int i =0; i<size; i++)
		print_exec_cost(plans[i]);
	
	write_to_global_plan(plans);
}

void create_plans_pipe(COMMAND *command)
{
	COMMAND *command1 = command;
	PIPE_COM *pipe = command1->value.Pipe;
	int size = global_plan->num_of_coms;
	int size_exec = global_host_list->num_of_hosts + 1;
	EXEC_COST *plans[size];
	
	for(int i=0; i<size; i++)
	{
		plans[i] = (EXEC_COST*)calloc(sizeof(EXEC_COST), sizeof(char));
		plans[i]->execution = (HOST_COST**)calloc(sizeof(HOST_COST*)*size_exec, sizeof(char));	
		for(int j =0; j<size_exec ; j++)
		{
			plans[i]->execution[j] = (HOST_COST *)calloc(sizeof(HOST_COST), sizeof(char));
			plans[i]->execution[j]->host = 0;
			plans[i]->execution[j]->cost = 0;
		}
	}

	for(int i=0; i<size; i++)
	{
		create_single_plan(command1, plans, i);
		command1 = pipe->next;
		if(command1)
			pipe = command1->value.Pipe;
	}

	for(int i =0; i<size; i++)
		print_exec_cost(plans[i]);
	
	write_to_global_plan(plans);
}

void create_single_plan(COMMAND *command, EXEC_COST **plans, int index)
{
	int size = global_host_list->num_of_hosts+1;
	for(int r=0; r<size; r++)
	{
		compute_minimum(command, r, plans, index);
	}
}


void compute_minimum(COMMAND *command, int receiver, EXEC_COST **plans, int cmd)
{
	HOST_COST *host_cost = plans[cmd]->execution[receiver];
	long min_cost, cost;
	int min_host;
	for(int i=0; i<global_host_list->num_of_hosts+1; i++)
	{
		if(i==0)
		{
			min_cost = *(command->cost->exec_cost[i]) + send_output(command, i, receiver);
			if(cmd>0)
				min_cost = min_cost + plans[cmd-1]->execution[i]->cost;
			min_host = i;
		}
		else
		{
			cost = *(command->cost->exec_cost[i]) + send_output(command, i, receiver);
			if(cmd>0)
				cost = cost + plans[cmd-1]->execution[i]->cost;
			if(cost<min_cost)
			{
				min_cost = cost;
				min_host = i;
			}
		}
	}
	
	host_cost->host = min_host;
	host_cost->cost = min_cost;
}

long send_output(COMMAND *command, int sender, int receiver)
{
	if(sender!=receiver)
	{
		switch(command->type)
		{
			case cm_simple:
				if(!command->value.Simple->redirects)
					return command->cost->output;
				else if(command->value.Simple->redirects->redirector ==1 && command->value.Simple->redirects->redirectee.file->host == receiver)
					return command->cost->output;
				else
					return command->cost->output;
				break;

			case cm_pipe:
				if(!command->value.Pipe->com->value.Simple->redirects)
					return command->cost->output;
				else if(command->value.Pipe->com->value.Simple->redirects->redirector ==1 && command->value.Pipe->com->value.Simple->redirects->redirectee.file->host == receiver)
					return command->cost->output;
				else
					return command->cost->output;
				break;
		}
	}
	return 0;
}

/*long send_final_output(COMMAND *command, int sender, int receiver)
{
	case cm_simple:
	
	case cm_pipe:
		if(!command->value.Pipe->next)
		{
			
		}
}*/

int get_min_total_cost_index(EXEC_COST **plans)
{
	long min_cost;
	int min_index;
	int cmds = global_plan->num_of_coms;
	HOST_COST *min_host_cost = plans[cmds-1]->execution[0];
	min_cost = min_host_cost->cost;
	min_index = 0;
	
	for(int i=0; i<global_host_list->num_of_hosts+1; i++)
	{
		if(plans[cmds-1]->execution[i]->cost < min_cost)
		{
			min_host_cost = plans[cmds-1]->execution[i];
			min_cost = min_host_cost->cost;
			min_index = i;
		}
	}
	return min_index;
}

int get_min_final_total_cost_index(EXEC_COST **plans)
{	
	int min_index=0;
	int cmds = global_plan->num_of_coms;
	COMMAND *command = getCommand(global_command, cmds-1);
	REDIRECT *redirect = NULL;
	switch(command->type)
	{
		case cm_simple:
			redirect = getFinalOutputRedirection(command);
			break;

		case cm_pipe:
			redirect = getFinalOutputRedirection(command->value.Pipe->com);
			break;
	}

	if(redirect!=NULL)
		min_index = redirect->redirectee.file->host;

	return min_index;
}	

void write_to_global_plan(EXEC_COST **plans)
{
	int size = global_plan->num_of_coms;
	int min_index = get_min_final_total_cost_index(plans);	
	global_plan->total_cost = plans[size-1]->execution[min_index]->cost;
	for(int i=size-1; i>=0 ; i--)
	{
		DEBUG(("\nmin_index : %d\n", min_index)); 
		/*if(i==size-1){
			*(global_plan->array[i]) = min_index;
			min_index = min_index;
		}
		else{*/
			*(global_plan->array[i]) = plans[i]->execution[min_index]->host;	
			min_index = plans[i]->execution[min_index]->host;
		//}
	}
	
	print_plan(global_plan);
	
	for(int i=0; i<size; i++)
	{
		dispose_exec_cost(plans[i]);
	}
}
	
void findExhaustivePatterns(COMMAND *command, int size, int hosts)
{
	int result[size];
	long min_cost = -1;
	long returned_cost=0;
	for(int i=0;i<size;i++)
	{
		result[i]=0;
	}

	while(1)
	{
		for(int i=0;i<size;i++)
		{
			printf("result[%d] : %d ", i, result[i]); 
			 	
		}
		printf("\n");

		if(min_cost == -1){
			min_cost = compute_pattern_cost(result, size, command);
			copy_array(global_plan->array, result, size);	
			global_plan->total_cost = min_cost;
		}
		else
		{
			returned_cost = compute_pattern_cost(result, size, command);
			if(returned_cost < min_cost)
			{
				copy_array(global_plan->array, result, size);
				min_cost = returned_cost;
				global_plan->total_cost = min_cost;
			}
			
		}

		for(int i = 0;i<=size;i++)
		{
			if(i==size)
				return;
			
			if(result[i]==hosts-1)
				result[i]=0;
			else{
				result[i] = result[i]+1;
				break;
			}
		}
	}
}

long compute_pattern_cost(int *pattern, int size, COMMAND *command)
{
	COMMAND *command1 = command;
	PIPE_COM *pipe = command->value.Pipe;
	long total_cost = 0;

	for(int i=0; i<size; i++)
	{
		total_cost += *(command1->cost->exec_cost[pattern[i]]); 
		if(i<size-1)
		{
			total_cost += send_output(command1, pattern[i], pattern[i+1]);
		}
		command1 = pipe->next;
		if(command1)
			pipe = command1->value.Pipe;
	}
	//DEBUG(("total_cost : %ld\n", total_cost));
	return total_cost;
}

int check_remote_threshold_grp(EXECUTION_GRP *grp)
{
	COMMAND *command1 = grp->start;
	PIPE_COM *pipe = command1->value.Pipe;
	long threshold = 0;	

	for(int i=0; i< grp->num_of_coms; i++)
	{
		threshold += *(command1->cost->input[grp->remote]);
		command1 = pipe->next;
		if(command1)
			pipe = command1->value.Pipe;
	}

	if(threshold >= 4194304)
		return 1;
	return 0;	
}

int check_remote_threshold_simple(COMMAND *command)
{
	long threshold = 0;
	int host = *(global_plan->array[0]);
	threshold += *(command->cost->input[host]);
	
	if(threshold >= 4194304)
	{
		return 1;
	}
	*(global_plan->array[0])=0;
	return 0;
}

void check_remote_threshold_grp_array(EXECUTION_GRP *array, int size)
{
	for(int i=0; i<size; i++)
	{
		int flag = check_remote_threshold_grp(array+i);
		DEBUG(("\nthreshold flag: %d\n", flag));
		if(!flag)
			(array+i)->remote = 0;
	}
}
