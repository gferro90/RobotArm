/*
 * ErrorFunction.cpp
 *
 *  Created on: 26/ago/2016
 *      Author: pc
 */

#include "stm32f4xx_hal.h"
#include "StreamString.h"
#include "StreamMemoryReference.h"
#include "task.h"
#include "MemoryOperationsHelper.h"
#include "usbd_cdc_if.h"

using namespace MARTe;

extern UART_HandleTypeDef *errorUartHandle;

static char8 buffer[128] = { 0 };
static char8 buffer2[128] = { 0 };
#if 0
void PrintStack(ThreadIdentifier &tid) {
    while (1) {
        MemoryOperationsHelper::Set(buffer2, 0, 128);

        StreamMemoryReference buffer2_stream(buffer2, 128);

        buffer2_stream.Seek(0);
        uint32 stack_remained = uxTaskGetStackHighWaterMark(tid);

//allErrors.Printf(" File %s Function %s Line %d", errorInfo.fileName, errorInfo.functionName, errorInfo.header.lineNumber);
        buffer2_stream.Printf("Stack remained %d\n\r", stack_remained);
        uint32_t size = buffer2_stream.Size() + 1;
        if (!USBInitialized()) {
            USBOpen();
        }
        USBWrite(buffer2_stream.Buffer(), &size, 0);
        Sleep::Sec(20.);
    }
}

void DebugErrorProcessFunction(const MARTe::ErrorManagement::ErrorInformation &errorInfo,
                               const char * const errorDescription) {
//StackType_t *stackTop=(StackType_t*) xTaskGetCurrentTaskHandle();
    MemoryOperationsHelper::Set(buffer, 0, 128);
    StreamMemoryReference buffer_stream(buffer, 128);
    buffer_stream.Seek(0);
    buffer_stream.Printf("%s\n\r", errorDescription);

    uint32_t size = buffer_stream.Size() + 1;
    if (!USBInitialized()) {
        USBOpen();
    }
    USBWrite(buffer, &size, 0);
}
#endif

void PrintStack(ThreadIdentifier &tid) {
    while (1) {
        MemoryOperationsHelper::Set(buffer2, 0, 128);

        StreamMemoryReference buffer2_stream(buffer2, 128);

        buffer2_stream.Seek(0);
        uint32 stack_remained = uxTaskGetStackHighWaterMark(tid);

        //allErrors.Printf(" File %s Function %s Line %d", errorInfo.fileName, errorInfo.functionName, errorInfo.header.lineNumber);
        buffer2_stream.Printf("Stack remained %d\n\r", stack_remained);
        uint32_t size = buffer2_stream.Size() + 1;
        HAL_UART_Transmit(errorUartHandle, (uint8_t*) buffer, size, HAL_MAX_DELAY);

        Sleep::Sec(20.);
    }
}

void DebugErrorProcessFunction(const MARTe::ErrorManagement::ErrorInformation &errorInfo,
        const char * const errorDescription) {
//	StackType_t *stackTop=(StackType_t*) xTaskGetCurrentTaskHandle();
    MemoryOperationsHelper::Set(buffer, 0, 128);
    StreamMemoryReference buffer_stream(buffer, 128);
    buffer_stream.Seek(0);
    buffer_stream.Printf("%s\n\r", errorDescription);

    uint32_t size = buffer_stream.Size() + 1;
    HAL_UART_Transmit(errorUartHandle, (uint8_t*) buffer, size, HAL_MAX_DELAY);

}
