#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "../include/main.h"
#include "../include/phase1.h"
#include "../include/phase2.h"

int main(int argc, char *argv[]){

	// argument count check
	if (argc < 2) {
		printf("Wrong number of args, expected 1, given %d\n", argc - 1);
		exit(1);
	}

	// Holder for filepath
	char filepath[800];
	DIR *dr = opendir("MapperInput");
 	struct dirent *de;

 	if (dr != NULL) {
 		// Delete MapperInput directory's contents if it exists
 		while ((de = readdir(dr)) != NULL) {
 			if(de->d_type == DT_REG) {
 				strcat(filepath, "MapperInput/");
 				strcat(filepath, de->d_name);
 				remove(filepath);
 				memset(filepath, 0, 800);
 			}	
 		}
		int i = rmdir("MapperInput");
    }

	//just make a function call to code in phase1.c
	//phase1 - Data Partition Phase
	createMapperInput(argv[1]);
	//create pipes
	
	//just make a function call to code in phase2.c
	//phase2 - Map Function
	master();

	//just make a function call to code in phase3.c
	//phase3 - Reduce Function

	//phase4
	//wait for all processes to reach this point
		
	//just make a function call to code in phase4.c
	//master process reports the final output
	
	return 0;

}
