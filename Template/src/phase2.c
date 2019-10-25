#include "../include/phase2.h"
//Phase 2
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
//case insensitive
//path: path of the file containing file paths to be analyzed 
//fdout: where to write the results of the mapping (freq array)
void map(char* path, int fdout){
	//open file at path
	//this file contains a list of file paths to count frequencies
	FILE *file;
	file = fopen(path,"r");

	char buff[100];

	//freq tracks the frequency of words starting with each letter a-z
	//freq[0] is the frequency of words starting with 'a'
	//freq[1] is the frequency of words starting with 'b'
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
		}else{
			printf("Opened %s\n",buff);
		}
		
		while (fgets(buff,100, look) != NULL){
			//buff is a line (word) from the file
			//printf("%s",buff);
			char start = tolower(buff[0]);
			//add to the appropriate counter in freq
			freq[start-97] += 1;
		}
		fclose(look);
		
		

		
	}

	fclose(file);

	//write the freq array to the write end of the pipe
	write(fdout, freq, sizeof(freq));

	return;
}



void reduce(int fds[], int pids[], int count){
	printf("%d processes to wait for\n",count);

	int totals[26];
	for (int i = 0; i < 26; i++){
		totals[i] = 0;
	}

	int num_found = 0;
	int is_found[count];
	for (int i = 0; i<count;i++){
		is_found[i] = 0;
	}

	int read_end_fd = -1;

	int finished_pid;

	//waits for children to finish
	//while((finished_pid = wait(NULL)) > 0);

	

	while((finished_pid = wait(NULL)) > 0 || num_found < count){
		
		if (finished_pid <= 0){
			//all threads done, choose any not previously done
			for (int c = 0; c < count; c++){
				if(!is_found[c]){
					read_end_fd = fds[c];
					is_found[c] = 1;
					num_found++;
					break;
				}
			}
		}else{

			for (int c = 0; c < count; c++){
				if(finished_pid == pids[c]){
					read_end_fd = fds[c];
					is_found[c] = 1;
					num_found++;
					break;
				}else{
				}
			} 
		}
		if(read_end_fd != -1){
			
			int ret[26];
			if (read(read_end_fd, ret, sizeof(ret)) != 0){
				for(int i =0;i<26;i++){
					totals[i] += ret[i];
				}
			}else{
				printf("failed to get result from mapper process.\n");
			}

			close(read_end_fd);
			read_end_fd = -1;
		}else{
			printf("Did not find a result for child process\n");
		}
	}

	FILE *out;
	out = fopen("ReducerResult.txt","w+");
	for(int i =0;i<26;i++){
		fprintf(out,"%c %d\n",i+97,totals[i]);
	}
	fclose(out);

	return;

}

void master(){
	char path[] = "./TestInput";

	//open the directory /MapperInput
	//get number of files in /MapperInput, n
	DIR *dr = opendir( path);
	if(dr == NULL){
		printf("Could not open MapperInput\n" );
		return;
	}

	//10 is max mapper input files
	int fds[10];
	pid_t pids[10];
	for (int i = 0; i < 10; i++){
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


