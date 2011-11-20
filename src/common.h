/*
 * common.h
 *
 *  Created on: 08/11/2011
 *      Author: piranna
 */

#ifndef COMMON_H_
#define COMMON_H_

void exitError(void);

void redirect_stdin(char* filepath);
void redirect_stdout(char* filepath);

void remove_entry(char* array[], int i, int* len);

#endif /* COMMON_H_ */
