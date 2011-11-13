/*
 * heredoc.h
 *
 *  Created on: 13/11/2011
 *      Author: piranna
 */

#ifndef HEREDOC_H_
#define HEREDOC_H_

typedef struct
{
	int  mode;
	char command[256];
	char buffer[1024];
} tHeredoc;

int heredoc_process(tHeredoc* heredoc, char** line);

#endif /* HEREDOC_H_ */
