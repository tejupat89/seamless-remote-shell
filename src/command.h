#pragma once

#define MAX_CMD_LEN 500
/* Command Types: */
enum command_type { /*cm_for, cm_while, cm_if, cm_connection,*/cm_simple, cm_pipe};

/* redirection types */
enum r_instruction { r_input_direction, r_output_direction};
		    
/*info for one single command*/
typedef struct command {
  enum command_type type;
  union{
	struct simple_com *Simple;
	struct pipe_com *Pipe;
  }value;
  struct cost *cost;
  struct remote *remote;
} COMMAND;

typedef struct remote {
  int exec_remote;
  char *server;
  char *directory;
} REMOTE;

/* A structure which represents a word. */
typedef struct word_desc {
  char *word;		/* Zero terminated string. */
  //int flags;		/* Flags associated with this word. */
} WORD_DESC;

/* A linked list of words. */
typedef struct word_list {
  WORD_DESC *word;
  struct word_list *next;
} WORD_LIST;

/* information for a remote file */
typedef struct remote_file{
  char *server; /* server or host on which file is located */
  char *directory; /* directory name for the file on the host */
  char *filename;  /* filename on the remote host */
} REMOTE_FILE;

/*A structure which represents a file*/
typedef struct filee{
  WORD_DESC *orgfileword; /* filename */
  int remote; /* remote is 0 for local file, and 1 for remote files */
  int host; /* hosts are represented by 0,1,2 and so on..., represents host on which file is located */
  long size; /* size of the file in bytes */
  struct remote_file *remotef; /* remote file structure for remote file, if the file has to be xfd to a remote dir, this is allocated */
} FILEE;

/* A linked list of files. */
typedef struct file_list{
  FILEE *file;
  struct file_list *next;
} FILE_LIST;

/*A structure which represents a redirectee structure(file/file descriptor to redirect the output/input to) */
typedef struct redirectee{
  int dest;
  FILEE *file;
} REDIRECTEE;

/*A structure which represents a redirection*/
typedef struct redirect{
  int redirector; /* for redirecting output thsi is 1, for redirecting input this is 0 */
  enum r_instruction instruction;
  int flags;
  REDIRECTEE redirectee; 
  struct redirect *next; 
}REDIRECT;

/* An element used in parsing. This is an ephemeral construct. */
typedef struct element{
  WORD_DESC *word;
  WORD_DESC *option;
  FILEE *file;
  REDIRECT *redirect;
  //FILE_LIST *files;
  //WORD_LIST *words;
} ELEMENT;

/* A structure which represents simple_command */
typedef struct simple_com{
  WORD_LIST *words;
  FILE_LIST *files;
  REDIRECT *redirects;
} SIMPLE_COM;

/* A structure which represents pipe_command */
typedef  struct pipe_com{
  COMMAND *com;
  COMMAND *next;
  COMMAND *prev;
} PIPE_COM;

typedef struct number{
  int number;
} NUMBER;

/* A structure which represents plan for execution */
typedef struct plan{
  int** array;
  int num_of_coms;
  long total_cost;
} PLAN;

/* A structure that maintains the cost information for each command involved in the operation */
typedef struct cost{
	long **input; // input cost from each host
	long output; // output cost for the command
	long **redir_output; // redirection output cost for each host
	long piped_input; // piped input received from previous command in the operation
	long **exec_cost; // cost of executing a command on each host
} COST;
 
/* A structure that represents which commands are executed on which hosts */
typedef struct execution_grp{
	COMMAND *start; // command from where the group begins
	int num_of_coms; // number of commands in the group, starting from command pointed by start
	int remote; // the host on which the group is executed
} EXECUTION_GRP;

/* A structure that stores information from cmd_stats.txt */
typedef struct cmd_info{
	char *cmd;
	int output;
	int speed;
} CMD_INFO;

/* A structure that represents each individual host */
typedef struct host{
	int host_num; // integer that reresents the host
	char *server; // server name of the host
} HOST;

/* A structure that represents the list of all hosts involved in the operation */
typedef struct host_list{
	int num_of_hosts; 
	struct host **host_array;
} HOST_LIST;

/* A structure that represents the execution cost on each host */
typedef struct exec_cost{
	struct host_cost** execution; 
} EXEC_COST;

/* A structure that represents execution cost on one single host */
typedef struct host_cost{
	int host;
	long cost;
} HOST_COST;
