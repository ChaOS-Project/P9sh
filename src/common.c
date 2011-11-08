/*
 * common.c
 *
 *  Created on: 08/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>


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
