/* Liam Downs, Ronald Elkins, Joshua Farren
 * 11/29/2020
 */

#define LINE_LEN 80
#define MAX_ARGS 64
#define MAX_ARG_LEN 16
#define MAX_PATHS 64
#define MAX_PATH_LEN 96
#define WHITESPACE " .,\t\n"

#ifndef NULL
#define NULL ...
#endif

struct command_t {
	char *name;
	int argc;
	char *argv[MAX_ARGS];
};