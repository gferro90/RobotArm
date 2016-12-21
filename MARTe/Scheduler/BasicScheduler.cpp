/**
 * @file BasicScheduler.cpp
 * @brief Source file for class BasicScheduler
 * @date 6 Aug 2016
 * @author andre
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
 * the class BasicScheduler (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "BasicScheduler.h"
#include "RealTimeApplication.h"

using namespace MARTe;
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
extern int8 SM_changeState;

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
BasicScheduler::BasicScheduler() {
    scheduledStates = NULL;
}

bool BasicScheduler::ConfigureScheduler() {
    bool ret = GAMSchedulerI::ConfigureScheduler();
    if (ret) {
        scheduledStates = GetSchedulableStates();
    }
    return ret;
}

void BasicScheduler::StartExecution() {
    while (!SM_changeState) {
        //run the first thread
        Cycle(0);
        if(SM_changeState){
            REPORT_ERROR(ErrorManagement::FatalError,"Stopping Scheduler");
        }
    }
}

void BasicScheduler::StopExecution() {

}

void BasicScheduler::Cycle(uint32 threadId) {
    ExecuteSingleCycle(scheduledStates[RealTimeApplication::GetIndex()]->threads[threadId].executables,
                       scheduledStates[RealTimeApplication::GetIndex()]->threads[threadId].numberOfExecutables);
}

CLASS_REGISTER(BasicScheduler, "1.0")
