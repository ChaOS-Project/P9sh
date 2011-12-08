/*
 * heredoc.c
 *
 *  Created on: 13/11/2011
 *      Author: piranna
 */


#include <u.h>
#include <libc.h>

#include "heredoc.h"


char*
heredoc_begin(char* line)
{
	return strrchr(line, '[');
}

int
heredoc_end(char* line)
{
	return line[0] == ']';
}


char*
heredoc_process(tHeredoc* heredoc, char* line)
// Check and eat heredoc lines.
// While processing, it stores them at an internal buffer and return nil,
// otherwise it returns the input and/or processed lines.
{
	// Heredoc is enabled - eat line
	if(heredoc->mode)
	{
		if(heredoc_end(line))
		{
			free(line);

			char* quote = quotestrdup(heredoc->buffer);

			int len = strlen(heredoc->command) + strlen(quote) + 9;
			line = malloc(len * sizeof(char));

			strcpy(line, "echo ");
			strcat(line, quote);
			strcat(line, " | ");
			strcat(line, heredoc->command);

			free(quote);
			heredoc->mode = 0;
		}

		// Copy line to the heredoc buffer
		else
		{
			strncat(heredoc->buffer, line, strlen(line));

			free(line);
			return nil;
		}
	}

	// Enable heredoc mode
	else
	{
		char* pos = heredoc_begin(line);
		if(pos)
		{
			heredoc->mode = 1;

			strncpy(heredoc->command, line, 256);
			heredoc->command[pos-line] = '\0';

			heredoc->buffer[0] = '\0';

			free(line);
			return nil;
		}
	}

	// Heredoc is disabled, return (processed) line
	return line;
}
