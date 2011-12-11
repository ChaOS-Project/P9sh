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
{
	if(key[0])
	{
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
// since regular expression functions are not working correctly
{
	char* expanded = calloc(1024 * 10, sizeof(char));

	char key[32];
	strcpy(key, "");

	int mode = 0;

	for(; *line; ++line)
	{
		char c = *line;

		if(c == '$')
		{
			// expand previous one (if any)
			do_expand(expanded, key);

			// prepare new expansion
			strcpy(key, "");
			mode = 1;
		}

		else if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_')
		{
			char* buf = mode? key: expanded;

			int len = strlen(buf);
			buf[len] = c;
			buf[len+1] = '\0';
		}

		else
		{
			if(mode == 0)
			{
				int len = strlen(expanded);
				expanded[len] = c;
			}
			else
			{
				do_expand(expanded, key);

				int len = strlen(expanded);
				expanded[len] = c;

				strcpy(key, "");
				mode = 0;
			}
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
				char* env = getenv(key);

				int len = strlen(env) + strlen(line) + 1;
				char* value = malloc(len * sizeof(char));

				strcpy(value, env);
				strcat(value, line);

				free(env);

				// put new value on environment variable
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
{
	// Split line in different assignations
	char* array[10];
	int numAssignations = gettokens(line, array, 10, "=");

	if(numAssignations > 1)
	{
		char* value = array[numAssignations-1];

		int len = strlen(value);
		if(value[len-1] == '\n')
			value[len-1] = '\0';

		numAssignations -= 2;
		for(; numAssignations >= 0; --numAssignations)
			putenv(array[numAssignations], value);

		return 1;
	}

	return 0;
}
