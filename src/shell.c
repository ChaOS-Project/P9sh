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

	int heredoc_mode = 0;
	char heredoc_command[256];
	char heredoc_buffer[1024];

	// Main loop
	char* line;
	while(line = Brdstr(bin, '\n', 0))
	{
		// Process heredoc
		if(heredoc_mode)
		{
			if(heredoc_end(line))
			{
				free(line);

				int len = strlen(heredoc_command) + strlen(heredoc_buffer) + 9;
				line = malloc(len * sizeof(char));
				strcpy(line, "echo ");

				char* quote = quotestrdup(heredoc_buffer);
				strcat(line, quote);
				free(quote);

				strcat(line, " | ");
				strcat(line, heredoc_command);

				heredoc_mode = 0;
			}

			// Copy line to the heredoc buffer
			else
			{
				strncat(heredoc_buffer, line, strlen(line));
				free(line);
				continue;
			}
		}

		// Enable heredoc mode
		else
		{
			char* pos = heredoc_begin(line);
			if(pos)
			{
				heredoc_mode = 1;

				strncpy(heredoc_command, line, 256);
				heredoc_command[pos-line] = '\0';
				free(line);

				heredoc_buffer[0] = '\0';

				continue;
			}
		}

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
