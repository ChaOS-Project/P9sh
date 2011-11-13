/*
 * file_redirections.c
 *
 *  Created on: 07/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "common.h"


void
remove_entry(char* array[], int i, int* len)
{
	for(; i < *len; ++i)
		array[i] = array[i+1];

	(*len)--;
}


void
stdin_redirection(char* array[], int i, int* len)
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
stdout_redirection(char* array[], int i, int* len)
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
file_redirections(char* line)
{
//	char* linedup = strdup(line);

	// Split line in independent tokens
	char* array[10];
	int numTokens = gettokens(line, array, 10, "\t\r ");
//	int numTokens = tokenize(linedup, array, 10);

	// search and apply redirections
	int i = 0;
	for(; i < numTokens; ++i)
	{
		char c = array[i][0];

		switch(c)
		{
			case '<':	// Stdin
				stdin_redirection(array, i, &numTokens);
				break;

			case '>':	// Stdout
				stdout_redirection(array, i, &numTokens);

			default:
				continue;
		};

		// step back index because we have removed the redirection entry so next
		// loop we will check over the same (next) one
		--i;
	}

	// collapse line to remove redirections
	for(i = 0; i < numTokens; ++i)
	{
		// move token
		int len = strlen(array[i]);
		memmove(line, array[i], len);

		// add null byte at end of collapsed line
		if(i >= numTokens-1)
			*(line+len) = '\0';

		// add space and advance if necesary
		else if(len)
		{
			*(line+=len) = ' ';
			++line;
		}
	}

//	free(linedup);
}
