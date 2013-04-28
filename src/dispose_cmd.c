#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dispose_cmd.h"
#include "shared.h"

/* How to free a WORD_DESC. */

void dispose_word (WORD_DESC *w)
{
  free (w->word);
  free (w);
}

/* How to get rid of a linked list of words.  A WORD_LIST. */
void dispose_word_list (WORD_LIST *list)
{
	WORD_LIST *t;

	while (list)
	{
		t = list;
		list = list->next;
		dispose_word (t->word);
		free (t);
    	}
}

/* How to get rid of a linked list of files.  A FILE_LIST. */
void dispose_file_list (FILE_LIST *list)
{
	FILE_LIST *t;
	while (list)
	{
		t = list;
		list = list->next;
		dispose_file (t->file);
		free (t);
    	}
}

void dispose_remote_file(REMOTE_FILE *remote_file)
{
	if(remote_file==NULL)
		return;
	free (remote_file->server);
	free (remote_file->directory);
	free (remote_file->filename);
	free (remote_file);
}

void dispose_file(FILEE *file)
{
	dispose_word (file->orgfileword);
	dispose_remote_file (file->remotef);
	free (file);
}	

void dispose_cost(COST *cost)
{
	for(int i =0; i<global_host_list->num_of_hosts+1; i++)
	{
		free(cost->input[i]);
		free(cost->redir_output[i]);
		free(cost->exec_cost[i]);
	}		
	free(cost->input);
	free(cost->redir_output);
	free(cost->exec_cost);
	free(cost);
}

void dispose_redirectee(REDIRECTEE redirectee)
{
	dispose_file(redirectee.file);
	//free(redirectee);
}

void dispose_redirection(REDIRECT *redir)
{
	REDIRECT *t;
	while (redir)
	{
		t = redir;
		redir = redir->next;
		dispose_redirectee (t->redirectee);
		free (t);
    	}
	free (redir);
}

void dispose_command(COMMAND *command)
{
	if(command == 0)
		return;
	switch(command->type)
	{
		case cm_simple:
			dispose_simple_command(command->value.Simple);
			break;
		case cm_pipe:
			dispose_pipe_command(command->value.Pipe);
			break;
	}
	dispose_remote(command->remote);
	dispose_cost(command->cost);
	free (command);
}

void dispose_simple_command(SIMPLE_COM *simple)
{
	dispose_word_list (simple->words);
	dispose_file_list (simple->files);
	dispose_redirection (simple->redirects);	
	free (simple);
}

void dispose_pipe_command(PIPE_COM *pipe)
{
	dispose_command(pipe->com);
	dispose_command(pipe->next);
	free (pipe);		
}

void dispose_argv(char **args)
{
	free(args);
}

void dispose_remote(REMOTE *remote)
{
	if(remote == NULL)
		return;
	free(remote->server);
	free(remote->directory);
	free(remote);
}

void dispose_plan(PLAN *plan)
{
	if(plan==NULL)
		return;
	
	dispose_array(plan->array, plan->num_of_coms);
	free(plan);
}

void dispose_array(int** array, int size)
{
	for(int i=0; i<size; i++)
	{
		free(array[i]);
	}
	free(array);
}

void dispose_cmd_info_array(CMD_INFO **array, int size)
{
	for(int i=0; i<size; i++)
	{
		dispose_cmd_info(array[i]);
	}
	free(array);
}

void dispose_cmd_info(CMD_INFO* info)
{
	free(info->cmd);
	free(info);
}

void dispose_host_list(HOST_LIST* list)
{
	for(int i=0; i<list->num_of_hosts; i++)
	{
		free(list->host_array[i]->server);
		free(list->host_array[i]);
	}
	free(list->host_array);
	free(list);
}

void dispose_exec_cost(EXEC_COST *exec_cost)
{
	for(int i=0; i<global_host_list->num_of_hosts+1; i++)
	{
		free(exec_cost->execution[i]);
	}
	free(exec_cost->execution);
	free(exec_cost);
}

void clean_up_hidden_directories(HOST_LIST *list)
{
	char command[100];
	for(int i=0; i<list->num_of_hosts; i++)
	{
		memset(command, 0, 100);
		sprintf(command,"ssh %s \"rm -rf .~\"", list->host_array[i]->server);
		system(command);
	}
}
