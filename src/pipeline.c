/*
 * pipeline.c
 *
 *  Created on: 06/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "command.h"


int
process_pipeline(char* line)
{
	// Split pipeline in independent commands
	char** array = calloc(10, sizeof(char*));
//	char* array[10];
	int numCommands = gettokens(line, array, 10, "|");

	// run pipeline
	int piped = 0;

	int i;
	for(i = 0; i < numCommands; ++i)
	{
		int	fd[2];

		pipe(fd);
		switch(fork())
		{
			case -1:
				return -1;

			case 0:		// child
				// Redirect stdin
				dup(fd[0],0);
				close(fd[0]);

//				// Redirect stdout
				dup(fd[1],1);
				close(fd[1]);

				process_command(array[i]);

			default:	// parent
				// Redirect previous stdout as child stdin
				dup(piped,fd[1]);
				if(piped)
					close(piped);

//				// Redirect child stdout to stdin
//				dup(fd[0],1);
//				dup(fd[0],pipedStdout);
				piped = fd[0];
//				close(fd[0]);
		}
	}

//	dup(pipedStdout,1);

	// Free array
	free(array);

	return 0;
}
