/*
 * pipeline.c
 *
 *  Created on: 06/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "command.h"
#include "environment.h"
#include "redirections.h"


int
pipeline_run(int numCommands, char* array[])
// Redirect the different commands stdin & stdout on `array` between them
// except the last stdout (so it goes to 'standard' shell stdout) on different
// child process and exec them
{
	int i = 0;
	for(; i < numCommands; ++i)
	{
		int	fd[2];
		pipe(fd);

		switch(fork())
		{
			case -1:
				return -1;

			case 0:		// child
				close(fd[0]);
				if(i < numCommands-1)
					dup(fd[1],1);
				close(fd[1]);

				command_process(array[i]);
				exits(nil);

			default:	// parent
				close(fd[1]);
				if(i < numCommands-1)
					dup(fd[0],0);
				close(fd[0]);
		}
	}

	return 0;
}

int
wait_childrens(int numCommands)
// Wait for the different child process to finish.
// If one of them finish with an error, stop waiting and return the error
{
	int i = 0;
	for(; i < numCommands; ++i)
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

	return 0;
}


int
pipeline_process(char* pipeline)
{
	// backup of the original stdin and stdout file descriptors
	int oldStdin  = dup(0, -1);
	int oldStdout = dup(1, -1);

	// apply redirections
	redirections(pipeline);

	// Split pipeline in independent commands
	char* array[10];
	int numCommands = gettokens(pipeline, array, 10, "|");

	// run pipeline
	int ret = pipeline_run(numCommands, array);

	// restore original stdin and stdout file descriptors
	dup(oldStdin,  0);
	dup(oldStdout, 1);

	// return pipeline error value if necessary
	if(ret < 0) return ret;

	// Wait for child process result
	return wait_childrens(numCommands);
}
