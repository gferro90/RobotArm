/**
 * @file ResetEncoderReader.cpp
 * @brief Source file for class ResetEncoderReader
 * @date 20/dic/2016
 * @author pc
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
 * the class ResetEncoderReader (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "ResetEncoderReader.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

ResetEncoderReader::ResetEncoderReader() {
    // Auto-generated constructor stub for ResetEncoderReader
    // TODO Verify if manual additions are needed
}

ResetEncoderReader::~ResetEncoderReader() {
    // Auto-generated destructor stub for ResetEncoderReader
    // TODO Verify if manual additions are needed
}

bool ResetEncoderReader::Execute() {
    uint32 n;
    //REPORT_ERROR(ErrorManagement::Warning, "Reading Enc");
    bool ret = true;
    for (n = 0u; (n < numberOfCopies) && (ret); n++) {
        if (copyTable != NULL_PTR(MemoryMapBrokerCopyTableEntry *)) {
            if (*(int32*) (copyTable[n].gamPointer) == -1) {
                __HAL_TIM_SET_COUNTER(timHandle, 0);
            }
            uint32 counter = __HAL_TIM_GET_COUNTER(timHandle);
            MemoryOperationsHelper::Copy(copyTable[n].gamPointer, &counter, copyTable[n].copySize);
        }
    }
    return ret;
}

CLASS_REGISTER(ResetEncoderReader, "1.0")
