/*
 * environment.c
 *
 *  Created on: 14/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include <regexp.h>

#include "common.h"


void
do_expand(char* expanded, char* key)
// Search for environment var `key` and append it to the `expanded` string
{
	// If `key` is not an empty string do the expansion, else do nothing
	if(key[0])
	{
		// Get the environment variable and do the expansion if it exists
		char* env = getenv(key);
		if(env)
		{
			char* quote = quotestrdup(env);
			free(env);

			strcat(expanded, quote);

			free(quote);
		}
//		else
//		{
//			strcat(expanded, "$");
//			strcat(expanded, key);
//		}
	}
}

char*
environment_expand(char* line)
// alternative function to expand environment variables using a state machine
// since regular expression functions are not working correctly for me
// (even with example code...)
{
	char* expanded = calloc(1024 * 10, sizeof(char));

	char key[32];
	strcpy(key, "");

	int mode = 0;

	// Loop over all the characters of `line` string
	for(; *line; ++line)
	{
		char c = *line;

		// Begin of environment variable
		if(c == '$')
		{
			// expand previous one (if any)
			do_expand(expanded, key);

			// prepare new expansion
			strcpy(key, "");
			mode = 1;
		}

		// Alphanumeric character
		// Append it to expanded string or to key depending of current mode
		else if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_'
			 || ('0' <= c && c <= '9'))
		{
			char* buf = mode? key: expanded;

			int len = strlen(buf);
			buf[len] = c;
			buf[len+1] = '\0';
		}

		// Other characters (spaces, tabs, new lines...)
		// Depending of the current mode, just add them to the expanded string
		// or expand previous key, too.
		else
		{
			if(mode)
			{
				do_expand(expanded, key);

				strcpy(key, "");
				mode = 0;
			}

			// Add readed simbol char
			int len = strlen(expanded);
			expanded[len] = c;
		}
	}

	do_expand(expanded, key);

	return expanded;
}

//char*
//environment_expand(char* line)
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
//			print("6");stdout.txt
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
redirect_environment(char* key)
// Redirect stdout to a child process that write it to an environment variable
{
	int fd[2];
	pipe(fd);

	switch(fork())
	{
		case -1:
			return -1;

		case 0:		// child
			close(fd[1]);
			dup(fd[0], 0);
			close(fd[0]);

			// Reset environment variable value (if any)
			putenv(key, "");

			// put stdout in value
			char line[1024];
			while(read(0, line, sizeof(line)))
			{
				// Get previous value of the environment variable
				char* env = getenv(key);

				// Append new data to the previous one
				int len = strlen(env) + strlen(line) + 1;
				char* value = malloc(len * sizeof(char));

				strcpy(value, env);
				strcat(value, line);

				free(env);

				// Put new value on the environment variable
				putenv(key, value);
				free(value);
			}

			exits(nil);

		default:	// parent
			close(fd[0]);
			dup(fd[1], 1);
			close(fd[1]);
	}

	return 0;
}


int
environment_set(char* line)
// Assign a value to one (or various) environment variable
// Return 1 if assignations were processed, else return 0
{
	// Split line in different assignations
	char* array[10];
	int numAssignations = gettokens(line, array, 10, "=");

	// If line has at least one assignation, do them
	if(numAssignations > 1)
	{
		char* value = array[numAssignations-1];

		// Remove trailing new line
		int len = strlen(value);
		if(value[len-1] == '\n')
			value[len-1] = '\0';

		// Do the assignations
		numAssignations -= 2;
		for(; numAssignations >= 0; --numAssignations)
			putenv(array[numAssignations], value);

		return 1;
	}

	return 0;
}
