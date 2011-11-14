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


void
do_expand(char** expanded, char* key)
{
	if(key[0])
	{
		char* env = getenv(key);
		if(env)
		{
			strcat(*expanded, env);
			free(env);
		}
		else
		{
			strcat(*expanded, "$");
			strcat(*expanded, key);
		}
	}
}

char*
expand_envVars(char* line)
// alternative function to expand environment variables using a state machine
// since regular expression functions are not working correctly
{
	char* expanded = malloc(1024 * 10 * sizeof(char));
	strncpy(expanded, "", 1024 * 10 * sizeof(char));

	char key[32];
	strncpy(key, "", 32);

	int mode = 0;

	for(; *line; ++line)
	{
		char c = *line;

		if(c == '$')
		{
			// expand previous one (if any)
			do_expand(&expanded, key);

			// prepare new expansion
			strncpy(key, "", 32);
			mode = 1;
		}

		else if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_')
		{
			char* buf = mode? key: expanded;

			buf[strlen(buf)] = c;
			buf[strlen(buf)] = '\0';
		}

		else
		{
			if(mode == 0)
			{
				expanded[strlen(expanded)] = c;
				expanded[strlen(expanded)] = '\0';
			}
			else
			{
				do_expand(&expanded, key);

				strncpy(key, "", 32);
				mode = 0;
			}
		}
	}

	do_expand(&expanded, key);

	return expanded;
}

//char*
//expand_envVars(char* line)
//{
//	line = "ls";
//
//	// Compile regular expression
//	char reg[] = "[a-zA-Z_]+";
////	char reg[] = "\\$[a-zA-Z_]+";
//	Reprog* prog = regcomp(reg);
//
//	Resub sub[16];
//
//	print("nelem(sub): %d\n", nelem(sub));
//	print("regexec: %d\n", regexec(prog, line, sub, nelem(sub)));
//
//	char* expanded = nil;
//
//	// Exec regular expression until there's no match
//	while(regexec(prog, line, sub, nelem(sub)))
//	{
//		print("expanded: %s\n",expanded);
//		if(expanded == nil)
//		{
//			print("5");
//			expanded = malloc(1024 * 10 * sizeof(char));
//			*expanded = '\0';
//			print("6");
//		}
//
//		// Concat normal string from begining (if any)
//		if(line < sub[0].s.sp)
//			strncat(expanded, line, sub[0].s.sp - line);
//		print("7");
//
//		// Concat environment var
//		char match[256];
//		strncpy(match, sub[0].s.sp, sub[0].e.ep - sub[0].s.sp);
//
//		char* env = getenv(match);
//		strncat(expanded, env, strlen(env));
//		free(env);
//		print("8");
//
//		// Advance line pointer to catch the next match (if any)
//		line = sub[0].e.ep;
//	}
//	print("9");
//
//	// Concat remaining normal string after last match (if any)
//	strncat(expanded, line, strlen(line));
//
//	// Free compiled regular expression
//	free(prog);
//
//	return expanded;
//}


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
	int ret = 0;

	char* array[10];
	int numTokens = tokenize(line, array, 10);

	if(!strcmp(array[0], "cd"))
	{
		cd(numTokens, array);

		ret = 1;
	}

	free(line);
	return ret;
}

int
set_env(char* line)
{
	// Split line in different assignations
	char* array[10];
	int numAssignations = gettokens(line, array, 10, "=");

	if(numAssignations > 1)
	{
		char* value = array[numAssignations-1];

		numAssignations -= 2;
		for(; numAssignations >= 0; --numAssignations)
			putenv(array[numAssignations], value);

		return 1;
	}

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
		// expand environment variables and substitute command line char pointer
		// with the one with expanded environment variables (if necesary)
//		char* expanded = expand_envVars(array[i]);
//		if(expanded) array[i] = expanded;
		array[i] = expand_envVars(array[i]);

		// move commands to background (if necesary)
		background(array[i]);

		// Exec `cd` (it's a built-in, but must change shell environment itself,
		// so we check and exec for it directly here)
		if(builtin_cd(array[i]))
			continue;

		// Set environment variable
		if(set_env(array[i]))
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

		// free line created by environment variables expansion
		free(array[i]);
	}

	return 0;
}
