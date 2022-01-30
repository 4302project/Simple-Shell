/* Liam Downs, Ronald Elkins, Joshua Farren
 * 11/29/2020
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "minishell.h"

char *lookupPath(char **, char **);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
void printPrompt();
void readCommand (char *);

int main() {
	
/* Shell initialization */

/* Get directory paths from PATH */
	char *pathv[MAX_ARGS];
	parsePath(pathv);

	while (1) {
		
		printPrompt();
		
	/* Read the command line and parse it */
		char commandLine[LINE_LEN];
		readCommand(commandLine);
		
	/* Checks for exit command to terminate */
		if(strcmp(commandLine, "exit\n") == 0){
			break;
		}
		struct command_t command;
		parseCommand(commandLine, &command);
		
	/* Get the full pathname for the file */
		command.name = lookupPath(command.argv, pathv);
		if(command.name == NULL) {
			/* Report error */
			continue;
		}
		
	/* Solves error with command.name not retaining it's value within child process
	 * command.name is copied to temporary variable name using strcpy() 
	 */
		char *name = (char *) malloc(MAX_PATH_LEN);
		strcpy(name, command.name);
		
	/* Create child and execute the command */
		int pid;
		if((pid = fork()) < 0) {
			perror("Error with child process");
			exit(1);
		}		
		else if(pid == 0) {
			execv(name, command.argv);
			exit(0);
		}
	/* Wait for the child to terminate */
		else {
			wait(0);
		}
		
	}

/* Shell termination */
	exit(0);
	
}

/* Builds the promptString to have the machine name and directory */
void printPrompt() {
	
	char hostname[255];
	char directory[MAX_PATH_LEN];
	
	gethostname(hostname, 255);
	getcwd(directory, MAX_PATH_LEN);

	char promptString[255 + MAX_PATH_LEN];
	sprintf(promptString, "[%s %s]$", hostname, directory);
	printf("%s ", promptString);
	
}

void readCommand(char *buffer) {
	
/* Reads input command line into buffer */
	fgets(buffer, LINE_LEN, stdin);
	
}

/* This function will build argv[] and set the argc value.
 * argc is the number of "tokens" or words on the command line
 * argv[] is an array of strings (pointers to char*). The last
 * element in argv[] must be NULL. As we scan the command line
 * from the left, the first token goes in argv[0] , the second in
 * argv[l], and so on. Each time we add a token to argv[],
 * we increment argc.
 */
int parseCommand(char *cLine, struct command_t *cmd) {
	int argc;
	char **clPtr;
	
/* Initialize, cLine is the command line */
	clPtr = &cLine;
	argc = 0;
	cmd->argv[argc] = (char *) malloc(MAX_ARG_LEN);
	
/* Fill argv[] */
	while((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
		cmd->argv[++argc] = (char *) malloc(MAX_ARG_LEN);
	}
	
/* Sets last allocated index to NULL */
	cmd->argc = argc-1;
	cmd->argv[cmd->argc] = NULL;
	return 1;
}

/* This function reads the PATH variable for this
 * environment, then builds an array, dirs[], of the
 * directories in PATH
 */
int parsePath(char *dirs[]) {
	
	char *pathEnvVar;
	char *thePath;
	
	int i;
	for(i=0; i<MAX_ARGS; i++) {
		dirs[i] = NULL;
	}
	pathEnvVar = (char *) getenv("PATH");
	thePath = (char *) malloc(strlen(pathEnvVar) + 1);
	strcpy(thePath, pathEnvVar);
	
/* Loop to parse thePath. Look for a ':'
 * delimiter between each path name.
 */
	char *token;
	token = strtok(thePath, ":");
	
	i = 0;
	while(token != NULL) {
		dirs[i++] = token;
		token = strtok(NULL, ":");
   }

}

/* This function searches the directories identified by the dir
 * argument to see if argv[0] (the file name) appears there,
 * then returns a string of that path.
 */
char *lookupPath(char **argv, char **dir) {
	
	char *result;
	char pName[MAX_PATH_LEN];
	
/* Check to see if file name is already an absolute path name */
	if(*argv[0] =='/') {
		return argv[0];
	}
	
/* Look in PATH directories, access() checks if the file is in a dir. */
	int i;
	for(i = 0; i < MAX_PATHS; i++) {
		sprintf(pName, "%s/%s", dir[i], argv[0]);
		if (access(pName, X_OK) != -1) {
			result = pName;
			return result;
		}
	}
	
/* File name not found in any path variable */
	fprintf(stderr, "%s: command not found\n", argv[0]);
	return NULL;
	
}
