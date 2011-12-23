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

#include "common.h"
#include "heredoc.h"
#include "script.h"


void
main(int argc, char* argv[])
{
    if(argc > 2)
    {
        fprint(2, "usage: %s [file]\n", argv[0]);
        exits("usage");
    }

	// Run script from file given as parameter instead of stdin if necesary
	if(argc == 2)
		redirect_stdin(argv[1]);

	// Allocate buffer for standard input
	Biobuf bin;
	if(Binit(&bin, 0, OREAD))
		sysfatal("%s: standard input: %r", argv0);

	tHeredoc heredoc;
	heredoc.mode = 0;

	// Main loop
	char* line;
	while((line = Brdstr(&bin, '\n', 0)))
	{
		// Process heredoc
		line = heredoc_process(&heredoc, line);
		if(line)
		{
		    // Process script
		    script_process(line);

		    // Free procesed line
		    free(line);
		}
	}

	// Free stdin buffer
	Bterm(&bin);

	// Exit sucesfully
	exits(nil);
}
