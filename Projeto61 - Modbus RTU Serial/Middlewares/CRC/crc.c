#include "crc.h"

uint16_t calc_crc(uint8_t *buf, uint16_t buf_size)
{
	uint16_t crc = 0xFFFF;

	for (uint16_t i = 0; i < buf_size; i++)
	{
		crc ^= buf[i];

		for (uint8_t j = 8; j != 0; j--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}

	return (((crc & 0xFF00) >> 8) | ((crc & 0x00FF) << 8));
}
