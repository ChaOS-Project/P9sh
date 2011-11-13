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


int
heredoc_process(tHeredoc* heredoc, char* line)
{
	if(heredoc->mode)
	{
		if(heredoc_end(line))
		{
			free(line);

			int len = strlen(heredoc->command) + strlen(heredoc->buffer) + 9;
			line = malloc(len * sizeof(char));
			strcpy(line, "echo ");

			char* quote = quotestrdup(heredoc->buffer);
			strcat(line, quote);
			free(quote);

			strcat(line, " | ");
			strcat(line, heredoc->command);

			heredoc->mode = 0;
		}

		// Copy line to the heredoc buffer
		else
		{
			strncat(heredoc->buffer, line, strlen(line));
			free(line);
			return 1;
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
			free(line);

			heredoc->buffer[0] = '\0';

			return 1;
		}
	}

	return 0;
}
