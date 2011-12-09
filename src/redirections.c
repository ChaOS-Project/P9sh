/*
 * file_redirections.c
 *
 *  Created on: 07/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "common.h"
#include "environment.h"


void
redirections_environment(char* array[], int i, int* len)
{
	// '%' is alone in the entry, remove it and get the next one
	if(strlen(array[i]) == 1)
		remove_entry(array, i, len);

	// entry has the file in it, set the pointer to it
	else
		array[i]++;

	// Put index at previous token
	--i;

	// Redirect stdout to file
	redirect_environment(array[i]);

	// Remove redirection from command parameters
	remove_entry(array, i, len);
}

void
redirections_stdin(char* array[], int i, int* len)
{
	// '<' is alone in the entry, remove it and get the next one
	if(strlen(array[i]) == 1)
		remove_entry(array, i, len);

	// entry has the file in it, set the pointer to it
	else
		array[i]++;

	// Redirect stdin to file
	redirect_stdin(array[i]);

	// Remove redirection from command parameters
	remove_entry(array, i, len);
}

void
redirections_stdout(char* array[], int i, int* len)
{
	// '>' is alone in the entry, remove it and get the next one
	if(strlen(array[i]) == 1)
		remove_entry(array, i, len);

	// entry has the file in it, set the pointer to it
	else
		array[i]++;

	// Redirect stdout to file
	redirect_stdout(array[i]);

	// Remove redirection from command parameters
	remove_entry(array, i, len);
}

void
redirections(char* line)
{
	// Split line in independent tokens
	char* linedup = strdup(line);
	char* array[10];
	int numTokens = tokenize(linedup, array, 10);

	// search and apply redirections
	int i = 0;
	for(; i < numTokens; ++i)
	{
		char c = array[i][0];

		switch(c)
		{
			case '%':	// Environment
				redirections_environment(array, i, &numTokens);
				break;

			case '<':	// Stdin
				redirections_stdin(array, i, &numTokens);
				break;

			case '>':	// Stdout
				redirections_stdout(array, i, &numTokens);

			default:
				continue;
		};

		// step back index because we have removed the redirection entry so next
		// loop we will check over the same (next) one
		--i;
	}

	// collapse line to remove redirections
	int diffpos = line-linedup;
	for(i = 0; i < numTokens; ++i)
	{
		// move token
		int len = strlen(array[i]);
		memmove(line, diffpos + array[i], len);

		// add null byte at end of collapsed line
		if(i >= numTokens-1)
			*(line+len) = '\0';

		// add space and advance if necesary
		else if(len)
		{
			line+=len;
			array[i]+=len;

			int diff = array[i+1] - array[i];
			memmove(line, diffpos + array[i], diff+1);
			line += diff;
		}
	}

	// Free local copy of line
	free(linedup);
}
