/*
 * command.c
 *
 *  Created on: 05/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "builtins.h"


typedef void (*builtin)(int argc, char* argv[]);


builtin
getBuiltin(char* command)
// Return a pointer to the function corresponding to the built-in `command`.
// If `command` doesn't exists return nil.
{
	if(!strcmp(command, "cd"))
		return cd;

	return nil;
}

char*
getPath(char* command)
{
	char* env = getenv("PATH");
	char* array[10];
	int numTokens = gettokens(env, array, 10, "\t\r\n :");

	char* path = calloc(256, sizeof(char));

	int i;
	for(i = 0; i < numTokens; ++i)
	{
		// Calc command path
		strncpy(path, array[i],256);
		strncat(path, "/", 1);
		strncat(path, command, strlen(command));

		// If command exists at dir, return it
		Dir* d = dirstat(path);
		free(d);
		if(d)	// We are only interested on the pointer, not on it's content...
			break;
	}

	free(env);
	return path;
}


void
run_command(int argc, char* argv[])
{
	if(argv[0] != nil)
	{
		// Check if command is a built-in function
		builtin command = getBuiltin(argv[0]);
		if(command)
		{
			command(argc, argv);
			exits(nil);
		}

		// Not built-in, search command
		else
		{
			// Calc command path
			char* path = getPath(argv[0]);

			// run command
			exec(path, argv);
			sysfatal("%s: %r", argv[0]);
		}
	}
}


void
process_command(char* line)
{
	// Create array of arguments from line
	char** array = calloc(10, sizeof(char*));
//	char* array[10];
	int numTokens = tokenize(line, array, 10);

	// run command line
	run_command(numTokens, array);

	// Free array
	free(array);
}
