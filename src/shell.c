/*
 ============================================================================
 Name        : P9sh.c
 Author      : Jesus Leganes Combarro "Piranna"
 Version     :
 Copyright   : GPL - Copyright 2011
 Description : CLI shell for Plan 9 operating system
 ============================================================================
 */

#include <u.h>
#include <libc.h>

void
main(int argc, char* argv[])
{
	print("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	exits(nil);
}
