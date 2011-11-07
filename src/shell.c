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

#include "script.h"


void
main(int argc, char* argv[])
{
	// Allocate buffer for standard input
	Biobuf*	bin = Bfdopen(0, O_RDONLY);
	if(bin == nil)
		sysfatal("%s: standard input: %r", argv0);

	// Main loop
	char* line;
	while(line = Brdstr(bin, '\n', 0))
	{
		process_script(line);
		free(line);
	}

	// Free stdin buffer
	Bterm(bin);

	// Exit sucesfully
	exits(nil);
}
