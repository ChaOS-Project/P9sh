/*
 * script.c
 *
 *  Created on: 06/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "pipeline.h"


int
background(char* line)
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

			case 0:		// child
			{
				// redirect stdin to /dev/null
				int fd = open("/dev/null", OREAD);
				dup(fd, 0);
				close(fd);

				// process pipeline
				process_pipeline(array[i]);
				exits(nil);
			}
		}
	}

	// collapse line
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
