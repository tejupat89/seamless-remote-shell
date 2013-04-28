#include<stdio.h>
#include<string.h>
#include "print_cmd.h"
#include "command.h"
#include "shared.h"
#include "debug.h"

void print_word(WORD_DESC *word){
	DEBUG(("\n"));
	DEBUG(("word->char: %s", word->word));
	DEBUG(("\n"));
}

void print_word_list(WORD_LIST *list){
	WORD_LIST *w=list;
	DEBUG(("\n"));
	while(w){
		DEBUG(("word->char: %s\n", w->word->word));
		w=w->next;
	} 
	DEBUG(("\n"));
}

void print_file(FILEE *file){
	DEBUG(("\n"));
	DEBUG(("file->orgfileword : %s\n", file->orgfileword->word));
	DEBUG(("file->remote : %d\n", file->remote));
	DEBUG(("file->host : %d\n", file->host));
	DEBUG(("file->size : %ld", file->size));
	if(file->remote)
		print_remote_file(file->remotef);
	DEBUG(("\n"));
}

void print_file_list(FILE_LIST *list){
	FILE_LIST *f;
	DEBUG(("\n"));
	for(f=list; f; f=f->next){
		DEBUG(("file->char: %s \n", f->file->orgfileword->word));
		//print_file(f->file);
	} 
	DEBUG(("\n"));
}

void print_remote_file(REMOTE_FILE *file)
{
	DEBUG(("\n"));
	DEBUG(("file->server : %s,   file->directory : %s,   file->filename : %s", file->server, file->directory, file->filename));
	DEBUG(("\n"));
}

void print_redirection(REDIRECT *redir)
{
	REDIRECT *temp=redir;
	DEBUG(("\n"));
	while(temp){
		DEBUG(("redir->redirector : %d, redir->flags : %d, redir->redirectee : %s\n", temp->redirector, temp->flags, temp->redirectee.file->orgfileword->word));
		temp=temp->next;
	}
	print_file(redir->redirectee.file);
	DEBUG(("\n"));
}

void print_simple_command(SIMPLE_COM *simple){
	DEBUG(("\n"));
	DEBUG(("Simple Command{\n"));
	if(simple->words){
		print_word_list(simple->words);
	}
	if(simple->files){
		print_file_list(simple->files);
	}
	if(simple->redirects)
		print_redirection(simple->redirects);
	DEBUG(("}\n"));
}

void print_pipe_command(PIPE_COM *pipe)
{
	DEBUG(("\n"));
	DEBUG(("Pipe Command {\n"));
	//COMMAND *c=pipe->next;
	print_command(pipe->com);
	DEBUG(("Pipe Command }\n"));
	print_command(pipe->next);
	
}

void print_command(COMMAND *command){
	if(command == NULL)
		return;
	if(command->type == cm_simple)
		print_simple_command(command->value.Simple);
	else if(command->type == cm_pipe)
		print_pipe_command(command->value.Pipe);
	//print_remote(command->remote);
	print_cost(command->cost);
}

void print_remote(REMOTE *remote)
{
	DEBUG(("remote {\n"));
	DEBUG(("remote->exec_remote : %d\n", remote->exec_remote));
	DEBUG(("remote->server : %s\n", remote->server));
	DEBUG(("remote->directory : %s\n", remote->directory));
	DEBUG(("}\n"));
}

void print_cost(COST *cost)
{
	DEBUG(("\n"));
	DEBUG(("COST {\n"));
	print_long_array(cost->input);
	DEBUG(("output : %ld\n", cost->output));
	print_long_array(cost->redir_output);
	DEBUG(("cost->piped_input : %ld\n", cost->piped_input));
	print_long_array(cost->exec_cost);
	DEBUG(("}\n"));
}

void print_long_array(long **array)
{
	int size = global_host_list->num_of_hosts+1;
	for(int i =0;i<size; i++)
	{
		DEBUG(("array[%d] : %ld  ", i, *(array[i])));
	}
	DEBUG(("\n"));
}

void print_plan(PLAN *plan)
{
	DEBUG(("\n"));
	DEBUG(("plan{\n"));
	for(int i=0;i<plan->num_of_coms;i++)
	{
		DEBUG(("plan->array[%d]: %d   ",i, *(plan->array[i])));
	}
	DEBUG(("\n"));
	DEBUG(("plan->num_of_coms : %d\n", plan->num_of_coms));
	DEBUG(("plan->total_cost : %ld\n", plan->total_cost));
	DEBUG(("\n"));
}

void print_execution_grp(EXECUTION_GRP *grp)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "EXECUTION_GRP{\n");
	fprintf(stderr, "grp->num_of_coms: %d\n", grp->num_of_coms);
	fprintf(stderr, "grp->remote: %d\n", grp->remote);
	fprintf(stderr, "}\n");
	fprintf(stderr, "\n");
}

void print_cmd_info_array(CMD_INFO** array, int size)
{
	for(int i=0;i<size;i++)
	{
		print_cmd_info(array[i]);
	}
}

void print_cmd_info(CMD_INFO *info)
{
	DEBUG(("\n"));
	DEBUG(("info->cmd:%s", info->cmd));
	DEBUG(("info->output:%d", info->output));
	DEBUG(("info->speed:%d", info->speed));
	DEBUG(("\n"));
}

void print_host_list(HOST_LIST *host_list)
{
	DEBUG(("\n"));
	DEBUG(("host_list->num_of_hosts : %d\n", host_list->num_of_hosts));
	for(int i=0; i<host_list->num_of_hosts; i++)
	{
		print_host(host_list->host_array[i]);
	}
	DEBUG(("\n"));
}

void print_host(HOST *host)
{
	DEBUG(("\n"));
	DEBUG(("host->host_num : %d  ", host->host_num));
	DEBUG(("host->server : %s", host->server));
	DEBUG(("\n"));
}	

void print_exec_cost(EXEC_COST *exec_cost)
{
	fprintf(stderr, "\n");
	for(int i=0; i<global_host_list->num_of_hosts+1; i++){
		fprintf(stderr, "on_host : %d  to_host : %d  ", exec_cost->execution[i]->host, i);
		fprintf(stderr, "cost : %ld  ", exec_cost->execution[i]->cost);
		fprintf(stderr, "\n");	
	}
	fprintf(stderr, "\n");
}
