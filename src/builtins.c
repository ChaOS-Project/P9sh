/*
 * builtins.c
 *
 *  Created on: 05/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>


void
cd(int argc, char* argv[])
{
	// Called with no parameters, go to $HOME
	if(argc == 1)
	{
		char* path = getenv("HOME");

		chdir(path);
//		putenv("PWD", path);
		free(path);
	}

	// Called with a path parameter (relative or absolute)
	else
	{
		chdir(argv[1]);
//		putenv("PWD", path);
	}
}
