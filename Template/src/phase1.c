#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "../include/phase1.h"
// max number of filepaths that can be added to mapper_m.txt
int max;
// counter for number of files in mapper_m.txt
int m = 0;
float filecount = 0;
// mappercount argument passed by user
float mc = 0;
// number of filepaths inside a MapperInput_m.txt file
int i = 0;
int k=0;
// Counts the number of files in the entire root directory
void countFiles(char* path) {
	DIR *dr = opendir(path);
	// open directory check
	if (dr == NULL) {
		printf("Invalid directory: Open Failed\n" );
		exit(0);
	}

	struct dirent *de;
	struct stat buf;

	while ((de = readdir(dr)) != NULL) {
		char str[800];
		strcpy(str, path);

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
					countFiles(str);
				}
			} else {
				filecount++;
			}
		}
	}
}

// Traverses the directory and its subdirectory
void traverse(char* path) {
	int ps = MAX_SIZE;
	char cwd[100];
	DIR *dr = opendir(path);

	// open directory check
	if (dr == NULL) {
		printf("Invalid directory: Open Failed\n" );
		exit(0);
	}

	struct dirent *de;
	struct stat buf;

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
				// Recurse if opendir is successful
				if (opendir(str) != NULL) {
					traverse(str);
				}
			} else {
				if(i == 0 || i == max) {
					// clear previous mapper name
					memset(result, 0, ps);
					memset(mappername, 0, ps);
					// create a mapper file with approriate path
					strcat(mappername, getcwd(cwd, sizeof(cwd)));
					strcat(mappername, "/MapperInput/Mapper_");

					if(m >= mc) {
						sprintf(result, "%d", m%(int)mc);
					} else {
						sprintf(result, "%d", m);
					}
					strcat(mappername, result);
					strcat(mappername, ".txt");
					i = 0;
					m++;
			  }

				strcat(str, "\n");
				// create a MapperInput_m.txt file
				FILE *fp;
				fp = fopen(mappername, "a");
				// write filepath to the file
				fprintf(fp,"%s", str);
				fclose(fp);
			//	printf("write result: %ld\n", );
				i++;
			}
		}
	}
	closedir(dr);
}

// Creates the MapperInput directory and calls traverse
void createMapperInput(char* path, int mappercount) {
	// Create 'MapperInput' folder
	int mapperInput = mkdir("MapperInput", 0777);
	mc = mappercount;
	// Check if folder was created
	if(mapperInput == -1) {
		printf("Directory creation was unsuccessful");
		exit(1);
	} else {
		countFiles(path);
		max = (filecount/mc)+0.5;
		traverse(path);
	}
}
