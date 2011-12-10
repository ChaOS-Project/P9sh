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
run_pipeline(int numCommands, char* array[])
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

				process_command(array[i]);
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
process_pipeline(char* line)
{
	// backup of the original stdin and stdout file descriptors
	int oldStdin  = dup(0, -1);
	int oldStdout = dup(1, -1);

	// apply redirections
	redirections(line);

	// Split pipeline in independent commands
	char* array[10];
	int numCommands = gettokens(line, array, 10, "|");

	// run pipeline
	int ret = run_pipeline(numCommands, array);

	// restore original stdin and stdout file descriptors
	dup(oldStdin,  0);
	dup(oldStdout, 1);

	// return pipeline error value if necessary
	if(ret < 0) return ret;

	// Wait for child process result
	return wait_childrens(numCommands);
}
