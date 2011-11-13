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
				exits(nil);
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
{
	line = strdup(line);

	char* array[10];
	int numTokens = tokenize(line, array, 10);

	if(!strcmp(array[0], "cd"))
	{
		cd(numTokens, array);

		free(line);
		return 1;
	}

	free(line);
	return 0;
}


int
process_script(char* line)
{
	// Split script in independent pipelines
	char* array[10];
	int numPipelines = gettokens(line, array, 10, ";");

	// run script commands
	int i;
	for(i = 0; i < numPipelines; ++i)
	{
		// move commands to background (if necesary)
		background(array[i]);

		// Exec `cd` (it's a built-in, but must change shell environment itself,
		// so we check and exec for it directly here)
		if(builtin_cd(array[i]))
			continue;

		// run foreground command
		Waitmsg*	m;

		switch(fork())
		{
			case -1:
				return -1;

			case 0:		// child
				process_pipeline(array[i]);
				exits(nil);

			default:	// parent
				m = wait();

				if(m->msg[0] == 0)
					free(m);

				else
				{
					werrstr(m->msg);
					free(m);
					return -1;
				}
		}
	}

	return 0;
}
