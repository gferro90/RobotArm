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

MAKEDEFAULTDIR=$(MARTE2_DIR)/MakeDefaults

C_FILES = $(wildcard *.c)
#S_FILES = $(wildcard *.s)
#LINKER_LD_FILE = $(shell ls ../*.ld -1 | tail -n 1)

OBJSX := $(C_FILES:.c=.x)
#OBJSX += $(S_FILES:.s=.x)

include $(MAKEDEFAULTDIR)/MakeStdLibDefs.$(TARGET)

BUILD_DIR=../MARTe/Build

INCLUDES += -I.
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/CMSIS/Include/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include/
#INCLUDES += -I$(STM_CUBE_FW_DIR)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/
#INCLUDES += -I$(STM_CUBE_FW_DIR)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc/

CFLAGS += -DconfigCHECK_FOR_STACK_OVERFLOW=1 
CFLAGS += -DINCLUDE_uxTaskGetStackHighWaterMark=1
CFLAGS += -DINCLUDE_xTaskGetCurrentTaskHandle=1

all: update_main \
	$(OBJS) \
	$(BUILD_DIR)/ToolConfigurationLib$(LIBEXT) \
	copy_startup

update_main:
	grep -q UserMainFunction main.c || sed -i '1s/^/extern void UserMainFunction(const void *arg);\r\n /' main.c
	sed -i '/int main(void)/,/while (1)/ s/StartDefaultTask/UserMainFunction/' main.c
	grep -q 'osThreadDef' main.c || sed -i 's/osKernelStart();/osThreadDef(UserMainThread, UserMainFunction, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); osThreadCreate(osThread(UserMainThread), NULL);&/' main.c
	ls Addons.hdd && grep -q 'Addons' main.c || sed -i 's/int main(void)/#include "Addons.hdd"\n\r&/' main.c
	if [ -f usbd_cdc_if.c ]; then cp usbd_cdc_if_template_c usbd_cdc_if.c; fi
	if [ -f usbd_cdc_if.h ]; then cp usbd_cdc_if_template_h usbd_cdc_if.h; fi
	
copy_startup:
	cp startup*.s ../MARTe

include $(MAKEDEFAULTDIR)/MakeStdLibRules.$(TARGET)

checkdefs:
ifndef STM_CUBE_FW_DIR
	$(error Please export in the variable STM_CUBE_FW_DIR the location of the STM Cube FW.)
endif
ifndef MARTE2_DIR
	$(error Please export in the variable MARTE2_DIR the location of folder where MARTe2 is installed.)
endif
ifndef TARGET
	$(error Please export in the variable TARGET the target platform (e.g. armv7em-freertos.)
endif


