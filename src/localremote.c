#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "localremote.h"
#include "debug.h"
#include "shared.h"
#include "copy_cmd.h"

int local_or_remote(char* file)
{
	char *cwd="/home/tejas";
	//getcwd(cwd, sizeof(cwd));
	struct stat *home_stat=malloc(sizeof(struct stat));
	struct stat *statinfo=malloc(sizeof(struct stat));
	stat(cwd, home_stat);
	
	dev_t m1=home_stat->st_dev;
	int home_major_id=major(m1);
	//DEBUG(("home major id:%i", home_major_id));
	int home_minor_id=minor(m1); 
	//DEBUG(("home minor id:%i", home_minor_id));
	
	int k=stat(file, statinfo);
	if(k==0){
		dev_t m=statinfo->st_dev;
		int device_major_id=major(m);
		//DEBUG(("device major id:%i", device_major_id));
		int device_minor_id=minor(m);
		//DEBUG(("device minor id:%i", device_minor_id));
		if(device_major_id==home_major_id && device_minor_id==home_minor_id){
			DEBUG(("\nLocal File\n"));
			free(home_stat);
			free(statinfo);
			return 0;
		}
		else{
			DEBUG(("\nRemote File\n"));
			free(home_stat);
			free(statinfo);
			return 1;
		}
	}
	else{
		free(home_stat);
		free(statinfo);
		return 0;
	}	
}


REMOTE_FILE* convert_to_remote(char *filename, REMOTE_FILE *remote_file)
{
	FILE* fp;
	char result1[2000];
	memset(result1, 0, 2000);
	char *result=result1;
	fp = popen("mount","r");
	fread(result,1,2000,fp);
	pclose (fp);
	//DEBUG(("%s",result));

	char *mount_entry=strsep(&result, "\n");
		
	/*process each mount entry*/
	while(result!=NULL){
		char server[50];
		memset(server, 0, 50);
		char mountpoint[50];
		memset(mountpoint, 0, 50);
		//DEBUG(("\n\nENTRY:\n%s\n", mount_entry));
		int i=0;
		while(i<3){
			char *mount_entry_token=strsep(&mount_entry, " ");

			/*server to ssh into*/
			if(i==0)
				strcpy(server, mount_entry_token);

			/*mountpoint on machine*/
			if(i==2)
				strcpy(mountpoint, mount_entry_token);
			//DEBUG(("ENTRY in mount:\n%s\n", mount_entry_token));
			i++;	
		}
		//DEBUG(("SERVER:\n%s\n", server));
		//DEBUG(("MOUNTPOINT:\n%s\n", mountpoint));

		/*finding the appropriate mount_entry for the filepath*/
		char *filepath=filename;
		char *found=strstr(filepath, mountpoint);
		if(found!=NULL){
			int len1=strlen(mountpoint);
			char *ch=found+len1;
			char c=*ch;
			//DEBUG(("FOUND position + len: %c\n", c));
			//if is an exact directory/file match only then proceed
			if(c=='\0' || c=='/') {
				char colon=':';
				char *dir_found;
				dir_found=strchr(server, colon);
				//DEBUG(("dir_found:%c\n", *dir_found));
				//DEBUG(("dir on remote machine:%s\n", dir_found+1));
				int serverlen=strlen(server);
				//DEBUG(("serverlen-1:%s\n", server+serverlen-1));
				char server1[50];
				char directory[50];
				char filename1[50];
				memset(server1, 0, 50);
				char *server2=server1;
				strcpy(server2, server);
				char *server_name=strsep(&server2, ":");				
				if(*(server+serverlen-1)!=':'){
					remote_file->server=strdup(server_name);
					DEBUG(("server_name:%s\n", remote_file->server));
					char *d=directory;
					strcpy(d, dir_found+1);
					remote_file->directory=strdup(d);
					DEBUG(("dir_name:%s\n", remote_file->directory));
					char *f=filename1;
					if(strcmp(found+len1, "")==0)
						strcpy(f,found+len1);
					else
						strcpy(f,found+len1+1);
					remote_file->filename=strdup(f);
					DEBUG(("file_name:%s\n", f));		
				}
				else{
					remote_file->server=strdup(server_name);
					DEBUG(("server_name:%s\n", remote_file->server));
					char *d=directory;
					strcpy(d, "~");
					remote_file->directory=strdup(d);
					DEBUG(("dir_name:%s\n", remote_file->directory));
					char *f=filename1;
					if(strcmp(found+len1, "")==0)
						strcpy(f,found+len1);
					else
						strcpy(f,found+len1+1);
					remote_file->filename=strdup(f);
					DEBUG(("file_name:%s\n", f));	
				}
			}
		}	
		mount_entry=strsep(&result, "\n");
	}
	return remote_file;	
}

long calculate_size(char *file)
{
	struct stat *statinfo = malloc(sizeof(struct stat));
	int stat_error = stat(file, statinfo);
	long size = 0;
	if(stat_error==0)
		size = statinfo->st_size;
	if(stat_error==0){
		free(statinfo);
		return size;
	}
	free(statinfo);
	return size;
}
