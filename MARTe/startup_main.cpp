/**
 * @file main.c
 * @brief Source file for class main.c
 * @date 10/08/2016
 * @author Andre' Neto
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing,
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class BufferGAM (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#define INCLUDE_CFG_FILE(x) QUOTE(x)
#define INCLUDE_SCHEDULER(x) QUOTE(x.h)

#include "AdvancedErrorManagement.h"
#include "ConfigurationDatabase.h"
#include "GAM.h"
#include "GAMGroup.h"
#include "GAMSchedulerI.h"
#include "ObjectRegistryDatabase.h"
#include "RealTimeThread.h"
#include "StandardParser.h"
#include "StreamMemoryReference.h"
#include "Threads.h"
#include "string.h"
#include "CfgUploader.h"
#include <stdio.h>
//#include "usbd_cdc_if.h"

#include INCLUDE_SCHEDULER(__SCHEDULER__)

/*#include "usbd_cdc_if.h"
 #include "cmsis_os.h"*/
using namespace MARTe;
const char8 * const config =
		#include INCLUDE_CFG_FILE(__CFG__FILE__)
 ;

//WHY???? I need this otherwise the class registered won't be linked
StreamString boh;

extern void DebugErrorProcessFunction(
		const MARTe::ErrorManagement::ErrorInformation &errorInfo,
		const char * const errorDescription);

extern void PrintStack(ThreadIdentifier &tid);

static void MARTeAppLauncher(void const *ignored) {

	uint32 confSize = StringHelper::Length(config) + 1;

	StreamMemoryReference *stream = new StreamMemoryReference(config, confSize);
	stream->Seek(0);

	bool ok = true;

#ifdef _UPLOAD_CFG
	StreamString *cfgBuffer = new StreamString;

	{
		ConfigurationDatabase localCdb;
		StandardParser localParser(*stream, localCdb);

		ok = localParser.Parse();
		delete stream;

		ReferenceContainer localContainer;
		localContainer.Initialise(localCdb);

		ReferenceT < CfgUploader > cfgUploader = localContainer.Find("CfgUploader");
		ok = cfgUploader.IsValid();
		if (ok) {
			cfgUploader->UploadCfg(*cfgBuffer);
		}
	}
#else
	StreamMemoryReference* cfgBuffer = stream;

#endif
	ConfigurationDatabase cdb;

	if (ok) {
		cfgBuffer->Seek(0);
		StandardParser globalParser(*cfgBuffer, cdb);

		ok = globalParser.Parse();
		//REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Status 0 %d", ok);
		delete cfgBuffer;

	}

	ObjectRegistryDatabase *godb = NULL;
	if (ok) {
		godb = ObjectRegistryDatabase::Instance();
		godb->Purge();

		ok = godb->Initialise(cdb);
		//REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Status 1 %d", ok);

	}

	ReferenceT < RealTimeApplication > application;
	if (ok) {
		application = godb->Find("Application1");
		ok = application.IsValid();
		//REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Status 2 %d", ok);

	}

	if (ok) {
		ok = application->ConfigureApplication();
		//REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Status 3 %d", ok);

	}

	if (ok) {

		ok = application->PrepareNextState("State1");
		//REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Status 4 %d", ok);

	}

	if (ok) {
		application->StartNextStateExecution();

	}
}

//char buffer[128]={0};
extern "C" {

void UserMainFunction(const void *arg) {

	SetErrorProcessFunction(&DebugErrorProcessFunction);

	MARTeAppLauncher (NULL);

	while (1) {
		Sleep::Sec(1.0);
	}
}
}

