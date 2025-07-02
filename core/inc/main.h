#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

void Error_Handler(void);

#define Led_Pin GPIO_PIN_13
#define Led_GPIO_Port GPIOC

#ifdef __cplusplus
}
#endif

#endif
