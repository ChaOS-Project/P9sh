/*
 * script.c
 *
 *  Created on: 06/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

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

				// check and set error string (if any)
				char err[ERRMAX];
				uint nerr;
				rerrstr(err, nerr);
				exits(err);
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

		// run foreground command
		switch(fork())
		{
			case -1:
				return -1;

			case 0:		// child
			{
				// process pipeline
				process_pipeline(array[i]);

				// check and set error string (if any)
				char err[ERRMAX];
				uint nerr;
				rerrstr(err, nerr);
				exits(err);
			}

			default:	// parent
			{
				Waitmsg* m = wait();

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
	}

	return 0;
}
