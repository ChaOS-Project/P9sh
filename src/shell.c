/*
 ============================================================================
 Name        : P9sh.c
 Author      : Jesus Leganes Combarro "Piranna"
 Version     :
 Copyright   : GPL - Copyright 2011
 Description : CLI shell for Plan 9 operating system
 ============================================================================
 */

// This program is currently being refactored to optimice and clean it's code


#include <u.h>
#include <libc.h>

#include <bio.h>

#include "heredoc.h"
#include "script.h"


void
main(int argc, char* argv[])
{
	// Allocate buffer for standard input
	Biobuf*	bin = Bfdopen(0, O_RDONLY);
	if(bin == nil)
		sysfatal("%s: standard input: %r", argv0);

	tHeredoc heredoc;
	heredoc.mode = 0;

	// Main loop
	char* line;
	while(line = Brdstr(bin, '\n', 0))
	{
		// Process heredoc
		if(heredoc_process(&heredoc, line))
			continue;

		// Process script
		process_script(line);

		// Free procesed line
		free(line);
	}

	// Free stdin buffer
	Bterm(bin);

	// Exit sucesfully
	exits(nil);
}
