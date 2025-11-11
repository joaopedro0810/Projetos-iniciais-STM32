#include "stm32f7xx_hal_dac.h"
#include "main.h"
#include "dac.h"

extern DAC_HandleTypeDef hdac;

void DAC_write_channel_1(uint16_t value)
{
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);
}
