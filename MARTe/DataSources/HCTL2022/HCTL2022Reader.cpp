/**
 * @file HCTL2022Reader.cpp
 * @brief Source file for class HCTL2022Reader
 * @date 22/dic/2016
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
 * the class HCTL2022Reader (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "HCTL2022Reader.h"


/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

HCTL2022Reader::HCTL2022Reader() {
    // Auto-generated constructor stub for HCTL2022Reader
    // TODO Verify if manual additions are needed
    dsHandle=NULL;
}

HCTL2022Reader::~HCTL2022Reader() {
    // Auto-generated destructor stub for HCTL2022Reader
    // TODO Verify if manual additions are needed
}

bool HCTL2022Reader::Execute() {
    uint32 n;
    //REPORT_ERROR(ErrorManagement::Warning, "Reading Enc");
    bool ret = true;
    for (n = 0u; (n < numberOfCopies) && (ret); n++) {
        if (copyTable != NULL_PTR(MemoryMapBrokerCopyTableEntry *)) {
            dsHandle->WriteSEL(0u);
            dsHandle->WriteOE(0u);
            uint8 hi=dsHandle->ReadByte;
            dsHandle->WriteSEL(1u);
            uint8 lo=dsHandle->ReadByte;
            dsHandle->WriteOE(1u);

            uint32 counter=(hi<<8)+lo;

            MemoryOperationsHelper::Copy(copyTable[n].gamPointer, &counter, copyTable[n].copySize);
        }
    }
    return ret;
}

bool HCTL2022Reader::Init(const SignalDirection direction,
                          DataSourceI &dataSourceIn,
                          const char8 * const functionName,
                          void * const gamMemoryAddress) {
    bool ret = MemoryMapInputBroker::Init(direction, dataSourceIn, functionName, gamMemoryAddress);
    if (ret) {
        dsHandle = dynamic_cast<HCTL2022>(dataSource);
        ret = (dsHandle != NULL);
    }
    return ret;
}

CLASS_REGISTER(HCTL2022Reader, "1.0")
