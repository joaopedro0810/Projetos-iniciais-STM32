#include "stm32f7xx_hal_uart.h"
#include "Modbus.h"
#include "uart.h"

extern UART_HandleTypeDef huart2;

extern uint8_t buffe_rx[8];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart2)
	{
		if (buffe_rx[0] == MODBUS_ADDERSS_SLAVE)
		{
			switch (buffe_rx[1])
			{
			case MODBUS_FCN_01:
				MODBUS_fcn_01(&buffe_rx[2], (sizeof(buffe_rx) - 2));
				break;
				
			case MODBUS_FCN_03:
				MODBUS_fcn_03(&buffe_rx[2], (sizeof(buffe_rx) - 2));
				break;
				
			case MODBUS_FCN_05:
				MODBUS_fcn_05(&buffe_rx[2], (sizeof(buffe_rx) - 2));
				break;
				
			case MODBUS_FCN_06:
				MODBUS_fcn_06(&buffe_rx[2], (sizeof(buffe_rx) - 2));
				break;
			
			default:
				break;
			}
		}
	}
}

void UART2_Transmit(const uint8_t *const buffer, const uint16_t buffer_size, const uint32_t timeout)
{
	HAL_UART_Transmit(&huart2, buffer, (buffer_size + 2), 100);
}
