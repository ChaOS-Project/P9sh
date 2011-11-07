/*
 * file_redirections.c
 *
 *  Created on: 07/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>


void
redirect_stdin(char* array[], int i, int* len)
{
	// '<' is alone, get it's next argument
	if(strlen(array[i]) == 1)
	{
		int j = i;
		for(; j < *len; ++j)
			array[j] = array[j+1];

		(*len)--;
	}

	// '<' has the file, increase the pointer
	else
		array[i]++;

	// Open file descriptor for redirection
	int fd = open(array[i], OREAD);
	dup(fd, 0);
	close(fd);

	// Remove redirection from command parameters
	int j = i;
	for(; j < *len; ++j)
		array[j] = array[j+1];

	(*len)--;
}

void
redirect_stdout(char* array[], int i, int* len)
{
	// '>' is alone, get it's next argument
	if(strlen(array[i]) == 1)
	{
		int j = i;
		for(; j < *len; ++j)
			array[j] = array[j+1];

		(*len)--;
	}

	// '<' has the file, increase the pointer
	else
		array[i]++;

	// Open file descriptor for redirection
	int fd = open(array[i], OWRITE);
	if(fd < 0)
		fd = create(array[i], OWRITE, 0664);
	dup(fd, 1);
	close(fd);

	// Remove redirection from command parameters
	int j = i;
	for(; j < *len; ++j)
		array[j] = array[j+1];

	(*len)--;
}

void
file_redirections(char* line)
{
	// Split line in independent tokens
	char* array[10];
	int numTokens = tokenize(line, array, 10);

	// apply redirections
	int i = 0;
	for(; i < numTokens; ++i)
	{
		char c = array[i][0];

		switch(c)
		{
			case '<':	// Stdinvoid
				redirect_stdin(array, i, &numTokens);
				--i;
				break;

			case '>':	// Stdout
				redirect_stdout(array, i, &numTokens);
				--i;
		}
	}

	// collapse line
	for(i = 0; i < numTokens; ++i)
	{
		// move token
		int len = strlen(array[i]);
		memmove(line, array[i], len);

		// add null byte at end of collapse line
		if(i == numTokens-1)
			*(line+len) = '\0';

		// add space and advance if necesary
		else if(len && i < numTokens-1)
		{
			*(line+=len) = ' ';
			++line;
		}
	}
}
