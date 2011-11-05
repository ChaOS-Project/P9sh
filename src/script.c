/*
 * script.c
 *
 *  Created on: 06/11/2011
 *      Author: piranna
 */

#include <u.h>
#include <libc.h>

#include "pipeline.h"


void
process_script(char* line)
{
	// Split script in independent pipelines
	char** array = calloc(10, sizeof(char*));
//	char* array[10];
	int numPipelines = gettokens(line, array, 10, ";");

	// run script
	int i;
	for(i = 0; i < numPipelines; ++i)
		process_pipeline(array[i]);

	// Free array
	free(array);
}
