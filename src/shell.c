/*
 ============================================================================
 Name        : P9sh.c
 Author      : Jesus Leganes Combarro "Piranna"
 Version     :
 Copyright   : GPL - Copyright 2011
 Description : CLI shell for Plan 9 operating system
 ============================================================================
 */

#include <u.h>
#include <libc.h>

#include <bio.h>

#include "builtins.h"


void
init(Biobuf** bin)
{
	// Allocate buffer for standard input
	*bin = Bfdopen(0, O_RDONLY);
	if(*bin == nil)
		sysfatal("%s: standard input: %r", argv0);
}

void
deinit(Biobuf** bin)
{
	// Free buffers
	Bterm(*bin);
}


int
run(char* cmd, char* argv[])
{
	Waitmsg*	m;
	int			ret;

	switch(fork())
	{
		case -1:
			return -1;

		case 0:		// child
			exec(cmd, argv);
			sysfatal("exec: %r");

		default:	// parent
			m = wait();

			if(m->msg[0] == 0)
				ret = 0;

			else
			{
				werrstr(m->msg);
				ret = -1;
			}

			free(m);
			return ret;
	}
}

typedef void (*builtin)(char* argv[]);

builtin
isBuiltin(char* command)
{
	if(!strcmp(command, "cd"))
		return cd;
	return nil;
}


void
process(char* line)
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
			run(path, array);
		}
	}

	// Free array
	free(array);
}


void
main(int argc, char* argv[])
{
	Biobuf*	bin;

	// Init buffers
	init(&bin);

	// Main loop
	for(;;)
	{
		char* line = Brdstr(bin, '\n', 0);
		if(line)
			process(line);
		free(line);
	}

	// Free buffers
	deinit(&bin);

	// Exit sucesfully
	exits(nil);
}
