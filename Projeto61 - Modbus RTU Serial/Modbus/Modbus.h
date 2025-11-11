#include "stdint.h"
#include "string.h"

#define MODBUS_ADDERSS_SLAVE		0x01

#define MODBUS_FCN_01		0x01
#define MODBUS_FCN_03		0x03
#define MODBUS_FCN_05		0x05
#define MODBUS_FCN_06		0x06

void MODBUS_fcn_01(const uint8_t *const buf, const uint16_t buf_size);
void MODBUS_fcn_03(const uint8_t *const buf, const uint16_t buf_size);
void MODBUS_fcn_05(const uint8_t *const buf, const uint16_t buf_size);
void MODBUS_fcn_06(const uint8_t *const buf, const uint16_t buf_size);