/*
 * SMConsumerActions.c
 *
 *  Created on: Jun 19, 2022
 *      Author: joaop
 */

#include "main.h"

void ToggleLED(uint8_t quantidade)
{
	for (uint8_t counter = 0; counter < quantidade; counter++)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_Delay(100);

		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
	}
}
