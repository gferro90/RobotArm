#!/bin/sh

	grep -q _PATCHED main.c || sed -i '1s/^/extern void UserMainFunction(const void *arg);\r\#include "HandleDatabase.h"\r\n/' main.c
	grep -q _PATCHED main.c || sed -i '/int main(void)/,/while (1)/ s/StartDefaultTask/UserMainFunction/' main.c
	grep -q _PATCHED main.c || sed -i '/int main(void)/,/while (1)/ s/128/2048/' main.c
	grep -q _PATCHED main.c || sed -i 's/int main(void)/UART_HandleTypeDef *errorUartHandle=\&_UART_HANDLE_ERR;\nint main(void)/' main.c
	grep -q _PATCHED main.c || sed -i 's/osKernelStart();/HAL_TIM_Base_Start((TIM_HandleTypeDef*) GetHandle("htim6"));\n\r&/' main.c
	grep -q _PATCHED main.c || sed -i '/USER CODE END Includes/,/USER CODE BEGIN PV/ s/\(HandleTypeDef\) \(.*\).*;/\1 \2; REGISTER_HANDLE(\2, \&\2);/' main.c
	grep -q _PATCHED main.c || sed -i '1s/^/#define _PATCHED\r\n /' main.c
	grep -q 'CalibrateTimer' stm32f4xx_it.c || (sed -i 's/void NMI_Handler(void)/extern void CalibrateTimer();\n&/' stm32f4xx_it.c && sed -i 's/osSystickHandler();/&\nCalibrateTimer();/' stm32f4xx_it.c)
	if [ -f usbd_cdc_if.c ]; then cp usbd_cdc_if_template_c usbd_cdc_if.c; fi
	if [ -f usbd_cdc_if.h ]; then cp usbd_cdc_if_template_h usbd_cdc_if.h; fi	

