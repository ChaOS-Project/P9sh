/*
 * builtins.c
 *
 *  Created on: 05/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>


void
cd(char* argv[])
{
	char* path = argv[1];
	print("cd 1 %s\n",path);

	// Called with no parameters
	if(path == nil || path[0] == 0)
	{
		path = getenv("HOME");
		print("cd 2 %s\n",path);

		putenv("PWD", path);
		free(path);
	}

	// Called with absolute path
	else if(path[0] != '/')
	{
		char newpath[256];

		char* pwd = getenv("PWD");
		strncpy(newpath, pwd,strlen(pwd));
		free(pwd);

		strncat(newpath, "/", 2);
		strncat(newpath, path, strlen(path));
		print("cd 3 %s\n",newpath);

		putenv("PWD", newpath);
	}

	// Called with relative path
	else
		putenv("PWD", path);
	print("getenv %s\n",getenv("PWD"));
}
