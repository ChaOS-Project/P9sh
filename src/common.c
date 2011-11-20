/*
 * common.c
 *
 *  Created on: 08/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>


void
exitError(void)
// Check and set an error string previouly defined with werrstr
{
	char err[ERRMAX];
	uint nerr = 0;
	rerrstr(err, nerr);
	exits(err);
}


void
redirect_stdin(char* filepath)
{
	int fd = open(filepath, OREAD);
	dup(fd, 0);
	close(fd);
}

void
redirect_stdout(char* filepath)
{
	int fd = open(filepath, OWRITE);
	if(fd < 0)
		fd = create(filepath, OWRITE, 0664);
	dup(fd, 1);
	close(fd);
}


void
remove_entry(char* array[], int i, int* len)
{
	for(; i < *len; ++i)
		array[i] = array[i+1];

	(*len)--;
}
