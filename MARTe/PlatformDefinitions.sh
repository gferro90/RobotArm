#!/bin/sh

#Exports needed by the project
export MARTe2_DIR=/home/giuseppe/MARTe2Project/GIT/MARTe2
export MARTe2_Embedded_Tools_DIR_BASE=/home/giuseppe/MARTe2Project/GIT/MARTe2_embedded_tools
export MARTe2_Embedded_Tools_DIR=$MARTe2_Embedded_Tools_DIR_BASE/MARTe2-platforms/STM32
export MARTe2_Components_DIR=/home/pc/MARTe2Project/GIT/MARTe2-components
export TARGET=arm_cort_m4_stm
export SPEC_DEFS='-DSTM32F407xx -DSTM32F4_Discovery -DUSE_FREERTOS -D_HAL_H="stm32f4xx_hal.h" -D__TIMER__NAME__=TIM6 -D__TIMER__NAME__=TIM6 -D_UART_HANDLE_ERR=huart2 -DERROR_ON_USB'
export SCRIPT_DIR=$MARTe2_Embedded_Tools_DIR/Scripts
export SCHEDULER_DIR=$MARTe2_Embedded_Tools_DIR/Scheduler
export PROJECT_NAME=RobotArm
export PROJECT_CONF_INCLUDES=$PWD/ToolConfiguration
export DRIVER_PLATFORM=STM32F4xx
export LINKER_LD_FILE=$PWD/stm32_flash.ld
