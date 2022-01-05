/*
 * HandleDatabase.c
 *
 *  Created on: 06 giu 2018
 *      Author: pc
 */

#include "HandleDatabase.h"
#include <stdio.h>
#include <string.h>

extern "C" {

struct HandleNode handleDatabase[64];
unsigned int numberOfHandles=0u;

void *GetHandle(const char *handleName){
	unsigned int i=0u;
	for(i=0u; i<numberOfHandles; i++){
		if(strcmp(handleDatabase[i].handleName, handleName)==0){
			return (void*)handleDatabase[i].handle;
		}
	}
	return NULL;
}

}
