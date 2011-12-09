/*
 * builtins.c
 *
 *  Created on: 05/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#ifdef unix
	#define ENV_HOME "HOME"
#else
	#define ENV_HOME "home"
#endif


void
cd(int argc, char* argv[])
{
	// Called with no parameters, go to $HOME
	if(argc == 1)
	{
		char* path = getenv(ENV_HOME);

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
