#include <stdio.h>
#include <string.h>

extern int client_main(int argc, char** argv);
extern int server_main(int argc, char** argv);

int main(int argc, char **argv){
	if(argc == 1 || strcmp(argv[1], "-c") == 0){
		client_main(argc, argv);
	} else if (strcmp(argv[1], "-s") == 0) {
		server_main(argc, argv);
	} else {
		printf("Usage:\n");
		printf("If client, type 'socketNetwork' or 'socketNetwork -c'\n");
		printf("If server, type 'socketNetwork -s'\n");
	}
}
