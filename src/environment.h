/*
 * environment.h
 *
 *  Created on: 14/11/2011
 *      Author: piranna
 */

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

char* environment_expand(char* line);
void  environment_redirection(char* line);
int   environment_set(char* line);

#endif /* ENVIRONMENT_H_ */
