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


void
init(Biobuf** bin, Biobuf** bout)
{
	// Allocate buffer for standard input
	*bin = Bfdopen(0, O_RDONLY);
	if(*bin == nil)
		sysfatal("%s: standard input: %r", argv0);

	// Allocate buffer for standard output
	*bout = Bfdopen(1, O_WRONLY);
	if(*bout == nil)
		sysfatal("%s: standard output: %r", argv0);
}

void
deinit(Biobuf** bin, Biobuf** bout)
{
	// Free buffers
	Bterm(*bin);
	Bterm(*bout);
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


void
main(int argc, char* argv[])
{
	Biobuf*	bin;
	Biobuf*	bout;

	// Init buffers
	init(&bin, &bout);

	// Main loop
	for(;;)
	{
		char* line = Brdstr(bin, '\n', 0);
		if(line)
		{
			Bwrite(bout, line, Blinelen(bin));
			Bflush(bout);

			// Create array of arguments from line
			char** array = malloc(10 * sizeof(char*));
			tokenize(line, array, 10);

			// run command
			run("/bin/ls", array);

			// Free line and array
			free(line);
			free(array);
		}
	}

	// Free buffers
	deinit(&bin, &bout);

	// Exit sucesfully
	exits(nil);
}
