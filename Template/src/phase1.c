#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#include "../include/phase1.h"
// max number of filepaths that can be added to mapper_m.txt
int max = 3;
// counter for number of files in mapper_m.txt
int filecount = 0;
void traverse(char* path) {

	int ps = 17000*sizeof(path);
	char cwd[ps];
	printf("ps %d\n", ps);
	DIR *dr = opendir(path); 

	// open directory check
	if (dr == NULL) {		
 		printf("Could not open current directory\n" );
		exit(0);
 	}

	struct dirent *de;
 	struct stat buf;
 	int i;

 	while ((de = readdir(dr)) != NULL) {
 		char str[ps];
 		strcpy(str, path);
  		char mappername[ps];
 	    char result[ps];

 		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..") || !strcmp(de->d_name, ".DS_Store") ) {
 			// ignore . and .. 
 		} else {
 			 // Get entry's information and check for error
   			if (stat(path, &buf) == -1) {
        		continue;
        	}
        	// Concatenate file name to str to get path of potential subdirectory 
 			strcat(str,"/");
 			strcat(str, de->d_name);

 			// Check type (directory or file)
 			if (de->d_type == DT_DIR) {
 				// recursion happens if opendir is successful
 				if (opendir(str) != NULL) {	
 				 	printf("File path: %s\n", str);	
 					printf("Directory Name: %s\n", de->d_name);
 					printf("Calling recursion subdirectory\n" );
 					traverse(str);
 				}
 			} else {
 				printf("\nFile Name: %s\n", de->d_name);
 				// create a new 
 				if(i == 0 || i == max) {
 					printf("creating new mapper file\n");
 					// clear previous mapper name
 					memset(result, 0, ps);
 					memset(mappername, 0, 800);
 					// create a mapper file 
       				strcat(mappername, getcwd(cwd, sizeof(cwd)));
 					strcat(mappername, "/MapperInput/Mapper_");
 					sprintf(result, "%d", filecount); 
 					strcat(mappername, result);
 					strcat(mappername, ".txt");
 					i = 0;
 					filecount++;
 				}
 				    printf("File path: %s\n", str);
 				    strcat(path, "\n");
 				    printf("mapername %s\n", mappername);
 				    int fd = open(mappername, O_APPEND|O_RDWR|O_CREAT, 0666);
 				 	write(fd, str, strlen(str));
 					i++;
 			}
  		}
 	}
 	closedir(dr);
}

void createMapperInput(char* path) {
	// Create 'MapperInput' folder
	int mapperInput = mkdir("MapperInput", 0750);
 	// Check if folder was created
	if(mapperInput == -1) {
		printf("Directory creation was unsuccessful");
		exit(1);
	} else {
		traverse(path);
	}
}










