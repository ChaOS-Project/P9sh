/*
 * script.c
 *
 *  Created on: 06/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "builtins.h"
#include "common.h"
#include "environment.h"
#include "pipeline.h"


int
background(char* line)
// Check and exec on background all the commands ended with '&' on the command
// line and return the last, foreground command
{
	// Split pipeline in independent commands
	char* array[10];
	int numCommands = gettokens(line, array, 10, "&");

	// run script commands
	int i;
	for(i = 0; i < numCommands-1; ++i)
	{
		switch(fork())
		{
			case -1:
				return -1;

			case 0:	// child
			{
				// redirect stdin to /dev/null
				redirect_stdin("/dev/null");

				// process pipeline
				process_pipeline(array[i]);
				exitError();
			}

			// parent loop to exec in background the next command (if any)
		}
	}

	// collapse line to let only the last (not background) command on it
	int len = strlen(array[numCommands-1]);
	memmove(line, array[numCommands-1], len);
	*(line+len) = '\0';

	return 0;
}


int
builtin_cd(char* line)
// Addapt `line` data and exec built-in 'change directory' command
{
	// Local copy of `line` to work with and return status (default = false)
	line = strdup(line);
	int ret = 0;

	// Split command line in independent tokens
	char* array[10];
	int numTokens = tokenize(line, array, 10);

	// If it's a `cd` command, call to the built-in function and set ret value
	if(numTokens && !strcmp(array[0], "cd"))
	{
		cd(numTokens, array);

		ret = 1;
	}

	// Free local copy of line and return if current directory was changed
	free(line);
	return ret;
}


void
process_script(char* line)
{
	// Split script in independent pipelines
	char* array[10];
	int numPipelines = gettokens(line, array, 10, ";");

	// run script commands
	int i;
	for(i = 0; i < numPipelines; ++i)
	{
		// expand environment variables and substitute command line char pointer
		// with the one with expanded environment variables (if necesary)
//		char* expanded = expand_envVars(array[i]);
//		if(expanded) array[i] = expanded;
		array[i] = environment_expand(array[i]);

		// move commands to background (if necesary)
		background(array[i]);

		// Exec `cd` (it's a built-in, but must change shell environment itself,
		// so we check and exec for it directly here)
		if(builtin_cd(array[i]))
		{
			free(array[i]);
			continue;
		}

		// Set environment variable
		if(environment_set(array[i]))
		{
			free(array[i]);
			continue;
		}

		// run foreground command
		process_pipeline(array[i]);

		// free line created by environment variables expansion
		free(array[i]);
	}
}
