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
