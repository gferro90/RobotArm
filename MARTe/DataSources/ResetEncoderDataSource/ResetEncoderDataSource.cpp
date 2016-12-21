/**
 * @file ResetEncoderDataSource.cpp
 * @brief Source file for class ResetEncoderDataSource
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
 * the class ResetEncoderDataSource (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "ResetEncoderDataSource.h"
#include "ResetEncoderReader.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

ResetEncoderDataSource::ResetEncoderDataSource():EncodersCounterDataSource() {
    // Auto-generated constructor stub for ResetEncoderDataSource
    // TODO Verify if manual additions are needed
}

ResetEncoderDataSource::~ResetEncoderDataSource() {
    // Auto-generated destructor stub for ResetEncoderDataSource
    // TODO Verify if manual additions are needed
}


const char8 *ResetEncoderDataSource::GetBrokerName(StructuredDataI &data,
                                    const SignalDirection direction) {
    if (direction == InputSignals) {
        return "ResetEncoderReader";
    }
    if (direction == OutputSignals) {
        return "TIMBaseWriter";
    }
    return "";
}

bool ResetEncoderDataSource::GetInputBrokers(ReferenceContainer &inputBrokers,
                              const char8* const functionName,
                              void * const gamMemPtr) {
    ReferenceT<ResetEncoderReader> broker("ResetEncoderReader");
    bool ret = broker.IsValid();
    if (ret) {
        ret = broker->Init(InputSignals, *this, functionName, gamMemPtr);
    }
    if (ret) {
        ret = inputBrokers.Insert(broker);
    }
    return ret;
}


CLASS_REGISTER(ResetEncoderDataSource, "1.0")
