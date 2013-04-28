#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include "convert.h"
#include "command.h"
#include "shared.h"
#include "debug.h"
#include "print_cmd.h"
#include "make_cmd.h"
#include "new_cost.h"
#include "plan.h"
#include "execute_cmd.h"


char **word_list_to_argv (WORD_LIST *list)
{
	int c,count;
	char **array;

	count = word_list_length (list);
	array = (char **)calloc ((1 + count) * sizeof (char *), sizeof(char));

	for (c=0; list; c++, list = list->next)
		array[c] = list->word->word;
	array[count] = (char *)NULL;

	return (array);
}

int word_list_length(WORD_LIST *list)
{
	WORD_LIST *temp=list;
	int count=0;
	while(temp)
	{
		count++;
		temp=temp->next;
	}
	return count;
}

int file_list_length(FILE_LIST *list)
{
	FILE_LIST *temp=list;
	int count=0;
	while(temp)
	{
		count++;
		temp=temp->next;
	}
	return count;
}

char **file_list_to_argv (FILE_LIST *list)
{
	int c,count;
	char **array;

	count = file_list_length (list);
	array = (char **)calloc ((1 + count) * sizeof (char *), sizeof(char));

	for (c=0; list; c++, list = list->next)
		array[c] = list->file->orgfileword->word;
	array[count] = (char *)NULL;

	return (array);
}

char **word_file_to_argv (WORD_LIST *wlist, FILE_LIST *flist)
{
	int c,wcount,fcount,count;
	char **array;

	wcount = word_list_length (wlist);
	fcount = file_list_length (flist);
	count = wcount + fcount;
	array = (char **)calloc ((1 + count) * sizeof (char *), sizeof(char));

	for (c=0; wlist; c++, wlist = wlist->next)
		array[c] = strdup(wlist->word->word);
	
	for (c=wcount; flist; c++, flist = flist->next){
		array[c] = strdup(flist->file->orgfileword->word);
	}
	array[count] = (char *)NULL;

	return (array);
}

char *extract_filename (char *filepath)
{
	if(strrchr(filepath, '/'))
		return (strrchr(filepath, '/')+1);
	else
		return filepath;
}

char *extract_directory(char *filepath, char *directory)
{
	if(strrchr(filepath, '/'))
	{
		int len = strlen(strrchr(filepath, '/'));
		int total_len = strlen(filepath);
		strncpy(directory, filepath, total_len-len);
		directory[total_len-len] = '\0';
	}
	else
	{
		strcpy(directory, "/home/tejas");	//change later 
	}
	return directory;
}

char *simple_command_to_remote(SIMPLE_COM *simple)
{
	char *buffer = (char *)calloc(200, sizeof(char));
	WORD_LIST *wlist = simple->words;
	FILE_LIST *flist = simple->files;
	REDIRECT *redir = simple->redirects;
	
	/* write the command word */	
	buffer = strcat(buffer, wlist->word->word);
	wlist = wlist->next;
	if(wlist)
		buffer = strcat(buffer, " ");
		
	/* if there are more words then they are patterns for sure, put them in quotes "pattern" */
	while(wlist)
	{
		/*is an OPTION/FLAG */
		if(strchr(wlist->word->word+0, '-'))
		{
			buffer = strcat(buffer, wlist->word->word);
		}
		else /* otherwise is a pattern */
		{
			buffer = strcat(buffer, "\"");
			buffer = strcat(buffer, wlist->word->word);
			buffer = strcat(buffer, "\"");
		}
		wlist = wlist->next;
		if(wlist)
			buffer = strcat(buffer, " ");
	}
	while(flist)
	{
		buffer = strcat(buffer, " ");
		buffer = strcat(buffer, flist->file->remotef->directory);
		buffer = strcat(buffer, "/");
		buffer = strcat(buffer, flist->file->remotef->filename);
		flist = flist->next;
	}
	while(redir)
	{
		buffer = strcat(buffer, " ");
		if(redir->redirector==1 && redir->redirectee.file->remote==1)
		{
			buffer = strcat(buffer, "> ");
			buffer = strcat(buffer, redir->redirectee.file->remotef->directory);
			buffer = strcat(buffer, "/");
			buffer = strcat(buffer, redir->redirectee.file->remotef->filename);
		}
		else if(redir->redirector==1 && redir->redirectee.file->remote==0 && redir->next==NULL)
		{
			/*buffer = strcat(buffer, "> ");
			buffer = strcat(buffer, redir->redirectee.file->remotef->directory);
			buffer = strcat(buffer, "/");
			buffer = strcat(buffer, redir->redirectee.file->remotef->filename);*/
			do_redirections(redir);
		}
		else if(redir->redirector == 0)
		{
			buffer = strcat(buffer, "< ");
			buffer = strcat(buffer, redir->redirectee.file->remotef->directory);
			buffer = strcat(buffer, "/");
			buffer = strcat(buffer, redir->redirectee.file->remotef->filename); 
		}
		redir = redir->next;
	}
	return (buffer);
}

char *simple_command_to_remote_in_pipe(COMMAND *command, int index)
{
	char *buffer = (char *)calloc(200, sizeof(char));
	SIMPLE_COM *simple = command->value.Pipe->com->value.Simple;
	WORD_LIST *wlist = simple->words;
	FILE_LIST *flist = simple->files;
	REDIRECT *redir = simple->redirects;
	
	/* write the command word */	
	buffer = strcat(buffer, wlist->word->word);
	wlist = wlist->next;
	if(wlist)
		buffer = strcat(buffer, " ");
		
	/* if there are more words then they are patterns for sure, put them in quotes "pattern" */
	while(wlist)
	{
		/*is an OPTION/FLAG */
		if(strchr(wlist->word->word+0, '-'))
		{
			buffer = strcat(buffer, wlist->word->word);
		}
		else /* otherwise is a pattern */
		{
			buffer = strcat(buffer, "\"");
			buffer = strcat(buffer, wlist->word->word);
			buffer = strcat(buffer, "\"");
		}
		wlist = wlist->next;
		if(wlist)
			buffer = strcat(buffer, " ");
	}
	while(flist)
	{
		buffer = strcat(buffer, " ");
		buffer = strcat(buffer, flist->file->remotef->directory);
		buffer = strcat(buffer, "/");
		buffer = strcat(buffer, flist->file->remotef->filename);
		flist = flist->next;
	}
	while(redir)
	{
		buffer = strcat(buffer, " ");
		if(redir->redirector==1 && redir->redirectee.file->host==*(global_plan->array[index]))
		{
			buffer = strcat(buffer, "> ");
			buffer = strcat(buffer, redir->redirectee.file->remotef->directory);
			buffer = strcat(buffer, "/");
			buffer = strcat(buffer, redir->redirectee.file->remotef->filename);
		}
		else if(redir->redirector==1 && redir->redirectee.file->host!=*(global_plan->array[index]) && redir->next==NULL)
		{
			/*buffer = strcat(buffer, "> ");
			buffer = strcat(buffer, redir->redirectee.file->remotef->directory);
			buffer = strcat(buffer, "/");
			buffer = strcat(buffer, redir->redirectee.file->remotef->filename);*/
			do_redirections(redir);
		}
		else if(redir->redirector == 0)
		{
			buffer = strcat(buffer, "< ");
			buffer = strcat(buffer, redir->redirectee.file->remotef->directory);
			buffer = strcat(buffer, "/");
			buffer = strcat(buffer, redir->redirectee.file->remotef->filename); 
		}
		redir = redir->next;
	}
	return (buffer);	
}

char *pipe_command_to_remote(COMMAND *command, int size)
{
	char *buffer = (char *)calloc(1000, sizeof(char));
	//COMMAND *next_com = command->next;
	for(int i=0;i<size;i++)
	{
		//char *temp = simple_command_to_remote(command->value.Pipe->com->value.Simple);
		char *temp = simple_command_to_remote_in_pipe(command, i);
		strcat(buffer, temp);
		free(temp);
		command = command->value.Pipe->next;
		if(i!=size-1)
			strcat(buffer, " | ");
	}
	return (buffer);
}

int word_list_len(WORD_LIST *list)
{
	int len=0;
	while(list)
	{
		len = len + strlen(list->word->word);
		list = list->next;
	}
	return len;
}

int file_list_len(FILE_LIST *list)
{
	int len=0;
	while(list)
	{
		len = len + strlen(list->file->orgfileword->word);
		list = list->next;
	}
	return len;
}

int redirects_len(REDIRECT *redir)
{
	int len=0;
	while(redir)
	{
		len = len + strlen(redir->redirectee.file->orgfileword->word);
		redir = redir->next;
	}
	return len;
}

void com_string_to_argv(char *com_string, char **args)
{ 
	char **ap = args;
	do {			
		*ap = strsep(&com_string, " \t");	 
		if(**ap != '\0')
			ap++;
	} while(com_string != NULL);
	*ap=0;
}

int num_of_commands(COMMAND *command)
{
	int com_num = 0;
	switch(command->type)
	{
		case cm_simple:
		{
			com_num = 1;
			break;
		}

		case cm_pipe:
		{
			PIPE_COM *pipe = command->value.Pipe;
			COMMAND *command1 = command;
			while(command1)
			{
				com_num++;
				command1 = pipe->next;
				if(command1)
					pipe = command1->value.Pipe;
			}
		}
	}
	return com_num;
}

COMMAND *getCommand(COMMAND *command, int index)
{
	COMMAND *command1 = command;
	PIPE_COM *pipe = command1->value.Pipe;
	if(index == 0)
		return command1;
	while(index>0)
	{
		command1 = pipe->next;
		if(command1)
			pipe = command1->value.Pipe;
		index--;
	}
	return command1;
}

REDIRECT *getFinalOutputRedirection(COMMAND *command)
{
	REDIRECT *redirect = command->value.Simple->redirects;
	REDIRECT *result = NULL;
	if(redirect==NULL)
		return NULL;
	while(redirect)
	{
		if(redirect->redirector==1)
			result = redirect;
		redirect = redirect->next;
	}
	return result;
}	



void load_cmd_info_array(CMD_INFO** array, int size)
{
	char *line = NULL;
	size_t len = 0;
	FILE *fp = fopen("cmd_stats.txt", "r");
	char buf[10];
	char *bufp = buf;
	int index = 0;
	while(getline(&line, &len, fp)!=-1)
	{
		int j = 0;
		while(line)
		{
			bufp = strsep(&line, ",");
			if(j==0)
				array[index]->cmd = strdup(bufp);
			else if (j==1)
				array[index]->output = atoi(bufp);
			else if(j==2)
				array[index]->speed = atoi(bufp);
			j++;			
		}
		index++;
	}
	if(line)
		free(line);
	fclose(fp);
	//print_cmd_info_array(array, size);
}

void fill_host_list(COMMAND *command)
{
	switch(command->type)
	{
		case cm_simple:
			fill_host_list_simple(command->value.Simple);
			break;
	
		case cm_pipe:
			fill_host_list_pipe(command);
			break;
	}
}

void fill_host_list_simple(SIMPLE_COM *simple)
{
	int count=0;
	FILE_LIST *files = simple->files;
	REDIRECT* redirects = simple->redirects;
	while(files)
	{
		if(files->file->remote==1)
			count++;
		files = files->next;	
	}	
	while(redirects)
	{
		if(redirects->redirectee.file->remote==1)
			count++;
		redirects = redirects->next;
	}
	
	char *list[count];
	int index =0;
	files = simple->files;
	redirects = simple->redirects;
	while(files)
	{
		if(files->file->remote==1 && notPresent(list, index, files->file->remotef->server)){
			DEBUG(("\nserver: %s \n", files->file->remotef->server));
			list[index++]=files->file->remotef->server;
		}
		files = files->next;	
	}
	while(redirects)
	{
		if(redirects->redirectee.file->remote==1 && notPresent(list, index, redirects->redirectee.file->remotef->server))
			list[index++]=redirects->redirectee.file->remotef->server;
		redirects = redirects->next;
	}

	global_host_list = make_host_list(index);
	for(int i=0; i<index; i++)
	{
		global_host_list->host_array[i]=make_host(i+1, strdup(list[i]));		
	}
	print_host_list(global_host_list);
}

int notPresent(char **list, int index, char *server)
{
	for(int i=0; i<index; i++)
	{
		if(strcmp(list[i], server)==0)
			return 0;
	}
	return 1;
}

void fill_host_list_pipe(COMMAND *command)
{
	COMMAND *command1 = command;
	PIPE_COM *pipe = command->value.Pipe;
	SIMPLE_COM *simple = pipe->com->value.Simple;
	FILE_LIST *files = simple->files;
	REDIRECT *redirects = simple->redirects;
	int count = 0;
	
	while(command1)
	{
		files = simple->files;
		redirects = simple->redirects;
		
		while(files)
		{
			if(files->file->remote==1)
				count++;
			files = files->next;	
		}	
		while(redirects)
		{
			if(redirects->redirectee.file->remote==1)
				count++;
			redirects = redirects->next;
		}
		command1 = pipe->next;
		if(command1)
		{
			pipe = command1->value.Pipe;
			simple = pipe->com->value.Simple;
		}
	}	

	char *list[count];
	int index =0;
	command1 = command;
	pipe = command->value.Pipe;
	simple = command->value.Pipe->com->value.Simple;
	files = simple->files;
	redirects = simple->redirects;
	while(command1)
	{
		files = simple->files;
		redirects = simple->redirects;
		
		while(files)
		{
			if(files->file->remote==1 && notPresent(list, index, files->file->remotef->server))
				list[index++]=files->file->remotef->server;
			files = files->next;	
		}	
		while(redirects)
		{
			if(redirects->redirectee.file->remote==1 && notPresent(list, index, redirects->redirectee.file->remotef->server))
				list[index++]=redirects->redirectee.file->remotef->server;
			redirects = redirects->next;
		}
		command1 = pipe->next;
		if(command1)
		{
			pipe = command1->value.Pipe;
			simple = pipe->com->value.Simple;
		}
	}
		
	global_host_list = make_host_list(index);
	for(int i=0; i<index; i++)
	{
		global_host_list->host_array[i]=make_host(i+1, strdup(list[i]));		
	}
	print_host_list(global_host_list);
}

void fill_files_with_host_nums(COMMAND *command)
{
	if(command == NULL)
		return;
	switch(command->type)
	{
		case cm_simple:
			fill_files_with_host_nums_simple(command->value.Simple);
			break;
	
		case cm_pipe:
			fill_files_with_host_nums_pipe(command->value.Pipe);
			break;
	}
}
	
void fill_files_with_host_nums_simple(SIMPLE_COM *simple)
{
	FILE_LIST *files = simple->files;
	REDIRECT *redirects = simple->redirects;
	while(files)
	{
		for(int i=0; i<global_host_list->num_of_hosts; i++)
		{
			if(files->file->remote==1 && strcmp(global_host_list->host_array[i]->server, files->file->remotef->server)==0)
			{
				files->file->host = global_host_list->host_array[i]->host_num;
				break;
			}
		}
		files = files->next;
	}
	while(redirects)
	{
		for(int i=0; i<global_host_list->num_of_hosts; i++)
		{
			if(redirects->redirectee.file->remote==1 && strcmp(global_host_list->host_array[i]->server, redirects->redirectee.file->remotef->server)==0)
			{
				redirects->redirectee.file->host = global_host_list->host_array[i]->host_num;
				break;
			}
		}
		redirects = redirects->next;
	}
}

void fill_files_with_host_nums_pipe(PIPE_COM *pipe)
{
	if(pipe==NULL)
		return;	
	fill_files_with_host_nums(pipe->com);
	fill_files_with_host_nums(pipe->next);
}

void fill_commands_with_cost_array(COMMAND *command)
{
	if(command==NULL)
		return;
	switch(command->type)
	{
		case cm_simple:
			command->cost->input = make_long_array();
			command->cost->redir_output = make_long_array();
			command->cost->exec_cost = make_long_array();
			break;

		case cm_pipe:
			command->cost->input = make_long_array();
			command->cost->redir_output = make_long_array();
			command->cost->exec_cost = make_long_array();
			fill_commands_with_cost_array(command->value.Pipe->com);
			fill_commands_with_cost_array(command->value.Pipe->next);
			break;
	}
}

void pre_processing(COMMAND *command)
{
	fill_host_list(command);
	fill_files_with_host_nums(command);
	fill_commands_with_cost_array(command);
	cost_calculate(command);
	plan(command);
	create_hidden_directories(global_host_list);
	/*if(global_host_list->num_of_hosts>0)
		//create_plans(command);
		findExhaustivePatterns(command, global_plan->num_of_coms, global_host_list->num_of_hosts+1);*/
}
