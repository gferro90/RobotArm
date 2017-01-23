/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id: USBDrv.h 3 2012-01-15 16:26:07Z aneto $
 *
**/

/**
 * @file USB driver which can be used to connect several MARTe systems or to
 * interface into external systems. For data streaming see also the StreamingDriver.
 * This code is heavily based in the ATMDrv, but in this case the header is implicit 
 * (not retrieved from the DDB) when used in USB_MODULE_TRANSMITTER mode
 */
#if !defined (_USB_DRV)
#define _USB_DRV

#include "System.h"
#include "GenericAcqModule.h"
#include "File.h"

/// Number buffers for data storage
static const int32 nOfDataBuffers = 3;
OBJECT_DLL(USBDrv)

// Callback declaration
 void ReceiverCallback(void *userData);

/// USB Module Class
class USBDrv : public GenericAcqModule {

OBJECT_DLL_STUFF(USBDrv)

/// Receiver CallBack
friend void ReceiverCallback(void *userData);

private:
    
    /**   Input/Output Buffer Size in Byte */
    int32                  packetByteSize;

    /** Fast mutex to protect reading and writing buffer */
    FastPollingMutexSem    mux;


    /**   Init all module entries */
    bool                   Init();



///////////////////////////////////////////////////////////////////////////////
//                           Receiver Type Module                            //
///////////////////////////////////////////////////////////////////////////////
private:

    /** Triple buffer for receiver type module */
    uint32                 *dataBuffer;

    int32 cycleCounter;

    /** Index of the write only buffer.
        The next write only buffer index is equal to (writeBuffer+1)%3
        The read only buffer index is equal to (writeBuffer+2)%3 */
    int32                   writeBuffer;
    
    /** */
    int32                   globalReadBuffer;

    /** Max data age in usec. It is used to decided if the read data are ready or not */
    int32                  maxDataAgeUsec;

    /** CPU mask of the receiver thread (in case it's a receiver module) */
    int32                  cpuMask;


    int                  inputsFromHttp[2];

    bool                 boardIsOn;

    FString objectPathStr;

    /** Enable the System Acquisition */
    bool EnableAcquisition();

    /** Disable the System Acquisition */
    bool DisableAcquisition();

    /** Receive callback method */
    void RecCallback(void* arg);

    /** Flag to control the receiving thread */    
    bool                    keepRunning;


     /** Dumps Input Data to the Stream
        @param s output stream
     */
    bool InputDump(StreamInterface &s) const;


public:

    /** Gets Data From the Module to the DDB
        @param usecTime Microseconds Time
        @return -1 on Error, 1 on success
    */
    int32 GetData(uint32 usecTime, int32 *buffer, int32 bufferNumber = 0);

///////////////////////////////////////////////////////////////////////////////
//                           Transmitter Type Module                         //
///////////////////////////////////////////////////////////////////////////////
private:

    /** The actual packet to be sent */
    void                  *outputPacket;

    /** Dumps the module outputs on the Stream */
    bool OutputDump(StreamInterface &s)const;

public:

    /** Sends the data in the DDB to the USB switch */
    bool WriteData(uint32 usecTime, const int32 *buffer);


///////////////////////////////////////////////////////////////////////////////
//                           Init & Mixed routine                            //
///////////////////////////////////////////////////////////////////////////////
private:    

    /** Copy constructors (since it is defined private it won't allow a public use!!) */
    USBDrv(const USBDrv&){};

    /** Operator=  (since it is defined private it won't allow a public use!!) */
    USBDrv& operator=(const USBDrv&){};

    /** Connection socket */
    int usbFile;

    int uartFile;

    int stmCfgFile;
public:

    /** Constructor */
    USBDrv();

    /** Deconstructor */
    ~USBDrv();

    /** Load Object Parameters from the ConfigurationDataBase */
    virtual bool ObjectLoadSetup(ConfigurationDataBase &info,StreamInterface *err);

    /** Saves Object Parameters to the ConfigurationDataBase */
    virtual bool ObjectSaveSetup(ConfigurationDataBase &info,StreamInterface *err){return True;};

    /** Object Description */
    virtual bool ObjectDescription(StreamInterface &s,bool full,StreamInterface *err);

    /** Set board used as input */
    virtual bool SetInputBoardInUse(bool on = True) {
        return true;
    }

    /** Set board used as output */
    virtual bool SetOutputBoardInUse(bool on = True) {
        return true;
    }

///////////////////////////////////////////////////////////////////////////////
//                           From Time Module                                //
///////////////////////////////////////////////////////////////////////////////

public:

    // Get the Time
    int64  GetUsecTime();

    virtual bool Poll();

///////////////////////////////////////////////////////////////////////////////
//                                  General                                  //
///////////////////////////////////////////////////////////////////////////////

    // Serve webpage
    bool   ProcessHttpMessage(HttpStream &hStream);

    // Called at pulse start
    bool PulseStart() {
        cycleCounter=0;
        return True;
    }
};

#endif

