/**
 * @file HCTL2022.h
 * @brief Header file for class HCTL2022
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

 * @details This header file contains the declaration of the class HCTL2022
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef HCTL2022_H_
#define HCTL2022_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "DataSourceI.h"
#include "stm32f4xx_hal.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

using namespace MARTe;

class HCTL2022: public DataSourceI {
public:
    CLASS_REGISTER_DECLARATION()

    HCTL2022();
    virtual ~HCTL2022();

    virtual bool Initialise(StructuredDataI &data);

    bool Synchronise();

    bool AllocateMemory();

    virtual uint32 GetNumberOfMemoryBuffers();

    virtual bool SetConfiguredDatabase(MARTe::StructuredDataI & data);

    bool GetSignalMemoryBuffer(const uint32 signalIdx,
                               const uint32 bufferIdx,
                               void *&signalAddress);

    virtual const char8 *GetBrokerName(StructuredDataI &data,
                                       const SignalDirection direction);

    virtual bool GetInputBrokers(ReferenceContainer &inputBrokers,
                                 const char8* const functionName,
                                 void * const gamMemPtr);

    virtual bool GetOutputBrokers(ReferenceContainer &outputBrokers,
                                  const char8* const functionName,
                                  void * const gamMemPtr);

    virtual bool PrepareNextState(const char8 * const currentStateName,
                                  const char8 * const nextStateName);

    void WriteOE(uint8 val);

    void WriteSEL(uint8 val);

    void Reset();

    uint8 ReadByte();

private:
    GPIO_TypeDef* oe;
    GPIO_TypeDef* sel;
    GPIO_TypeDef* reset;
    GPIO_TypeDef* byteReg;
    uint8 oePin;
    uint8 selPin;
    uint8 resetPin;
    uint16 byteRegMask;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* HCTL2022_H_ */

