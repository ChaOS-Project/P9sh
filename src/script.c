/*
 * script.c
 *
 *  Created on: 06/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include <regexp.h>

#include "builtins.h"
#include "common.h"
#include "pipeline.h"


char*
expand_envVars(char* line)
{
	line = "ls";

	// Compile regular expression
	char reg[] = "[a-zA-Z_]+";
//	char reg[] = "\\$[a-zA-Z_]+";
	Reprog* prog = regcomp(reg);

	Resub sub[16];

	print("nelem(sub): %d\n", nelem(sub));
	print("regexec: %d\n", regexec(prog, line, sub, nelem(sub)));

	char* expanded = nil;

	// Exec regular expression until there's no match
	while(regexec(prog, line, sub, nelem(sub)))
	{
		print("expanded: %s\n",expanded);
		if(expanded == nil)
		{
			print("5");
			expanded = malloc(1024 * 10 * sizeof(char));
			*expanded = '\0';
			print("6");
		}

		// Concat normal string from begining (if any)
		if(line < sub[0].s.sp)
			strncat(expanded, line, sub[0].s.sp - line);
		print("7");

		// Concat environment var
		char match[256];
		strncpy(match, sub[0].s.sp, sub[0].e.ep - sub[0].s.sp);

		char* env = getenv(match);
		strncat(expanded, env, strlen(env));
		free(env);
		print("8");

		// Advance line pointer to catch the next match (if any)
		line = sub[0].e.ep;
	}
	print("9");

	// Concat remaining normal string after last match (if any)
	strncat(expanded, line, strlen(line));

	// Free compiled regular expression
	free(prog);

	return expanded;
}


int
background(char* line)
// Check and exec on background all the commands ended with '&' on the command
// line and return the last, foreground command
{
	// Split pipeline in independent commands
	char* array[10];
	int numCommands = gettokens(line, array, 10, "&");

	// run script commands
	int i;
	for(i = 0; i < numCommands-1; ++i)
	{
		switch(fork())
		{
			case -1:
				return -1;

			case 0:	// child
			{
				// redirect stdin to /dev/null
				redirect_stdin("/dev/null");

				// process pipeline
				process_pipeline(array[i]);
				exitError();
			}

			// parent loop to exec in background the next command (if any)
		}
	}

	// collapse line to let only the last (not background) command on it
	int len = strlen(array[numCommands-1]);
	memmove(line, array[numCommands-1], len);
	*(line+len) = '\0';

	return 0;
}


int
builtin_cd(char* line)
{
	line = strdup(line);

	char* array[10];
	int numTokens = tokenize(line, array, 10);

	if(!strcmp(array[0], "cd"))
	{
		cd(numTokens, array);

		free(line);
		return 1;
	}

	free(line);
	return 0;
}


int
process_script(char* line)
{
	// Split script in independent pipelines
	char* array[10];
	int numPipelines = gettokens(line, array, 10, ";");

	// run script commands
	int i;
	for(i = 0; i < numPipelines; ++i)
	{
//		// expand environment variables and substitute command line char pointer
//		// with the one with expanded environment variables (if necesary)
//		char* expanded = expand_envVars(array[i]);
//		if(expanded) array[i] = expanded;

		// move commands to background (if necesary)
		background(array[i]);

		// Exec `cd` (it's a built-in, but must change shell environment itself,
		// so we check and exec for it directly here)
		if(builtin_cd(array[i]))
			continue;

		// run foreground command
		switch(fork())
		{
			case -1:
				return -1;

			case 0:		// child
			{
				// process pipeline
				process_pipeline(array[i]);
				exitError();
			}

			default:	// parent
			{
				Waitmsg* m = wait();

				if(m->msg[0] == 0)
					free(m);

				else
				{
					werrstr(m->msg);
					free(m);
					return -1;
				}
			}
		}

//		// free line created by environment variables expansion
//		free(expanded);
	}

	return 0;
}
