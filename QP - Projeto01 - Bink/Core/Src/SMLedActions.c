/*
 * SMLedActions.c
 *
 *  Created on: Jun 19, 2022
 *      Author: joaop
 */


#include "main.h"

void LigarLED(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void DesligarLED(void)
{
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}
