%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "command.h"
#include "make_cmd.h"
#include "print_cmd.h"
#include "dispose_cmd.h"
#include "execute_cmd.h"
#include "convert.h"
#include "shared.h"
#include "debug.h"

int yylex(void);
void yyerror(char *); 
ELEMENT make_word_to_file(ELEMENT);
WORD_DESC *copy_word(WORD_DESC *);
ELEMENT check_word_or_file(ELEMENT);
COMMAND *append_word_list(WORD_LIST *list, COMMAND *command);

static REDIRECTEE redir;
static int word_count=0;

%}
%union{
WORD_DESC *word;
WORD_LIST *word_list;
NUMBER *number;
//FILEE *file;
//FILE_LIST *file_list;
REDIRECT *redirect;
ELEMENT element;
COMMAND *command;
}

%token <word> FCMD
%token <word> FSCMD
%token <word> SCMD
%token <word> OPTION
%token <word> WORD
%token <number> yacc_EOF
//%token <word> TIME
%type <redirect> redirection

/*%type <word_list> option_list
%type <word_list> word_list
%type <file_list> file_list*/
//%type <word_list> time
%type <element> simple_command_element fcmd_element fscmd_element scmd_element
%type <command> simple_command simple_file_command simple_file_string_command simple_string_command default_command
%type <command> command pipeline simple_list

%type <command> inputunit

%start inputunit

%%
inputunit:	simple_list	
			{ 
			if(global_command)
				dispose_command(global_command);
			if(global_remote)
				dispose_remote(global_remote);
			global_command = $1;
			global_remote = make_remote();
			global_plan = make_plan(num_of_commands(global_command));
			
			/*dispose_command(global_command);
			dispose_remote(global_remote);
			dispose_plan(global_plan);*/
			//global_remote = make_remote();
			fork_flag =1;
			YYACCEPT;	
			}
	|	simple_list ';'
			{
			if(global_command)
				dispose_command(global_command);
			if(global_remote)
				dispose_remote(global_remote);
			global_command = $1;
			global_remote = make_remote();
			global_plan = make_plan(num_of_commands(global_command));
			//global_remote = make_remote();
			fork_flag =1;
			YYACCEPT;	
			}
	|	simple_list ';''\n'	
			{ 
			if(global_command)
				dispose_command(global_command);
			if(global_remote)
				dispose_remote(global_remote);
			global_command = $1;
			global_remote = make_remote();
			global_plan = make_plan(num_of_commands(global_command));
			//global_remote = make_remote();
			fork_flag = 1;
			YYACCEPT;	
			}
	|	simple_list '\n'	
			{ 
			if(global_command)
				dispose_command(global_command);
			if(global_remote)
				dispose_remote(global_remote);
			global_command = $1;
			global_remote = make_remote();
			global_plan = make_plan(num_of_commands(global_command));
			//global_remote = make_remote();
			fork_flag = 1;
			YYACCEPT;	
			}
	|	'\n'
			{
			/*dispose_command(global_command);
			dispose_remote(global_remote);*/
			fork_flag = 0;
			YYACCEPT;
			}
	|	yacc_EOF
			{
			/*dispose_command(global_command);
			dispose_remote(global_remote);
			dispose_plan(global_plan);*/
			fork_flag=0;
			return 1;
			}	
	;

/*option_list:	 OPTION				
			{$$ = make_word_list($1, (WORD_LIST *)NULL);}
	|	 option_list OPTION		
			{$$ = make_word_list($2, $1);}
	;

word_list:	WORD			
			{$$ = make_word_list($1, (WORD_LIST *)NULL);}
	|	word_list WORD		
			{$$ = make_word_list($2, $1);}
	;
*/

/*
file_list:	WORD			
			{$$ = make_file_list($1, (FILE_LIST *)NULL);}
	|	file_list WORD		
			{$$ = make_file_list($2, $1);}
	;
*/

redirection:	'>' WORD
			{
			  redir.file = make_redir_file($2);
			  $$ = make_redirection(1, r_output_direction, redir);
			}
	|	'<' WORD
			{
			  redir.file = make_redir_file($2);
			  $$ = make_redirection(0, r_input_direction, redir);
			}
	;

simple_command_element:	OPTION
			{ $$.option=$1; $$.word=0; $$.file=0; $$.redirect=0; print_word($1); 
			  if(strcmp($1->word,"-f")==0)
				word_count++;
			}
	|	WORD
			{ $$.word=$1; $$.option=0; $$.file=0; $$.redirect=0; print_word($1); word_count++; }
	| 	redirection
			{ $$.redirect=$1; $$.word=0; $$.option=0; $$.file=0; print_redirection($1);}
	;

fcmd_element:	FCMD
			{ $$.word=$1; $$.file=0; $$.redirect=0; print_word($1); }
	;

fscmd_element:	FSCMD
			{ $$.word=$1; $$.file=0; $$.redirect=0; print_word($1); }
	;

scmd_element:	SCMD
			{ $$.word=$1; $$.file=0; $$.redirect=0; print_word($1); }
	;

simple_file_command:	fcmd_element
			{ $$ = make_simple_command($1, (COMMAND *)NULL); }
			  			
	|	simple_file_command simple_command_element
			{ 
			  if($2.word){
				$2=make_word_to_file($2);
				print_file($2.file);
			  }
			  $$ = make_simple_command($2, $1); 
			}
	;

simple_file_string_command: fscmd_element
			{ $$ = make_simple_command($1, (COMMAND *)NULL); word_count=0;}
	|	simple_file_string_command simple_command_element
			{
			  if(word_count>1){
				$2=make_word_to_file($2);
				print_file($2.file);
			  }
 			  $$ = make_simple_command($2, $1);
			}		
	;

simple_string_command:	scmd_element
			{ $$ = make_simple_command($1, (COMMAND *)NULL); }
	|	simple_string_command simple_command_element
			{ $$ = make_simple_command($2, $1); }
	;

default_command:	simple_command_element
			{ $$ = make_simple_command($1, (COMMAND *)NULL); }
	|	default_command simple_command_element
			{ 
			  if($2.word){
				$2=check_word_or_file($2);
			  }
			  $$ = make_simple_command($2, $1);
			}
	;

simple_command:	simple_file_command
			{ $$ = $1; }
	|	simple_file_string_command
			{ $$ = $1; }
	|	simple_string_command
			{ $$ = $1; }
	|	default_command
			{ $$ = $1; }
	;

command:	simple_command
			{ $$ = $1; }
	;

/*time_command:	time simple_command
			{ $$ = append_word_list($1, $2); }	
	;
*/
newline_list:	
	|	newline_list '\n'
	;

pipeline:	command '|' newline_list command
			{ 
			  $1 = make_bare_pipe_command($1);
			  $$ = make_pipe_command($1, $4);
			}
	|	pipeline '|' command
			{ $$ = make_pipe_command($1, $3);}
	;

simple_list:	command
			{ $$=$1; }
	|	pipeline
			{ $$=$1; }
	;

%%
void yyerror(char *s){
	fprintf(stderr, "error: %s\n", s);
}

ELEMENT make_word_to_file(ELEMENT elem)
{
	elem.file = make_file(copy_word(elem.word));
	elem.word = 0;
	elem.redirect = 0;
	elem.option = 0;
	return elem;
}

WORD_DESC *copy_word(WORD_DESC *w)
{
	WORD_DESC *temp = (WORD_DESC *)calloc(sizeof(WORD_DESC), sizeof(char));
	int len = strlen(w->word)+1;
	temp->word = calloc(len, sizeof(char));
	strcpy(temp->word, w->word);
	dispose_word(w);
	return (temp);	
}

ELEMENT check_word_or_file(ELEMENT elem)
{
	struct stat *statinfo=malloc(sizeof(struct stat));
	int stat_error=stat(elem.word->word, statinfo);
	if(stat_error==0)
	{
		free(statinfo);
		return make_word_to_file(elem);
	}	
	else
	{
		free(statinfo);
		return elem;
	}
}

COMMAND *append_word_list(WORD_LIST *list, COMMAND *command)
{
	WORD_LIST *temp = list;
	while(temp->next)
		temp = temp->next;
	temp->next = command->value.Simple->words;

	command->value.Simple->words = list;
	return (command);
}

