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
// counter for number of files in mapper_m.txt
int filecount = 0;
// max number of filepaths that can be added to mapper_m.txt
int max = 2;

void traverse(char* path) {
	//2) Traverse the 'Sample' folder hierarchy and insert the text file paths
	// to Mapper_i.txt in a load balanced manner
	//opens current directory, argument-pathname (char*)
	
	DIR *dr = opendir(path); 

	// opendir returns NULL if couldn't open directory
	if (dr == NULL) {		
 		printf("Could not open current directory\n" );
		exit(0);
 	}

	struct dirent *de;
 	struct stat buf;
 	int i;

 	while ((de = readdir(dr)) != NULL) {
 		char str[800];
 		strcpy(str, path);
  		char mappername[800];
 	    char result[80];
 	    char filecontents[800];

 		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) {
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
 					printf("Directory Name: %s\n", de->d_name);
 					printf("Calling recursion subdirectory\n" );
 					traverse(str);
 				}
 			} else {
 				printf("File Name: %s\n\n", de->d_name);
 				// create a new 
 				if(i == 0 || i == 2) {
 					printf("creating new mapper file\n");
 					// clear previous mapper name
 					memset(result, 0, 80);
 					memset(mappername, 0, 800);
 					memset(filecontents, 0, 800);
 				 	// create a mapper file 
 					strcat(mappername, "Mapper_");
 					sprintf(result, "%d", filecount); 
 					strcat(mappername, result);
 					strcat(mappername, ".txt");
 					i = 0;
 				}
 				    strcat(filecontents,str);
 				    strcat(filecontents, "\n");
 				 	int fd = open(mappername, O_RDWR|O_CREAT, 0666);
 					write(fd, filecontents, strlen(filecontents));
 					i++;
 					filecount++;
 			}

  		}
 	}
 	closedir(dr);
}

//	3) 	Ensure to keep track of the number of text files for empty folder condition 

void createMapperInput(char* path) {
	//1) Create 'MapperInput' folder
	int mapperInput = mkdir("MapperInput", 0700);

 	// Check if folder was created
	if(mapperInput) {
		printf("Directory creation was unsuccessful");
		exit(1);
	} else {
		traverse(path);
	}
}










