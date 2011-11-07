/*
 * command.c
 *
 *  Created on: 05/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "builtins.h"


typedef void (*builtin)(char* argv[]);


builtin
isBuiltin(char* command)
{
	if(!strcmp(command, "cd"))
		return cd;

	return nil;
}


void
run_command(char* array[])
{
	if(array[0] != nil)
	{
		// Check if command is a built-in function
		builtin command = isBuiltin(array[0]);
		if(command)
		{
			command(array);
			exits(nil);
		}

		// Not built-in, search command
		else
		{
			// Calc command path
			char path[256];
			strncpy(path, "/bin/",6);
			strncat(path, array[0], strlen(array[0]));

			// run command
			exec(path, array);
			sysfatal("exec: %r");
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
	run_command(array);

	// Free array
	free(array);
}
