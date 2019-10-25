#include "../include/phase2.h"
#include "../include/phase3.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

//find the number of words corresponding to each letter of the alphabet from the files listed in the Mapper_i file
//path: path of the file containing file paths to be analyzed 
//fdout: where to write the results of the mapping (freq array)
void map(char* path, int fdout){
	//open file at path which contains a list of file paths
	FILE *file;
	file = fopen(path,"r");

	char buff[100];

	//freq tracks the number of occurances of each letter.
	//freq[0] is the number of a's
	//freq[1] is the number of b's
	//etc
	int freq[26];
	for(int i =0;i<26;i++){
		freq[i]=0;
	}
	

	printf("Input file: %s\n",path);
	while (fgets(buff,100, file) != NULL){
        	//buff is a new file path
		strtok(buff, "\n");
		//open file at the path
		FILE *look;
		look = fopen(buff,"r");		
		
		if(look == NULL){
			perror("Failed to open file");
			return;
		}
		
		while (fgets(buff,100, look) != NULL){
			//buff is a line (word) from the file
			//printf("%s",buff);
			for (int i = 0; i < strlen(buff);i ++ ){
				//add to the appropriate counter in freq
				freq[tolower(buff[i])-97] += 1;
				//printf("%c\n",tolower(buff[0]));
			}
	
		}
		fclose(look);
		
	}

	fclose(file);

	//write the freq array to the write end of the pipe
	write(fdout, freq, sizeof(freq));

	return;
}




//n is the number of mapper_i files in mapper input
void master(){
	n=10;
	char path[] = "./MapperInput";

	//open the directory /MapperInput
	DIR *dr = opendir( path);
	if(dr == NULL){
		printf("Could not open MapperInput\n" );
		return;
	}

	//create array of n file descriptors to pass to reduce
	int fds[n];
	//create array of n pids to pass to reduce
	pid_t pids[n];
	//initialize fds as invalid
	for (int i = 0; i < n; i++){
		fds[i] = -1;
	}

	int c = 0;
	struct dirent *de;
	while ((de = readdir(dr)) != NULL){
		if(strcmp(de->d_name,".") && strcmp(de->d_name,"..") )
		if (de->d_type != DT_DIR){
			//non-directory

			//filename
			printf("%s\n", de->d_name);
			//create a pipe for this mapperinput file
			int fd[2];
			pipe(fd);

			pid_t cpid;
			if ((cpid = fork()) == -1) {
				perror("fork");
				return;
			}if (cpid > 0) {
				//parent
				//close write end of pipe
				close(fd[1]);

				//add read end of pipe to fds
				fds[c] = fd[0];
				//add child pid to pids
				pids[c] = cpid;

				
			}else{
				//child
				//close read end of pipe
				close(fd[0]);		

				char newPath[100];
				strcpy(newPath,path);
				strcat(newPath,"/");
				strcat(newPath,de->d_name);

				//pass path and write end of pipe to map function
				map(newPath, fd[1]);

				
				

				close(fd[1]);
				return;
			}
			c++;
		}
		
	}

	reduce(fds,pids,c);
}		

//No output

//The grading will be carried out based on the per letter word
//count algorithm, pipe setup and usage.


