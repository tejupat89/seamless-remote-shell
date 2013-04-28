#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "command.h"
#include "make_cmd.h"
#include "localremote.h"
#include "convert.h"
#include "shared.h"
#include "copy_cmd.h"

WORD_DESC* make_bare_word (char *string)
{
	WORD_DESC *temp;

	temp = (WORD_DESC *)calloc(sizeof (WORD_DESC), sizeof(char));
	if (*string)
	   temp->word = string;
	else
	{
	   temp->word = (char *)calloc(1, sizeof(char));
	   temp->word[0] = '\0';
	}

	return (temp);
}

WORD_DESC* make_word(char *string){
	WORD_DESC *temp;
	temp = make_bare_word (string);
	return (temp);
}

WORD_LIST* make_word_list(WORD_DESC *word, WORD_LIST *wlink){
	WORD_LIST *temp;

  	temp = (WORD_LIST *)calloc(sizeof(WORD_LIST), sizeof(char));
  	temp->word = word;
  	temp->next = (WORD_LIST *)NULL;
	if(wlink)
	{
		while(wlink->next){
			wlink = wlink->next;
		}
		wlink->next = temp;
	}
	else
		wlink = temp;
  	return (wlink);
}

FILEE* make_file(WORD_DESC *word){
	FILEE *temp;

	temp = (FILEE *)calloc(sizeof(FILEE), sizeof(char));
	temp->orgfileword = word;
	temp->remote = local_or_remote(temp->orgfileword->word);
	temp->host = 0;
	temp->size = calculate_size(temp->orgfileword->word);
	if(temp->remote==0)
		temp->remotef = (REMOTE_FILE *)NULL;
	else{
		temp->remotef = make_remote_file(temp->orgfileword->word);
	}
	return (temp);
}

FILEE* make_redir_file(WORD_DESC *word){
	FILEE *temp;
	int filename_len = strlen(word->word);
	char dir[filename_len];
	memset(dir, 0, filename_len);
	extract_directory(word->word, dir);

	temp = (FILEE *)calloc(sizeof(FILEE), sizeof(char));
	temp->orgfileword = word;
	temp->remote = local_or_remote(dir);
	temp->size = calculate_size(temp->orgfileword->word);
	if(temp->remote==0)
		temp->remotef = (REMOTE_FILE *)NULL;
	else{
		temp->remotef = make_remote_file(temp->orgfileword->word);
	}
	return (temp);
}

FILE_LIST* make_file_list(FILEE *file, FILE_LIST *flink){
	FILE_LIST *temp;

	temp = (FILE_LIST *)calloc(sizeof(FILE_LIST), sizeof(char));
	temp->file = file;
	temp->next = (FILE_LIST *)NULL;
	while(flink->next){
		flink = flink->next;
	}
	flink->next = temp;
	return (flink);
} 

REMOTE_FILE *make_remote_file(char *orgfilename)
{
	REMOTE_FILE *temp;

	temp = (REMOTE_FILE *)calloc(sizeof(REMOTE_FILE), sizeof(char));
	temp = convert_to_remote (orgfilename, temp);

	return (temp);
}

REMOTE_FILE *make_remote_file_given(char *server, char *directory, char *filename)
{
	REMOTE_FILE *temp;

	temp = (REMOTE_FILE *)calloc(sizeof(REMOTE_FILE), sizeof(char));
	temp->server = strdup(server);
	temp->directory = strdup(directory);
	temp->filename = strdup(filename);

	return (temp);
}

REDIRECT* make_redirection(int source, enum r_instruction instruction, REDIRECTEE dest_and_filename)
{
	REDIRECT *temp;

	temp = (REDIRECT *)calloc(sizeof(REDIRECT), sizeof(char));
	temp->redirector = source;
	temp->instruction = instruction;
	temp->flags = 0;
	temp->redirectee = dest_and_filename;
	temp->next = (REDIRECT *)NULL;

	switch(instruction)
	{
		case r_output_direction:
			temp->flags = O_RDWR | O_CREAT;
			break;

		case r_input_direction:
			temp->flags = O_RDONLY;
			break;
	}
	return (temp);
}
 
/* make a bare simple command-initialize all the slots */
COMMAND *make_bare_simple_command ()
{
	COMMAND *command;
	SIMPLE_COM *temp;

	command = (COMMAND *)malloc (sizeof (COMMAND));
	temp = (SIMPLE_COM *)malloc (sizeof (SIMPLE_COM));
	command->value.Simple = temp;

	temp->words = (WORD_LIST *)NULL;
	temp->files = (FILE_LIST *)NULL;	
	temp->redirects = (REDIRECT *)NULL;

	command->type = cm_simple;
	command->remote = make_remote();
	command->cost = make_cost();

	return (command);
}

REMOTE *make_remote()
{
	REMOTE *temp = (REMOTE *)calloc(sizeof(struct remote), sizeof(char));
	temp->exec_remote = 0;
	temp->server = NULL;
	temp->directory = NULL;
	return (temp);
}

COST *make_cost()
{
	COST *temp = (COST *)calloc(sizeof(struct cost), sizeof(char));
	temp->input = (long**)NULL;
	temp->output = 0;
	temp->redir_output = (long**)NULL;
	temp->piped_input = 0; 
	temp->exec_cost = (long**)NULL;
	return (temp);
}

long **make_long_array()
{
	int size = global_host_list->num_of_hosts+1;
	long **temp = (long**)calloc(sizeof(long*)*size, sizeof(char));
	for(int i = 0; i<size; i++)
	{
		temp[i] = (long *)calloc(sizeof(long), sizeof(char));
	}
	return temp;
}

PLAN *make_plan(int size)
{
	PLAN *plan = (PLAN *)calloc(sizeof(struct plan), sizeof(char));

	plan->array = (int **)calloc(sizeof(int*)*size, sizeof(char));
	for(int i=0;i<size;i++)
	{
		plan->array[i] = (int *)calloc(sizeof(int), sizeof(char));
	}
	plan->num_of_coms = size;
	plan->total_cost = 0;
	return plan;
}

/* Return a command which is the connection of the word or redirection in ELEMENT, and the command * or NULL in COMMAND. */
COMMAND* make_simple_command(ELEMENT element, COMMAND *command){
	/* If we are starting from scratch, then make the initial command
     	structure.  Also note that we have to fill in all the slots, since
     	malloc doesn't return zeroed space. */
  	if (!command)
    		command = make_bare_simple_command ();

  	if (element.word)
    	{
		WORD_LIST *tw = (WORD_LIST *)calloc (sizeof (WORD_LIST), sizeof(char));
		tw->word = element.word;
		/*tw->next = command->value.Simple->words;
		command->value.Simple->words = tw;*/
		tw->next = (WORD_LIST *)NULL;
		if(command->value.Simple->words == NULL)
			command->value.Simple->words = tw;
		else
		{
			WORD_LIST *list = command->value.Simple->words;
			while(list->next){
				list = list->next;
			}
			list->next = tw;
		}
		
    	}
	else if (element.option)
    	{
		WORD_LIST *tw = (WORD_LIST *)calloc (sizeof (WORD_LIST), sizeof(char));
		tw->word = element.option;
		/*tw->next = command->value.Simple->words;
		command->value.Simple->words = tw;*/
		tw->next = (WORD_LIST *)NULL;
		if(command->value.Simple->words == NULL)
			command->value.Simple->words = tw;
		else
		{
			WORD_LIST *list = command->value.Simple->words;
			while(list->next){
				list = list->next;
			}
			list->next = tw;
		}
    	}
	else if(element.file)
	{
		FILE_LIST *tf = (FILE_LIST *)calloc (sizeof (FILE_LIST), sizeof(char));
		tf->file = element.file;
		/*tf->next = command->value.Simple->files;
		command->value.Simple->files = tf;*/
		tf->next = (FILE_LIST *)NULL;
		if(command->value.Simple->files == NULL)
			command->value.Simple->files = tf;
		else
		{
			FILE_LIST *list = command->value.Simple->files;
			while(list->next){
				list = list->next;
			}
			list->next = tf;
		}
	}
  	else if(element.redirect)
    	{
      		REDIRECT *r = element.redirect;
      		/* Due to the way <> is implemented, there may be more than a single
	 	redirection in element.redirect.  We just follow the chain as far
	 	as it goes, and hook onto the end. */
      		/*while (r->next)
			r = r->next;*/
		if(command->value.Simple->redirects == NULL)
			command->value.Simple->redirects = r;
		else
		{
	      		REDIRECT *list = command->value.Simple->redirects;
			while(list->next)
				list = list->next;
			list->next = r;		
		}
	}
	return (command);
}

COMMAND* make_pipe_command (COMMAND* com1, COMMAND* com2)
{
	COMMAND *temp = make_bare_pipe_command (com2);

	/*piped commands are stored in ascending order(in-order) in linked list*/
	COMMAND *com = com1;
	while(com->value.Pipe->next){
		com = com->value.Pipe->next;
	}
	com->value.Pipe->next = temp;
	temp->value.Pipe->prev = com;
	return (com1);
}

COMMAND* make_bare_pipe_command (COMMAND* com)
{
	COMMAND *command;
	PIPE_COM *temp;

	command = (COMMAND *)calloc(sizeof(COMMAND), sizeof(char));
	temp = (PIPE_COM *)calloc(sizeof(PIPE_COM), sizeof(char));
	command->value.Pipe = temp;
	
	temp->com = com;
	temp->next = (COMMAND *)NULL;
	temp->prev = (COMMAND *)NULL;

	command->type = cm_pipe;
	command->remote = make_remote();
	command->cost = make_cost();

	return (command);
}

CMD_INFO** make_cmd_info_array(int size)
{
	CMD_INFO** array = (CMD_INFO**)calloc(sizeof(CMD_INFO*)*size, sizeof(char));
	for(int i=0; i<size; i++)
	{
		array[i] = (CMD_INFO*)calloc(sizeof(CMD_INFO), sizeof(char));
		array[i]->cmd = NULL;
		array[i]->output = 0;
		array[i]->speed = 0;
	}	
	return array;
}

HOST *make_host(int host_num, char *server)
{
	HOST *temp = (HOST*)calloc(sizeof(HOST), sizeof(char));
	temp->host_num = host_num;
	temp->server = server;
	return temp;
}

HOST_LIST *make_host_list(int num_of_hosts)
{
	HOST_LIST *temp = (HOST_LIST *)calloc(sizeof(HOST_LIST), sizeof(char));
	temp->num_of_hosts = num_of_hosts;
	temp->host_array = (HOST**)calloc(sizeof(HOST*)*num_of_hosts, sizeof(char));
	for(int i=0; i<num_of_hosts; i++)
	{
		temp->host_array[i] = (HOST*)NULL;
	}
	return temp;
}


