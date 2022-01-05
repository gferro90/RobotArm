/*
 * HandleDatabase.h
 *
 *  Created on: 07 giu 2018
 *      Author: pc
 */

#ifndef MARTE2_PLATFORMS_STM32_SCRIPTS_HANDLEDATABASE_H_
#define MARTE2_PLATFORMS_STM32_SCRIPTS_HANDLEDATABASE_H_

#ifdef __cplusplus
 extern "C" {
#endif

struct HandleNode {
	const char *handleName;
	void* handle;
};

extern struct HandleNode handleDatabase[64];
extern unsigned int numberOfHandles;

#define REGISTER_HANDLE(varName, varHandle)\
__attribute__((constructor))\
void init_##varName(){\
	handleDatabase[numberOfHandles].handleName = #varName;\
	handleDatabase[numberOfHandles].handle = (void*)varHandle;\
	numberOfHandles++;\
}


void *GetHandle(const char *handleName);

#ifdef __cplusplus
 }
#endif

#endif /* MARTE2_PLATFORMS_STM32_SCRIPTS_HANDLEDATABASE_H_ */
