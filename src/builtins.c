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
// Change the current working dir
//
// Althought it's coded as a built-in commnad for homogeneity, in fact it's
// being called directly from the P9sh main process because it must change the
// shell current working dir
{
	// Called with no parameters, go to user's home
	if(argc == 1)
	{
		char* path = getenv(ENV_HOME);

		chdir(path);
		free(path);
	}

	// Called with a path parameter (relative or absolute)
	else
		chdir(argv[1]);
}
