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
 * $Id: USBDrv.cpp 3 2012-01-15 16:26:07Z aneto $
 *
 **/

#include "Endianity.h"
#include "CDBExtended.h"
#include "FastPollingMutexSem.h"
#include "USBDrv.h"
#include "Sleep.h"
#include "stdio.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//#define SEND_USB
//#define TEST_SCHEDULER
//#define TEST_INTERRUPT_SCHEDULER

static int32 GetBaudRate(int32 speedVal) {

    int32 speeds[] = { B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, 0 };
    int32 speedValues[] = { 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 0 };

    int32 i = 0;
    while (speedValues[i] != 0) {
        if (speedValues[i] == speedVal) {
            return speeds[i];
        }
        i++;
    }
    return 0;

}

/**
 * Enable System Acquisition
 */
bool USBDrv::EnableAcquisition() {
    return true;
}

/**
 * Disable System Acquisition
 */
bool USBDrv::DisableAcquisition() {
    close(usbFile);
    return true;
}

/*
 * Constructors
 */
bool USBDrv::Init() {
    // Init general parameters
    mux.Create();
    packetByteSize = 0;
    // Init receiver parameters
    writeBuffer = 0;
    keepRunning = false;
    cpuMask = 0xFFFF;
    usbFile = -1;
    uartFile = -1;
    stmCfgFile = -1;
    // reset all buffers pointers 
    dataBuffer = NULL;
    cycleCounter = 0;
    inputsFromHttp[0] = 0;
    inputsFromHttp[1] = 0;
    boardIsOn = false;
    return true;
}

USBDrv::USBDrv() {
    Init();
}

/*
 * Destructor
 */
USBDrv::~USBDrv() {
    keepRunning = false;
    DisableAcquisition();

    // Free memory
    if (dataBuffer != NULL)
        free((void *&) dataBuffer);
}

/**
 * ObjectLoadSetup
 */
bool USBDrv::ObjectLoadSetup(ConfigurationDataBase &info,
                             StreamInterface *err) {

    SleepMsec(15000);

    printf("\n0\n");

    // Disable previous opened connections
    DisableAcquisition();
    // Parent class Object load setup
    CDBExtended cdb(info);
    if (!GenericAcqModule::ObjectLoadSetup(info, err)) {
        AssertErrorCondition(InitialisationError, "USBDrv::ObjectLoadSetup: %s GenericAcqModule::ObjectLoadSetup Failed", Name());
        return false;
    }
    cpuMask = 0xFFFF;
    printf("\n1\n");

    int32 speedValue = 0;
    if (!cdb.ReadInt32(speedValue, "BaudRate", 115200)) {
        AssertErrorCondition(Warning, "USBDrv::ObjectLoadSetup: %s Baud Rate not specified. Using default: %d", Name(), speedValue);
    }
    FString usbFileLocation;
    if (!cdb.ReadFString(usbFileLocation, "USBFileLocation", "/dev/ttyACM0")) {
        AssertErrorCondition(Warning, "USBDrv::Initialise not specified. Using default:%s", usbFileLocation.Buffer());
    }

    printf("\n2\n");

    uint32 j = 0;
    //Wait ten seconds before exit
    while (usbFile < 0) {
        usbFile = open(usbFileLocation.Buffer(), O_RDWR | O_NOCTTY | O_SYNC);
        if (j++ > 1000) {
            AssertErrorCondition(InitialisationError, "USBDrv::Timeout expired waiting the open of the usb file: %s", usbFileLocation.Buffer());
            return false;
        }
        SleepMsec(10);

    }

    int32 speed = GetBaudRate(speedValue);

    int32 parity = 0;

    struct termios ttyUSB;
    memset(&ttyUSB, 0, sizeof ttyUSB);
    if (tcgetattr(usbFile, &ttyUSB) != 0) {
        return false;
    }

    cfsetospeed(&ttyUSB, speed);
    cfsetispeed(&ttyUSB, speed);

    ttyUSB.c_cflag = (ttyUSB.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    ttyUSB.c_iflag &= ~IGNBRK;         // disable break processing
    ttyUSB.c_lflag = 0;                // no signaling chars, no echo,
                                       // no canonical processing
    ttyUSB.c_oflag = 0;                // no remapping, no delays
    ttyUSB.c_cc[VMIN] = 1;            // read  block
    ttyUSB.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
    ttyUSB.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    ttyUSB.c_cflag |= (CLOCAL | CREAD); // ignore modem controls,
                                        // enable reading
    ttyUSB.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    ttyUSB.c_cflag |= parity;
    ttyUSB.c_cflag &= ~CSTOPB;
    ttyUSB.c_cflag &= ~CRTSCTS;

    if (tcsetattr(usbFile, TCSANOW, &ttyUSB) != 0) {
        return false;
    }

    tcflush(usbFile, TCIOFLUSH);
#if 0
    FString uartFileLocation;
    if (!cdb.ReadFString(uartFileLocation, "UARTFileLocation", "/dev/ttyUSB0")) {
        AssertErrorCondition(Warning, "USBDrv::Initialise not specified. Using default:%s", uartFileLocation.Buffer());
    }

    uartFile = open(uartFileLocation.Buffer(), O_RDWR | O_NOCTTY);
    if (uartFile < 0) {
        printf("\nUART not opened\n");
        AssertErrorCondition(Warning, "USBDrv::Failed opening the uart file: %s", uartFileLocation.Buffer());
    }
    else {

        struct termios ttyUART;
        memset(&ttyUART, 0, sizeof ttyUART);
        if (tcgetattr(uartFile, &ttyUART) != 0) {
            return false;
        }
        ttyUART.c_cflag = B9600 | CS8 | CLOCAL | CREAD;      //<Set baud rate
        ttyUART.c_iflag = IGNPAR;
        ttyUART.c_oflag = 0;
        ttyUART.c_lflag = 0;
        ttyUART.c_cc[VMIN] = 1;// read  block
        tcflush(uartFile, TCIOFLUSH);

        if (tcsetattr(uartFile, TCSANOW, &ttyUART) != 0) {
            return false;
        }

    }
#endif
    printf("\n3\n");

    //Read destinationServerAddress destinationServerPort serverPort....
    if (numberOfInputChannels < 1) {
        AssertErrorCondition(InitialisationError, "USBDrv::EnableAcquisition: At least 1 input channels must be specified for the header");
        return false;
    }

    printf("\n4\n");

    // Read cpu mask
    if (!cdb.ReadInt32(cpuMask, "CpuMask", 0xFFFF)) {
        AssertErrorCondition(Warning, "USBDrv::ObjectLoadSetup: %s CpuMask was not specified. Using default: %d", Name(), cpuMask);
    }

    // Create Data Buffers. Compute total size and allocate storing buffer 
    packetByteSize = numberOfInputChannels * sizeof(uint32);
    dataBuffer = (uint32 *) malloc(packetByteSize);

    if (!EnableAcquisition()) {
        AssertErrorCondition(InitialisationError, "USBDrv::ObjectLoadSetup Failed Enabling Acquisition");
        return false;
    }

    printf("\n5\n");

    // Tell user the initialization phase is done
    AssertErrorCondition(Information, "USBDrv::ObjectLoadSetup:: USB Module %s Correctly Initialized", Name());

    printf("\n6\n");
#if 0
    FString stmCfgFileLocation;
    if (!cdb.ReadFString(stmCfgFileLocation, "StmCFG", "MARTe-Stm-BareMetal.cfg")) {
        AssertErrorCondition(Warning, "StmCFG::Initialise not specified. Using default:%s", usbFileLocation.Buffer());
    }

    stmCfgFile = open(stmCfgFileLocation.Buffer(), O_RDWR);
    if (stmCfgFile < 0) {
        AssertErrorCondition(InitialisationError, "USBDrv::STM cfg file does not exist in %s", stmCfgFileLocation.Buffer());
        return false;
    }
#endif
    //int uartFile = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_SYNC);
//    uint32 totalSize = strlen(toSend);
   /* const uint32 packetSize = 63;
    const uint32 buffSize = 64;

    char buff[buffSize];
    uint32 sizeToW = buffSize;

    uint32 written = 0;
    memset(buff, '@', sizeToW);
    do {
        sizeToW -= written;
        written += write(usbFile, buff + written, sizeToW);
    }
    while (written < sizeToW);
    SleepMsec(1000);
    printf("\n7\n");*/

#ifdef SEND_USB
    bool end = false;

    //for (uint32 index = 0; index < totalSize; index += packetSize) {
    while (!end) {
        sizeToW = buffSize;
        memset(buff, 0, sizeToW);

        uint32 max = packetSize;
        if ((max = read(stmCfgFile, buff, max)) <= 0) {
            break;
        }

        if (max < packetSize) {
            buff[packetSize] = '@';
            end = true;
        }
        /*uint32 max = (totalSize - index);
         if (max > packetSize) {
         max = packetSize;
         }
         else {
         buff[packetSize] = '@';
         }
         memcpy(buff, toSend + index, max);
         */
        written = 0;
        uint32 iteration = 0;

        do {
            sizeToW -= written;
            written += write(usbFile, buff + written, sizeToW);
            iteration++;
        }
        while (written < sizeToW);
        buff[packetSize] = 0;
        printf("\n%d bytes written on %d, %d iteration:\n [%s]", written, sizeToW, iteration, buff);

        SleepMsec(100);
    }
#endif
    return true;
}

/**
 * GetData
 */
int32 USBDrv::GetData(uint32 usecTime,
                      int32 *buffer,
                      int32 bufferNumber) {
    memcpy(buffer, dataBuffer, packetByteSize);
    return 1;
}

/**
 * WriteData
 */
bool USBDrv::WriteData(uint32 usecTime,
                       const int32 *buffer) {
    return false;
}

/**
 * InputDump
 */
bool USBDrv::InputDump(StreamInterface &s) const {
    return true;
}

/**
 * OutputDump
 */
bool USBDrv::OutputDump(StreamInterface &s) const {
    return true;
}

/**
 * GetUsecTime
 */
int64 USBDrv::GetUsecTime() {
    return dataBuffer[0];
}

/**
 * ObjectDescription
 */
bool USBDrv::ObjectDescription(StreamInterface &s,
                               bool full,
                               StreamInterface *err) {
    s.Printf("%s %s\n", ClassName(), Version());
    // Module name
    s.Printf("Module Name --> %s\n", Name());

    return true;
}

bool USBDrv::ProcessHttpMessage(HttpStream &hStream) {
    hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html");
    hStream.keepAlive = false;

    FString changeStateStr;
    changeStateStr.SetSize(0);
    if (hStream.Switch("InputCommands.changeState")) {
        hStream.Seek(0);
        hStream.GetToken(changeStateStr, "");
        hStream.Switch((uint32) 0);
    }
    FString sendStr;
    sendStr.SetSize(0);
    if (hStream.Switch("InputCommands.send")) {
        hStream.Seek(0);
        hStream.GetToken(sendStr, "");
        hStream.Switch((uint32) 0);
    }

    FString increaseStr;
    increaseStr.SetSize(0);
    if (hStream.Switch("InputCommands.increase")) {
        hStream.Seek(0);
        hStream.GetToken(increaseStr, "");
        hStream.Switch((uint32) 0);
    }

    FString decreaseStr;
    decreaseStr.SetSize(0);
    if (hStream.Switch("InputCommands.decrease")) {
        hStream.Seek(0);
        hStream.GetToken(decreaseStr, "");
        hStream.Switch((uint32) 0);
    }

    if (changeStateStr == "1") {
        inputsFromHttp[0] = (int) -1;
        FString stateStr;
        stateStr.SetSize(0);
        if (hStream.Switch("InputCommands.state")) {
            hStream.Seek(0);
            hStream.GetToken(stateStr, "");
            hStream.Switch((uint32) 0);
            inputsFromHttp[1] = (int) atoi(stateStr.Buffer());
        }

        bool boardState = boardIsOn;
        boardIsOn = (inputsFromHttp[1] != -1);
        if (!boardState && boardIsOn) {
            (printf("\nSend Initial packet...!\n"));
            const uint32 packetSize = 63;
            const uint32 buffSize = 64;

            char buff[buffSize];
            uint32 sizeToW = buffSize;

            uint32 written = 0;
            memset(buff, '@', sizeToW);
            do {
                sizeToW -= written;
                written += write(usbFile, buff + written, sizeToW);
            }
            while (written < sizeToW);
        }
        boardIsOn ? (printf("\nBoard Is On!\n")) : (printf("\nBoard Is Off!\n"));

    }
    else {
        FString motorNumberStr;
        motorNumberStr.SetSize(0);
        if (hStream.Switch("InputCommands.motorNumber")) {
            hStream.Seek(0);
            hStream.GetToken(motorNumberStr, "");
            hStream.Switch((uint32) 0);
            inputsFromHttp[0] = (int) atoi(motorNumberStr.Buffer());
        }
        if (sendStr == "1") {
            FString motorReferenceStr;
            motorReferenceStr.SetSize(0);
            if (hStream.Switch("InputCommands.motorReference")) {
                hStream.Seek(0);
                hStream.GetToken(motorReferenceStr, "");
                hStream.Switch((uint32) 0);
                inputsFromHttp[1] = (int) atoi(motorReferenceStr.Buffer());
            }
        }

        if (increaseStr == "1") {
            inputsFromHttp[1] = 1;
        }
        if (decreaseStr == "1") {
            inputsFromHttp[1] = -1;
        }
    }

    hStream.Printf("<html><head><title>USBDrv</title></head>\n");
    hStream.Printf("<body>\n");

    hStream.Printf("Motor number:<br>\n");
    hStream.Printf("<form><input type=\"text\" name=\"motorNumber\">");
    hStream.Printf("<button type=\"submit\" name=\"increase\" value=\"1\">+</button>\n");
    hStream.Printf("<button type=\"submit\" name=\"decrease\" value=\"1\">-</button><br>\n");

    hStream.Printf("Reference:<br>\n");
    hStream.Printf("<input type=\"text\" name=\"motorReference\"><br>");

    hStream.Printf("<button type=\"submit\" name=\"send\" value=\"1\">Send</button><br>\n");

    hStream.Printf("State:<br>\n");
    hStream.Printf("<input type=\"text\" name=\"state\"><br>");
    hStream.Printf("<button type=\"submit\" name=\"changeState\" value=\"1\">Change State</button>");
    hStream.Printf("</form>\n</body></html>\n");
    //copy to the client
    hStream.WriteReplyHeader(true);

    write(usbFile, inputsFromHttp, (2 * sizeof(int)));
    printf("\n %d %d\n", inputsFromHttp[0], inputsFromHttp[1]);

#if 0
    const uint32 buffSize = 128;
    if (objectPathStr.Size() > 0 && objectPathStr.Size() < buffSize) {
        char toSend[buffSize];
        memset(toSend, 0, buffSize);

        memcpy(toSend, objectPathStr.Buffer(), objectPathStr.Size());
        //  write(usbFile, toSend, buffSize);
        for (uint32 k = 0; k < buffSize; k++) {
            write(uartFile, toSend + k, 1);
            SleepMsec(10);
        }
        tcflush(uartFile, TCIOFLUSH);
        printf("\n %f %f %s\n", inputsFromHttp[0], inputsFromHttp[1], toSend);

        /*uint32 packetSize = 64;
         FString reply;
         int32 ret = 1;
         char c = 'a';
         while (ret > 0) {
         reply="";
         while (c != 0) {
         if ((ret = read(uartFile, &c, 1)) > 0) {
         reply += c;
         }
         else {
         c = 0;
         }
         }
         printf("\n%s\n", reply.Buffer());
         }*/
    }
#endif
    return true;
}

bool USBDrv::Poll() {
    uint32 defaultSize = ((numberOfInputChannels - 1) * sizeof(uint32));
    *dataBuffer = cycleCounter;

    int32 nRead = 0;
    while ((nRead += read(usbFile, dataBuffer + 1 + nRead, defaultSize)) < defaultSize) {
        defaultSize -= nRead;
    }

    cycleCounter++;
    for (int i = 0; i < nOfTriggeringServices; i++) {
        triggerService[i].Trigger();
    }
    return true;
}

OBJECTLOADREGISTER(USBDrv, "$Id: USBDrv.cpp 3 2012-01-15 16:26:07Z aneto $")
