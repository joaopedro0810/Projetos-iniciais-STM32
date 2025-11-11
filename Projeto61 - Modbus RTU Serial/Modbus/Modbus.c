#include "crc.h"
#include "gpio.h"
#include "dac.h"
#include "uart.h"
#include "Modbus.h"

void MODBUS_fcn_01(const uint8_t *const buf, const uint16_t buf_size)
{
	uint8_t out_buf[16] = {MODBUS_ADDERSS_SLAVE, MODBUS_FCN_01, 0};

	if (	((buf[0] == 0x00) && ((buf[1] == 0x04) || (buf[1] == 0x05)))
		&& 	((buf[2] == 0x00) && ((buf[3] == 0x02) || (buf[3] == 0x01))))
	{
		out_buf[2] = 0x01;
		out_buf[3] = 0;		//@ToDo - Implementar leitura dos leds

		uint16_t crc_calc = calc_crc(out_buf, 4);
		memcpy(&out_buf[4], &crc_calc, sizeof(crc_calc));
		UART2_Transmit(out_buf, (buf_size + 2), 100);
	}
}

void MODBUS_fcn_03(const uint8_t *const buf, const uint16_t buf_size)
{
	uint8_t out_buf[16] = {MODBUS_ADDERSS_SLAVE, MODBUS_FCN_03, 0};

	if (	(buf[0] == 0x00)
		&& 	(buf[1] == 0x07)
		&& 	(buf[2] == 0x00)
		&& 	(buf[3]	== 0x02))
	{
		out_buf[2] = 0x04;		//@ToDo - Implementar qtd bytes enviados

		out_buf[3] = 0x00; //@ToDo - Implementar valores ADC Byte Maior;
		out_buf[4] = 0x00; //@ToDo - Implementar valores ADC Byte Menor;
		out_buf[5] = 0x00; //@ToDo - Implementar valores DAC Byte Maior;
		out_buf[6] = 0x00; //@ToDo - Implementar valores DAC Byte Maior;

		uint16_t crc_calc = calc_crc(out_buf, 7);
		memcpy(&out_buf[7], &crc_calc, sizeof(crc_calc));

		UART2_Transmit(out_buf, (buf_size + 2), 100);
	}
}

void MODBUS_fcn_05(const uint8_t *const buf, const uint16_t buf_size)
{
	uint8_t out_buf[16] = {MODBUS_ADDERSS_SLAVE, MODBUS_FCN_05, 0};

	if ((buf[0] == 0x00) &&	(buf[1] == 0x04))
	{
		GPIO_write_led_2(buf[2]);
		memcpy(&out_buf[2], buf, buf_size);
		UART2_Transmit(out_buf, (buf_size + 2), 100);
	}
	else if ((buf[0] == 0x00) &&	(buf[1] == 0x05))
	{
		GPIO_write_led_3(buf[2]);
		memcpy(&out_buf[2], buf, buf_size);
		UART2_Transmit(out_buf, (buf_size + 2), 100);
	}
}

void MODBUS_fcn_06(const uint8_t *const buf, const uint16_t buf_size)
{
	uint8_t out_buf[16] = {MODBUS_ADDERSS_SLAVE, MODBUS_FCN_06, 0};

	if ((buf[0] == 0x00) &&	(buf[1] == 0x08))
	{
		uint16_t dac_dados = (buf[2] << 8) + buf[3];

		if (dac_dados > 4095)
		{
			dac_dados = 4095;
		}
		else if (dac_dados < 0)
		{
			dac_dados = 0;
		}

		DAC_write_channel_1(dac_dados);
		memcpy(&out_buf[2], buf, buf_size);
		UART2_Transmit(out_buf, (buf_size + 2), 100);
	}
}
