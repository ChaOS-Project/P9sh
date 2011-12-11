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
redirections_environment(char* array[], int i)
{
	char* file;

	// '%' is alone in the entry, remove it and get the next one
	if(strlen(array[i]) == 1)
		array[i][0] = '\0';

	// entry has the file in it, set the pointer to it
	else
		array[i]++;

	// Put index at previous token
	file = array[i-1];

	// Redirect stdout to file
	redirect_environment(file);

	// Remove redirection from command parameters
	file[0] = '\0';
}

void
redirections_stdin(char* array[], int i)
{
	char* file;

	// '<' is alone in the entry, remove it and get the next one
	if(strlen(array[i]) == 1)
	{
		array[i][0] = '\0';
		file = array[i+1];
	}

	// entry has the file in it, set the pointer to it
	else
	{
		array[i]++;
		file = array[i];
	}

	// Redirect stdin to file
	redirect_stdin(file);

	// Remove redirection from command parameters
	file[0] = '\0';
}

void
redirections_stdout(char* array[], int i)
{
	char* file;

	// '>' is alone in the entry, remove it and get the next one
	if(strlen(array[i]) == 1)
	{
		array[i][0] = '\0';
		file = array[i+1];
	}

	// entry has the file in it, set the pointer to it
	else
	{
		array[i]++;
		file = array[i];
	}

	// Redirect stdout to file
	redirect_stdout(file);

	// Remove redirection from command parameters
	file[0] = '\0';
}

void
redirections(char* line)
{
	// Split line in independent tokens
	char* linedup = strdup(line);
	char* array[1000];
	int numTokens = tokenize(linedup, array, 1000);

	// search and apply redirections
	int i = 0;
	for(; i < numTokens; ++i)
	{
		char c = array[i][0];

		switch(c)
		{
			case '%':	// Environment
				redirections_environment(array, i);
				break;

			case '<':	// Stdin
				redirections_stdin(array, i);
				break;

			case '>':	// Stdout
				redirections_stdout(array, i);

			default:
				continue;
		};

		// step back index because we have removed the redirection entry so next
		// loop we will check over the same (next) one
		--i;
	}

	// collapse line to remove redirections
	int diffpos = line-linedup;
	for(i = 0; i < numTokens-1; ++i)
	{
		if(array[i][0])
		{
			int len = array[i+1] - array[i];
			memmove(line, diffpos + array[i], len);
			line += len;
		}
	}

	if(numTokens && array[numTokens-1][0])
	{
		int len = strlen(array[numTokens-1]);
		memmove(line, diffpos + array[numTokens-1], len);
		line += len;
	}
	*line = '\0';


	// Free local copy of line
	free(linedup);
}
