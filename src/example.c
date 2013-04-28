
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include "command.h"
#include "make_cmd.h"
#include "print_cmd.h"
#include "dispose_cmd.h"
#include "execute_cmd.h"
#include "convert.h"
#include "debug.h"
#include "shared.h"

COMMAND *global_command = (COMMAND *)NULL;
REMOTE *global_remote = (REMOTE *)NULL;
PLAN *global_plan = (PLAN *)NULL;
HOST_LIST *global_host_list = (HOST_LIST *)NULL;
CMD_INFO **cmd_info_array = (CMD_INFO **)NULL;
int fork_flag = 0;
int script_flag = 0;
int remote_flag = 1;
extern FILE *yyin;
extern FILE *yyout;
extern int yyparse(void);

char *prompt() {
	static char prompt[102];
	char pwd[100];
	getcwd(pwd,100);
	sprintf(prompt,"%s/>>> ",pwd);
	return prompt;
}

int builtinCommands(char *line) {
	if(strcmp("exit",line)==0) {
		exit(0);
		return 1;
	}
	else if(strncmp("cd ",line,3)==0) {
		chdir(line+3);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv){
	//char *str = "1";
	/*char *str ="cat /home/tejas/remote_server2/debug.h";
	yy_scan_string(str);
	yyparse();
	fill_host_list(global_command);
	print_command(global_command);
	dispose_command(global_command);
	yylex_destroy();
	return 0;*/
	cmd_info_array = make_cmd_info_array(NUM_OF_CMDS);
	load_cmd_info_array(cmd_info_array, NUM_OF_CMDS);
	if(argc>1) {
		yyin = fopen(argv[1], "r");
		while(!yyparse())
		{
			if(fork_flag){
				int stat_loc;						
					int pid = fork();
				if(!pid)
				{
					print_command(global_command);
					//if(global_command)
						//execute_command(global_command);
					
				}
				else{
					waitpid(pid,&stat_loc,0);
				}
			}
		}
		fclose(yyin);
	}
	else
	{
		while(1){
			char *line = readline(prompt());
			if(line && *line) add_history(line);
			if(!line) {
				return 0;
			}
			if(line[0]=='#') continue;

			if(!builtinCommands(line) && strlen(line)>0){

				int stat_loc;						
				int pid = fork();
				if(!pid)
				{
					yy_scan_string(line);
					yyparse();
					//planner(global_command);
					pre_processing(global_command);
					execute_command(global_command);
					//print_plan(global_plan);
					//print_command(global_command);
					//print_remote(global_remote);
					//execute_command(global_command);
					clean_up_hidden_directories(global_host_list);
					dispose_command(global_command);
					dispose_remote(global_remote);
					dispose_host_list(global_host_list);
					//dispose_plan(global_plan);
					//yylex_destroy();
				}
				else{
					DEBUG(("waiting\n"));
					waitpid(pid,&stat_loc,0);
				}
			}
			free(line);
		}
	}
	dispose_cmd_info_array(cmd_info_array, NUM_OF_CMDS);
}

