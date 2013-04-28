#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "copy_cmd.h"


void copy_remote(REMOTE *remote1, REMOTE *remote2)
{
	remote2->exec_remote = remote1->exec_remote;
	if(remote1->server!=NULL)
		remote2->server = strdup(remote1->server);
	else
		remote2->server = NULL;
	if(remote1->directory!=NULL)
		remote2->directory = strdup(remote1->directory);
	else
		remote2->directory = NULL;
}

void copy_remote_from_remote_file(REMOTE *remote, REMOTE_FILE *remote_file)
{
	remote->exec_remote = 1;
	remote->server = strdup(remote_file->server);
	remote->directory = strdup(remote_file->directory);
}

void copy_cost(COST *cost1, COST *cost2, int size)
{
	copy_long_array(cost1->input, cost2->input, size);
	cost2->output = cost1->output;
	copy_long_array(cost1->redir_output, cost2->redir_output, size);
	cost2->piped_input = cost1->piped_input;
	copy_long_array(cost1->exec_cost, cost2->exec_cost, size);
}

void copy_long_array(long** source, long** dest, int size)
{
	for(int i=0; i<size; i++)
		*(dest[i]) = *(source[i]);
}


void copy_array(int** dest, int* source, int size)
{
	for(int i=0; i<size; i++)
	{
		**(dest+i) = source[i];
	}
}
