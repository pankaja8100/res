#include "main.h"
#include "stm32f1xx_it.h"

extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim2;

void NMI_Handler(void)
{
  HAL_RCC_NMI_IRQHandler();
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}

void I2C2_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c2);
}

void I2C2_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c2);
}
