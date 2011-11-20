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
//		else
//		{
//			strcat(*expanded, "$");
//			strcat(*expanded, key);
//		}
	}
}

char*
environment_expand(char* line)
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

			// reset var
			putenv(key, "");

			int nr;
			char line[1024];
			while((nr = read(0, line, sizeof(line))))
			{
				char value[10 * 1024];

				char* env = getenv(key);
				strncpy(value, env, 10 * 1024);
				strncat(value, line, 10 * 1024);
				free(env);

				putenv(key, value);
			}

			exits(nil);

		default:	// parent
			close(fd[0]);
			dup(fd[1], 1);
			close(fd[1]);
	}

	return 0;
}

void
environment_redirection(char* line)
{
	// Split line in independent tokens
	char* array[10];
	int numTokens = tokenize(line, array, 10);

	// search and apply redirections
	int i = 1;
	for(; i < numTokens; ++i)
	{
		char c = array[i][0];

		switch(c)
		{
			case '%':	// redirection
				// '%' is alone in the entry, remove it and get the next one
				if(strlen(array[i]) == 1)
					remove_entry(array, i, &numTokens);

				// entry has the file in it, set the pointer to it
				else
					array[i]++;

				// Put index at previous token
				--i;

				// Redirect stdout to file
				redirect_environment(array[i]);

				// Remove redirection from command parameters
				remove_entry(array, i, &numTokens);
				break;

			default:
				continue;
		};

		// step back index because we have removed the redirection entry so next
		// loop we will check over the same (next) one
		--i;
	}

	// collapse line to remove redirections
	for(i = 0; i < numTokens; ++i)
	{
		// move token
		int len = strlen(array[i]);
		memmove(line, array[i], len);

		// add null byte at end of collapsed line
		if(i >= numTokens-1)
			*(line+len) = '\0';

		// add space and advance if necesary
		else if(len)
		{
			*(line+=len) = ' ';
			++line;
		}
	}
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

		numAssignations -= 2;
		for(; numAssignations >= 0; --numAssignations)
			putenv(array[numAssignations], value);

		return 1;
	}

	return 0;
}
