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
OBJSX=HCTL2022.x HCTL2022Reader.x

SPB =

MAKEDEFAULTDIR=$(MARTE2_DIR)/MakeDefaults

include $(MAKEDEFAULTDIR)/MakeStdLibDefs.$(TARGET)

BUILD_DIR=../../Build

INCLUDES += -I.
INCLUDES += -I$(FINAL_TARGET_INCLUDES)
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L0Types/
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L1Portability/
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L1Portability/Environment/FreeRTOS/
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L2Objects/
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L3Streams/
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L4Configuration/
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L4Messages/
INCLUDES += -I$(MARTE2_DIR)/Source/Core/BareMetal/L5GAMs/
INCLUDES += -I$(MARTE2_TOOLS)/DataSources/TIM/
INCLUDES += -I$(MARTE2_TOOLS)/DataSources/Encoder/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/BSP/Components/Common/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/CMSIS/Device/ST/STM32F7xx/Include/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/CMSIS/Include/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/BSP/Components/otm8009a/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/
INCLUDES += -I$(STM_CUBE_FW_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include/

all: $(OBJS) $(SUBPROJ) \
	$(BUILD_DIR)/HCTL2022DataSource$(LIBEXT)

include $(MAKEDEFAULTDIR)/MakeStdLibRules.$(TARGET)

