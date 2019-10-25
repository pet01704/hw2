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
