#############################################################
#
# Copyright 2015 F4E | European Joint Undertaking for ITER 
#  and the Development of Fusion Energy ('Fusion for Energy')
#
# Licensed under the EUPL, Version 1.1 or - as soon they 
# will be approved by the European Commission - subsequent  
# versions of the EUPL (the "Licence"); 
# You may not use this work except in compliance with the 
# Licence. 
# You may obtain a copy of the Licence at: 
#  
# http://ec.europa.eu/idabc/eupl
#
# Unless required by applicable law or agreed to in 
# writing, software distributed under the Licence is 
# distributed on an "AS IS" basis, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
# express or implied. 
# See the Licence for the specific language governing 
# permissions and limitations under the Licence. 
#
# $Id: Makefile.inc 3 2012-01-15 16:26:07Z aneto $
#
#############################################################

MAKEDEFAULTDIR=$(MARTe2_Embedded_Tools_DIR_BASE)/MakeDefaults

C_FILES = $(wildcard *.c)
CPP_FILES = $(wildcard *.cpp)


OBJSX := $(C_FILES:.c=.x)
OBJSX += $(CPP_FILES:.cpp=.x)

include $(MAKEDEFAULTDIR)/MakeStdLibDefs.$(TARGET)

BUILD_DIR=../Build

INCLUDES += -I.
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/CMSIS/Include/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/$(DRIVER_PLATFORM)_HAL_Driver/Inc/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/CMSIS/Device/ST/$(DRIVER_PLATFORM)/Include/
INCLUDES += -I$(SCHEDULER_DIR)


#CFLAGS += -DconfigCHECK_FOR_STACK_OVERFLOW=1
#CFLAGS += -DINCLUDE_uxTaskGetStackHighWaterMark=1
CFLAGS += -DINCLUDE_xTaskGetCurrentTaskHandle=1

all: update \
	$(OBJS) \
	$(BUILD_DIR)/ToolConfiguration$(DLLEXT) \
	$(BUILD_DIR)/ToolConfiguration$(LIBEXT) \
	copy_startup

update:
	ls stm32f4xx.h || python3 STM32ExtractFiles.py $(PROJECT_NAME) $(PROJECT_NAME) .
	./update.sh
	
copy_startup:
	cp startup*.s ../

include $(MAKEDEFAULTDIR)/MakeStdLibRules.$(TARGET)

checkdefs:
ifndef STM_CUBE_FW_DIR
	$(error Please export in the variable STM_CUBE_FW_DIR the location of the STM Cube FW.)
endif
ifndef MARTe2_DIR
	$(error Please export in the variable MARTe2_DIR the location of folder where MARTe2 is installed.)
endif
ifndef MARTe2_Components_DIR
	$(error Please export in the variable MARTe2_DIR the location of folder where MARTe2 is installed.)
endif
ifndef TARGET
	$(error Please export in the variable TARGET the target platform (e.g. armv7em-freertos.)
endif


