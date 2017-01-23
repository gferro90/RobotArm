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
#include "usbd_cdc_if.h"

#include INCLUDE_SCHEDULER(__SCHEDULER__)

/*#include "usbd_cdc_if.h"
 #include "cmsis_os.h"*/
using namespace MARTe;

const char8 *const config =
#include INCLUDE_CFG_FILE(__CFG__FILE__)
        ;

//WHY???? I need this otherwise the class registered won't be linked °_°
StreamString boh;
int8 SM_changeState = 1;
int8 SM_nextState = 0;

extern void DebugErrorProcessFunction(const MARTe::ErrorManagement::ErrorInformation &errorInfo,
                                      const char * const errorDescription);

extern void PrintStack(ThreadIdentifier &tid);
#if 0
static void MARTeAppLauncher(void const *ignored) {
    if (!USBInitialized()) {
        USBOpen();
    }

    char buffer[64];
    uint32 size=64;
    USBRead(buffer, (uint32_t*)&size, 0);
    USBSync();

    uint32 confSize = StringHelper::Length(config) + 1;
    ConfigurationDatabase cdb;
    StreamMemoryReference *stream = new StreamMemoryReference(config, confSize);
    stream->Seek(0);
    StandardParser parser(*stream, cdb);

    bool ok = parser.Parse();
    delete stream;

    ObjectRegistryDatabase *godb = NULL;
    if(ok) {
        godb = ObjectRegistryDatabase::Instance();
        godb->CleanUp();
        ok = godb->Initialise(cdb);

    }

    ReferenceT < RealTimeApplication > application;
    if (ok) {
        application = godb->Find("Application1");
        ok = application.IsValid();

    }

    if (ok) {
        ok = application->ConfigureApplication();
    }

    if (ok) {
        ok = application->PrepareNextState("State1");
    }

    if (ok) {
        application->StartExecution();
    }
}

#endif

static void MARTeAppLauncher(void const *ignored) {

    if (!USBInitialized()) {
        USBOpen();
    }

    char buffer[64];
    uint32 size = 64;

    uint32 confSize = StringHelper::Length(config) + 1;
    ConfigurationDatabase cdb;

    //TODO read the stream from serial port
    StreamMemoryReference *stream = new StreamMemoryReference(config, confSize);
    stream->Seek(0);
    StandardParser parser(*stream, cdb);

    bool ok = parser.Parse();
    delete stream;

    ObjectRegistryDatabase *godb = NULL;
    godb = ObjectRegistryDatabase::Instance();

    if (ok) {
        /*for (uint32 i = 0; i < 20; i++) {

            REPORT_ERROR(ErrorManagement::Warning, "before Init");
            Sleep::MSec(100);
        }*/
        ok = godb->Initialise(cdb);
    }

    ReferenceT < RealTimeApplication > application;
    if (ok) {
        /*for (uint32 i = 0; i < 20; i++) {

            REPORT_ERROR(ErrorManagement::Warning, "Init done");
            Sleep::MSec(100);
        }*/
        application = godb->Find("Application1");
        ok = application.IsValid();

    }

    if (ok) {
         /*for (uint32 i = 0; i < 20; i++) {

         REPORT_ERROR(ErrorManagement::Warning, "Before config");
         Sleep::MSec(100);
         }*/
        ok = application->ConfigureApplication();
        /*for (uint32 i = 0; i < 20; i++) {

         REPORT_ERROR(ErrorManagement::Warning, "After config");
         Sleep::MSec(100);
         }*/

    }

    ReferenceContainer states;
    if (ok) {
        ok = application->GetStates(states);

        for (;;) {
         //   for (uint32 i = 0; i < 20; i++) {

            REPORT_ERROR(ErrorManagement::Warning, "Waiting initial packet...");
           /* Sleep::MSec(100);
            }*/

            // wait from usb the trigger
            size = 64;
            USBRead(buffer, (uint32_t*) &size, 0);
            USBSync();
            SM_nextState = 0;

            REPORT_ERROR(ErrorManagement::Warning, "Go on");

            while (SM_nextState >= 0) {
                if (ok) {
                    ok = application->PrepareNextState(states.Get(SM_nextState)->GetName());
                }

                if (ok) {
                    SM_changeState = 0;
                    application->StartExecution();
                }
            }
            REPORT_ERROR(ErrorManagement::Warning, "OOOK!");
        }
    }

}
extern "C" {

void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName) {
    while (1) {
        //	REPORT_ERROR_PARAMETERS(ErrorManagement::FatalError, "Stack overflow in task %s", pcTaskName);
    }
}

void main(const void *arg) {

    SetErrorProcessFunction(&DebugErrorProcessFunction);

    ThreadIdentifier tid = Threads::BeginThread(MARTeAppLauncher, NULL, configMINIMAL_STACK_SIZE * 16, "MARTeAppLauncher");

    //Threads::BeginThread((ThreadFunctionType)PrintStack, &tid, configMINIMAL_STACK_SIZE*8);

    while (1) {
        Sleep::Sec(1.0);
    }
}
}

