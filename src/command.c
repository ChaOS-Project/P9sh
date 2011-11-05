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


//int
//run(char* cmd, char* argv[])
//{
//	Waitmsg*	m;
//	int			ret;
//
//	switch(fork())
//	{
//		case -1:
//			return -1;
//
//		case 0:		// child
//			exec(cmd, argv);
//			sysfatal("exec: %r");
//
//		default:	// parent
//			m = wait();
//
//			if(m->msg[0] == 0)
//				ret = 0;
//
//			else
//			{
//				werrstr(m->msg);
//				ret = -1;
//			}
//
//			free(m);
//			return ret;
//	}
//}


builtin
isBuiltin(char* command)
{
	if(!strcmp(command, "cd"))
		return cd;

	return nil;
}


void
process_command(char* line)
{
	// Create array of arguments from line
	char** array = calloc(10, sizeof(char*));
//	char* array[10];
	tokenize(line, array, 10);

	// run command line
	if(array[0] != nil)
	{
		// Check if command is a built-in function
		builtin command = isBuiltin(array[0]);
		if(command)
			command(array);

		// Not built-in, search command
		else
		{
			// Calc command path
			char path[256];
			strncpy(path, "/bin/",6);
			strncat(path, array[0], strlen(array[0]));

			// run command
//			run(path, array);
			exec(path, array);
			sysfatal("exec: %r");
		}
	}

	// Free array
	free(array);
}
