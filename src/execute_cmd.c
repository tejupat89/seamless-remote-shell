#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "execute_cmd.h"
#include "convert.h"
#include "shared.h"
#include "make_cmd.h"
#include "print_cmd.h"
#include "dispose_cmd.h"
#include "new_cost.h"
#include "plan.h"
#include "debug.h"

extern char **environ;

void execute_command(COMMAND *command)
{
	switch(command->type)
	{
		case cm_simple:
			execute_simple_command(command);
			break;

		case cm_pipe:
			execute_pipe_command(command);	
			break;		
	}
	/*if(command->remote->exec_remote==1){
		execute_command_remotely(command);
	}
	else
		execute_command_locally(command);*/
}

/*void execute_command_locally(COMMAND *command)
{
	switch(command->type)
	{
		case cm_simple:
			execute_simple_command(command->value.Simple);
			break;
		
		case cm_pipe:
			execute_pipe_command(command->value.Pipe);
			break;
	}
}*/

void execute_simple_command(COMMAND *command)
{
	check_remote_threshold_simple(command);
	if(*(global_plan->array[0])==0)
		execute_simple_command_locally(command->value.Simple);
	else
	{
		execute_scp_simple(command, *(global_plan->array[0]));
		execute_simple_command_remotely(command, *(global_plan->array[0]));
	}
}

void execute_simple_command_locally(SIMPLE_COM *simple)
{
	if(simple->redirects)
		do_redirections(simple->redirects);

	char **args = word_file_to_argv(simple->words, simple->files);
	exec_execve(args);
	dispose_argv(args);
}

/*void execute_pipe_command(PIPE_COM *com)
{
	if(com->next)
	{
		// else, if there are piped commands coming, we need to fork and connect the two processes by a pipe
		int newpipe[2];
		pipe(newpipe);
	
		int pid=fork();
		// child executes next command, with input from the pipe output stdout is same
		if(!pid) {
			dup2(newpipe[0], 0); // use pipe for stdin in child
			close(newpipe[0]);
			close(newpipe[1]); 

			//execute_command(com->next->value.Pipe->com);
			execute_command(com->next);
		}	
		// parent executes command, but writes to pipe input. input is the same
		else { 
			dup2(newpipe[1], 1); // use pipe for stdout in parent
			close(newpipe[1]);
			close(newpipe[0]); 

			execute_command(com->com);
		}
	}
	else
		execute_command(com->com);
		
}*/

/* decide number of execution groups and fill up individual execution groups */
void execute_pipe_command(COMMAND *command)
{
	int num_of_grps = find_num_of_execution_grps();
	EXECUTION_GRP grp_array[num_of_grps];
	COMMAND *command1 = command;
	PIPE_COM *pipe = command1->value.Pipe;
	int index = 0;
	
	for(int i=0; i<global_plan->num_of_coms; i++)
	{
		if(i==0)
		{
			grp_array[index].start = command1;
			grp_array[index].num_of_coms = 1;
			grp_array[index].remote = *(global_plan->array[i]);
		}
		else if(*(global_plan->array[i])!=*(global_plan->array[i-1]))
		{
			index++;
			grp_array[index].start = command1;
			grp_array[index].num_of_coms = 1;
			grp_array[index].remote = *(global_plan->array[i]);
		}	
		else
		{
			grp_array[index].num_of_coms++; 
		}
		command1 = pipe->next;
		if(command1)
			pipe = command1->value.Pipe;
	}
	for(int i=0; i<num_of_grps; i++)
	{
		print_execution_grp(&grp_array[i]);
	}
	
	check_remote_threshold_grp_array(&grp_array[0], num_of_grps);

	for(int i=0; i<num_of_grps; i++)
	{
		print_execution_grp(&grp_array[i]);
	}
	execute_grps(&grp_array[0], num_of_grps);
}

/* find the number of execution groups  */
int find_num_of_execution_grps()
{
	int count=1;
	for(int i=1; i<global_plan->num_of_coms; i++)
	{
		if(*(global_plan->array[i])!=*(global_plan->array[i-1]))
			count++;
	}
	return count;
}

/* execute the execution groups, fork a process for each execution group  */
void execute_grps(EXECUTION_GRP* grp_array, int size)
{
	if(size==1)
	{
		execute_single_grp(grp_array);
	}
	else if(size>1)
	{
		for(int i=0; i<size; i++)
		{
		   if(i<size-1)
		   {
			// else, if there are piped commands coming, we need to fork and connect the two processes by a pipe
			int newpipe[2];
			pipe(newpipe);
	
			int pid=fork();
			// child executes next command, with input from the pipe output stdout is same
			if(!pid) {
				dup2(newpipe[0], 0); // use pipe for stdin in child
				close(newpipe[0]);
				close(newpipe[1]); 

				//execute_command(com->next->value.Pipe->com);
				continue;
			}	
			// parent executes command, but writes to pipe input. input is the same
			else { 
				dup2(newpipe[1], 1); // use pipe for stdout in parent
				close(newpipe[1]);
				close(newpipe[0]); 

				execute_single_grp(grp_array+i);
			}
		   }
		   else{
			execute_single_grp(grp_array+i);
		   }
		} 
	}
}

/* execute a single execution group */
void execute_single_grp(EXECUTION_GRP* grp)
{
	if(grp->remote==0)
		execute_single_grp_local(grp);	
	else
		execute_single_grp_remote(grp);
}

/* execute single group locally, if a piped command- fork a new process for each command  */
void execute_single_grp_local(EXECUTION_GRP* grp)
{
	for(int i=0; i<grp->num_of_coms; i++)
	{
	   if(i<grp->num_of_coms-1)
	   {
		// else, if there are piped commands coming, we need to fork and connect the two processes by a pipe
		int newpipe[2];
		pipe(newpipe);
	
		int pid=fork();
		// child executes next command, with input from the pipe output stdout is same
		if(!pid) {
			dup2(newpipe[0], 0); // use pipe for stdin in child
			close(newpipe[0]);
			close(newpipe[1]); 

			continue;
		}	
		// parent executes command, but writes to pipe input. input is the same
		else { 
			dup2(newpipe[1], 1); // use pipe for stdout in parent
			close(newpipe[1]);
			close(newpipe[0]); 

			execute_simple_command_locally(getCommand(grp->start, i)->value.Pipe->com->value.Simple);
		}
	    }
	    else{
		execute_simple_command_locally(getCommand(grp->start, i)->value.Pipe->com->value.Simple);
	     }
	} 	
}

/* execute a single group remotely  */
void execute_single_grp_remote(EXECUTION_GRP* grp)
{
	for(int i=0; i<grp->num_of_coms; i++)
		execute_scp_simple(getCommand(grp->start, i)->value.Pipe->com, grp->remote);

	execute_pipe_command_remotely(grp->start, grp->num_of_coms, grp->remote);
}

/* supply arguments to execve to run the command */
void exec_execve(char **args)
{
	if(strchr(args[0],'/'))
		execve(args[0],args,environ);
	else {
		char *paths = getenv("PATH");
		while(paths) {
			char *path = strsep(&paths,":");		
			char buf[255];
			sprintf(buf,"%s/%s",path,args[0]);
			struct stat s;
			if(stat(buf,&s)==0) {
				execve(buf,args,environ);	
				return;
			}
		}
	}	
}

/* perform redirections for a single command  */
void do_redirections(REDIRECT *redir)
{
	char outfile[255];
	int len=0;
	int outfd=-1;
	int infd=-1;
	while(redir)
	{
		memset(outfile, 0, 255);
		len = strlen(redir->redirectee.file->orgfileword->word);
		strncpy(outfile,redir->redirectee.file->orgfileword->word,len);
		outfile[len] = 0;
		
		switch(redir->instruction)
		{
		  case r_output_direction:
		  {
			outfd = open(outfile,O_RDWR|O_CREAT);
			chmod(outfile,00777);
			if(outfd<0) perror("couldn't open file");
			dup2(outfd, 1);
			break;
		  }
		  	
		  case r_input_direction:
		  {
			infd = open(outfile,O_RDONLY);
			dup2(infd, 0);
			break;
		  }
		}
		redir = redir->next;
	}
}

/*void execute_command_remotely(COMMAND *command)
{
	execute_scp(command);
	switch(command->type)
	{
	  case cm_simple:
	  {
		execute_simple_command_remotely(command);
		//execute_scp_after(command);
		break;
	  }
	  case cm_pipe:
		//execute_pipe_command_remotely(command);
		//execute_scp_after(command);
		break;
						
	}
	

}*/

/*void execute_scp_after(COMMAND *command)
{
	if(command == NULL)
		return;
	switch(command->type)
	{
	  case cm_simple:
	  {
		execute_scp_redirects_output(command->value.Simple->redirects, command);
		break;
	  }
	  case cm_pipe:
	  {
		execute_scp_after(command->value.Pipe->com);
		execute_scp_after(command->value.Pipe->next);
		break;
	  }
	}
}*/

/* execute a simple command remotely */
void execute_simple_command_remotely(COMMAND *command, int host)
{
	char *com_string = (char *)calloc(300, sizeof(char));
	char *buffer = simple_command_to_remote(command->value.Simple);
	sprintf(com_string, "ssh %s %s", global_host_list->host_array[host-1]->server, buffer);
	free(buffer);
	fprintf(stderr, "%s\n", com_string);
	char **args = (char **)calloc(100, sizeof(char));
	com_string_to_argv(com_string, args);
	exec_execve(args);
	dispose_argv(args);
	free(com_string);
}


/* execute a piped command remotely  */
void execute_pipe_command_remotely(COMMAND *command, int size, int host)
{
	char *com_string = (char *)calloc(1100, sizeof(char));
	char *buffer = pipe_command_to_remote(command, size);
	sprintf(com_string, "ssh %s %s", global_host_list->host_array[host-1]->server, buffer);
	free(buffer);
	fprintf(stderr, "%s\n", com_string);
	char **args = (char **)calloc(100, sizeof(char));
	com_string_to_argv(com_string, args);
	exec_execve(args);
	dispose_argv(args);
	free(com_string);
}

/*void execute_scp(COMMAND *command)
{
	if(command == NULL)
		return;
	switch(command->type)
	{
	  case cm_simple:
	  {
		execute_scp_simple(command);
		break;
	  }
	  case cm_pipe:
	  {
		execute_scp(command->value.Pipe->com);
		execute_scp(command->value.Pipe->next);
	  	break;
	  }
	}
}*/

void execute_scp_simple(COMMAND *com, int host)
{
	SIMPLE_COM *simple = com->value.Simple;
	if(simple->files)
		execute_scp_files(simple->files, com, host);
	if(simple->redirects)
		execute_scp_redirects_input(simple->redirects, com, host);
}

void execute_scp_files(FILE_LIST *list, COMMAND *com, int host)
{
	char command[100];
	char *filename;
	while(list)
	{
		if(list->file->host != host)
		{
			memset(command, 0, 100);
			filename = extract_filename(list->file->orgfileword->word);
			if(list->file->host == 0)
				sprintf(command, "scp %s %s:%s/%s", list->file->orgfileword->word, global_host_list->host_array[host-1]->server, ".~", filename);
			else
				sprintf(command, "scp %s:%s/%s %s:%s/%s", list->file->remotef->server, list->file->remotef->directory, list->file->remotef->filename, global_host_list->host_array[host-1]->server, ".~", filename);
			system(command);
			list->file->remotef = make_remote_file_given(global_host_list->host_array[host-1]->server, ".~", filename);
			print_remote_file(list->file->remotef);
			fprintf(stderr, "%s\n", command);
		}
		list = list->next;
	}	
}



void execute_scp_redirects_input(REDIRECT *redir, COMMAND *com, int host)
{
	char command[100];
	char *filename;
	while(redir)
	{
		if(redir->redirectee.file->host != host)
		{
			memset(command, 0, 100);
			filename = extract_filename(redir->redirectee.file->orgfileword->word);
			redir->redirectee.file->remotef = make_remote_file_given(global_host_list->host_array[host-1]->server, ".~", filename);
			if(redir->redirector==0)
			{
				if(redir->redirectee.file->host==0)
					sprintf(command, "scp %s %s:%s/%s", redir->redirectee.file->orgfileword->word, global_host_list->host_array[host-1]->server, ".~", filename);
				else
					sprintf(command, "scp %s:%s/%s %s:%s/%s", redir->redirectee.file->remotef->server, redir->redirectee.file->remotef->directory, redir->redirectee.file->remotef->filename, global_host_list->host_array[host-1]->server, ".~", filename);
				fprintf(stderr, "%s\n", command);
				system(command);
			}
			else if(redir->redirector==1)
			{
				char outfile[255];
				memset(outfile, 0, 255);
				int len = strlen(redir->redirectee.file->orgfileword->word);
				strncpy(outfile,redir->redirectee.file->orgfileword->word,len);
				outfile[len] = 0;
				int outfd;
				outfd = open(outfile,O_RDWR|O_CREAT);
				chmod(outfile,00777);
				if(outfd<0) perror("couldn't open file");	
			}
		}
		redir = redir->next;
	}	
}

void execute_scp_redirects_output(REDIRECT *redir, COMMAND *com, int host)
{
	fprintf(stderr, "inside\n");
	char command[100];
	while(redir)
	{
	   if(redir->redirector==1)
	   {
		fprintf(stderr, "remote: %d\n", redir->redirectee.file->remote);
		if(!redir->redirectee.file->remote)
		{
			memset(command, 0, 100);
			//sprintf(command, "scp %s:%s/%s %s", com->remote->server, com->remote->directory, redir->redirectee.file->remotef->filename, redir->redirectee.file->orgfileword->word);
			sprintf(command, "scp %s:%s/%s %s", global_remote->server, global_remote->directory, redir->redirectee.file->remotef->filename, redir->redirectee.file->orgfileword->word);
			fprintf(stderr, "%s\n", command);
			system(command);
		}
	   }
	   redir = redir->next;
	}	
}

/* create hidden directories on each host so that scp can be performed later if needed */
void create_hidden_directories(HOST_LIST *list)
{
	char command[100];
	for(int i=0; i<list->num_of_hosts; i++)
	{
		memset(command, 0, 100);
		sprintf(command,"ssh %s \"mkdir -p .~\"", list->host_array[i]->server);
		system(command);
	}
}
