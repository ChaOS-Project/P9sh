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
{
//	if(!strcmp(command, "cd"))
//		return cd;

	return nil;
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
			char path[256];

			// Calc command path for current dir
			strncpy(path, "./",6);
			strncat(path, argv[0], strlen(argv[0]));

			// If command doesn't exists at current dir,
			// calc command path on bin dir
			Dir* d = dirstat(path);
			if(d == nil)
			{
				strncpy(path, "/bin/",6);
				strncat(path, argv[0], strlen(argv[0]));
			}
			free(d);

			// run command
			exec(path, argv);
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
	run_command(numTokens, array);

	// Free array
	free(array);
}
