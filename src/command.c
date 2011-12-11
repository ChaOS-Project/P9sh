/*
 * command.c
 *
 *  Created on: 05/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "builtins.h"

#ifdef unix
	#define ENV_PATH "PATH"
	#define PATH_TOKEN_SPLIT ":"
#else
	#define ENV_PATH "path"
	#define PATH_TOKEN_SPLIT "\t\r\n "
#endif


// Typedef with the C-main function sign to be used for the built-in functions
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
// Search for `command` on the different directories specified on the path and
// return the executable full path.
// If command is not found, it returns the last processed full path
{
	char* env = getenv(ENV_PATH);

	char* array[10];
	int numTokens = gettokens(env, array, 10, PATH_TOKEN_SPLIT);

	char* path = calloc(256, sizeof(char));

	int i;
	for(i = 0; i < numTokens; ++i)
	{
		// Calc command path
		strncpy(path, array[i],256);
		strncat(path, "/",     256);
		strncat(path, command, 256);

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
command_run(int argc, char* argv[])
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
command_process(char* command)
{
	// Create array of arguments from line
	char** array = calloc(1000, sizeof(char*));
//	char* array[10];
	int numTokens = tokenize(command, array, 1000);

	// run command line
	command_run(numTokens, array);

	// Free array
	free(array);
}
