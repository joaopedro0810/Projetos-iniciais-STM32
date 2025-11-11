#include "stm32f7xx_hal_gpio.h"
#include "main.h"
#include "gpio.h"

void GPIO_init_leds(void)
{
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
}

void GPIO_write_led_2(bool status)
{
	if (status)
	{
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
	}
	else
	{
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
	}
}

void GPIO_write_led_3(bool status)
{
	if (status)
	{
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
	}
	else
	{
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
	}
}
