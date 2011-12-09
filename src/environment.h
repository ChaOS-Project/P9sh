/*
 * environment.h
 *
 *  Created on: 14/11/2011
 *      Author: piranna
 */

#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

char* environment_expand(char* line);
int   environment_set(char* line);

int   redirect_environment(char* key);

#endif /* ENVIRONMENT_H_ */
