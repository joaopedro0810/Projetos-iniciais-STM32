/*
 * SMConsumerActions.c
 *
 *  Created on: Jun 19, 2022
 *      Author: joaop
 */

#include "main.h"

void ToggleLED(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
