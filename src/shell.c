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
main(int argc, char* argv[])
{
	Biobuf*	bin;
	Biobuf*	bout;
	char*	line;

	// Allocate buffer for standard input
	bin = Bfdopen(0, O_RDONLY);
	if(bin == nil)
		sysfatal("%s: standard input: %r", argv0);

	// Allocate buffer for standard output
	bout = Bfdopen(1, O_WRONLY);
	if(bout == nil)
		sysfatal("%s: standard output: %r", argv0);

	// Main loop
	for(;;)
	{
		line = Brdstr(bin, '\n', 0);
		if(line)
		{
			Bwrite(bout, line, Blinelen(bin));
			Bflush(bout);

			free(line);
		}
	}

	// Free buffers
	Bterm(bin);
	Bterm(bout);

	// Exit sucesfully
	exits(nil);
}
