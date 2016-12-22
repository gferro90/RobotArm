/**
 * @file HCTL2022.cpp
 * @brief Source file for class HCTL2022
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
 * the class HCTL2022 (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "HCTL2022.h"

extern "C" {
void * GetHwHandle(const char*);
}

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

HCTL2022::HCTL2022() {
    // Auto-generated constructor stub for HCTL2022
    // TODO Verify if manual additions are needed
    oe = NULL;
    sel = NULL;
    reset = NULL;
    byteReg = NULL;

    oePin = 0u;
    selPin = 0u;
    resetPin = 0u;
    byteRegMask = 0u;
}

HCTL2022::~HCTL2022() {
    // Auto-generated destructor stub for HCTL2022
    // TODO Verify if manual additions are needed
}

bool HCTL2022::Initialise(StructuredDataI &data) {
    bool ret = DataSourceI::Initialise(data);
    if (ret) {
        StreamString oeId;
        ret = data.Read("OE_Port", oeId);
        if (ret) {
            oe = (GPIO_TypeDef *) GetHwHandle(oe.Buffer());
            ret = (oe != NULL);
        }
        if (!ret) {
            REPORT_ERROR(ErrorManagement::Warning, "No OE port specified");
        }
        if (ret) {
            StreamString selId;
            ret = data.Read("SEL_Port", selId);
            if (ret) {
                sel = (GPIO_TypeDef *) GetHwHandle(selId.Buffer());
                ret = (sel != NULL);
            }
            if (!ret) {
                REPORT_ERROR(ErrorManagement::Warning, "No SEL port specified");
            }
        }
        if (ret) {
            StreamString resetId;
            ret = data.Read("RESET_Port", resetId);
            if (ret) {
                reset= (GPIO_TypeDef *) GetHwHandle(resetId.Buffer());
                ret = (reset!= NULL);
            }
            if (!ret) {
                REPORT_ERROR(ErrorManagement::Warning, "No RESET port specified");
            }
        }
        if (ret) {
            StreamString regId;
            ret = data.Read("ByteReg_Port", regId);
            if (ret) {
                byteReg = (GPIO_TypeDef *) GetHwHandle(regId.Buffer());
                ret = (byteReg != NULL);
            }
            if (!ret) {
                REPORT_ERROR(ErrorManagement::Warning, "No ByteReg port specified");
            }
        }
        if (ret) {
            ret = data.Read("OE_Pin", oePin);
            if (ret) {
                ret = (oePin <16u);
            }
            if (!ret) {
                REPORT_ERROR(ErrorManagement::Warning, "No OE pin specified");
            }
        }
        if (ret) {
            ret = data.Read("SEL_Pin", selPin);
            if (ret) {
                ret = (selPin <16u);
            }
            if (!ret) {
                REPORT_ERROR(ErrorManagement::Warning, "No SEL pin specified");
            }
        }
        if (ret) {
            ret = data.Read("RESET_Pin", resetPin);
            if (ret) {
                ret = (resetPin <16u);
            }
            if (!ret) {
                REPORT_ERROR(ErrorManagement::Warning, "No RESET pin specified");
            }
        }
        if (ret) {
            ret = data.Read("ByteReg_Mask", byteRegMask);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::Warning, "No ByteReg mask specified");
            }
        }
    }

    WriteOE(1u);
    reset->BSRR|=(1<<resetPin);


    return ret;
}

bool HCTL2022::Synchronise() {
    return true;
}

bool HCTL2022::AllocateMemory() {
    return true;
}

uint32 HCTL2022::GetNumberOfMemoryBuffers() {

    return 1u;
}

bool HCTL2022::SetConfiguredDatabase(MARTe::StructuredDataI & data) {
    bool ret = DataSourceI::SetConfiguredDatabase(data);
    uint32 numberOfSignals = GetNumberOfSignals();

    for (uint32 i = 0u; i < numberOfSignals && ret; i++) {
        ret = (GetSignalType(i) == TypeDescriptor::GetTypeDescriptorFromTypeName("uint32"));
        if (!ret) {
            REPORT_ERROR(ErrorManagement::InitialisationError, "The HCTL2022DataSource signal type has to be uint32");
        }
    }
    return ret;
}

bool HCTL2022::GetSignalMemoryBuffer(const uint32 signalIdx,
                                     const uint32 bufferIdx,
                                     void *&signalAddress) {
    signalAddress = NULL;
    return true;
}

const char8 *HCTL2022::GetBrokerName(StructuredDataI &data,
                                     const SignalDirection direction) {
    if (direction == InputSignals) {
        return "HCTL2022Reader";
    }
    return "";
}

bool HCTL2022::GetInputBrokers(ReferenceContainer &inputBrokers,
                               const char8* const functionName,
                               void * const gamMemPtr) {
    ReferenceT < HCTL2022Reader > broker("HCTL2022Reader");
    bool ret = broker.IsValid();
    if (ret) {
        ret = broker->Init(InputSignals, *this, functionName, gamMemPtr);
    }
    if (ret) {
        ret = inputBrokers.Insert(broker);
    }
    return ret;
}

bool HCTL2022::GetOutputBrokers(ReferenceContainer &outputBrokers,
                                const char8* const functionName,
                                void * const gamMemPtr) {
    return false;
}

bool HCTL2022::PrepareNextState(const char8 * const currentStateName,
                                const char8 * const nextStateName) {
    return true;
}

void HCTL2022::WriteOE(uint8 val) {
    (val)?(oe->BSRR|=(1<<oePin)):(oe->BSRR|=(1<<(16+oePin)));
}

void HCTL2022::WriteSEL(uint8 val) {
    (val)?(sel->BSRR|=(1<<oePin)):(sel->BSRR|=(1<<(16+oePin)));
}

void Reset(){
    reset->BSRR|=(1<<(16+resetPin));
    //TODO delay?
    reset->BSRR|=(1<<(resetPin));

}


uint8 HCTL2022::ReadByte() {
    uint16 regPort=byteReg->IDR;
    uint8 ret=0u;
    uint8 cnt=0u;
    for(uint8 i=0u; i<16u; i++){
        if((byteRegMask>>i)&0x1){
            ret|=(((regPort>>i)&0x1u)<<cnt);
            cnt++;
        }
    }
    return ret;
}


CLASS_REGISTER(HCTL2022, "1.0")
