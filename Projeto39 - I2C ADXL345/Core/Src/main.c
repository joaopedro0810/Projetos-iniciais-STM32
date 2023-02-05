/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define ADXL345_ADDRESS_WRITE		 0xA6
#define ADXL345_ADDRESS_READ		 0xA7

#define DATA_DEVID_REGISTER			0x00
#define DATA_POWER_CTRL_REGISTER	0x2D
#define DATA_FORMAT_REGISTER		0x31
#define DATA_X0_REGISTER			0x32
#define DATA_X1_REGISTER			0x33
#define DATA_Y0_REGISTER			0x34
#define DATA_Y1_REGISTER			0x35
#define DATA_Z0_REGISTER			0x36
#define DATA_Z1_REGISTER			0x37


#define ADXL_CONST_CONVERT								0.0078
#define ADXL_CONVERT_REGISTER_DATA(data_to_convert)		(data_to_convert * ADXL_CONST_CONVERT)


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

uint32_t loopCont = 0;

static struct
{
	uint8_t device_id;
	uint8_t data_register[6];

	struct
	{
		int16_t x;
		int16_t y;
		int16_t z;

	} data_readed_register;

	struct
	{
		float x;
		float y;
		float z;

	} data_converted_g;

} adxl_data = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

void adxl_init(void);
void adxl_write_byte(uint8_t reg, uint8_t data);
void adxl_read_byte(uint8_t reg, uint8_t *data_readed);
void adxl_read_values (uint8_t reg, uint8_t *data_rec);
uint16_t little_endian_2_bytes(uint8_t *data);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  adxl_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  adxl_read_values(DATA_X0_REGISTER, adxl_data.data_register);

	  adxl_data.data_readed_register.x = little_endian_2_bytes(&adxl_data.data_register[0]);
	  adxl_data.data_readed_register.y = little_endian_2_bytes(&adxl_data.data_register[2]);
	  adxl_data.data_readed_register.z = little_endian_2_bytes(&adxl_data.data_register[4]);

	  adxl_data.data_converted_g.x = ADXL_CONVERT_REGISTER_DATA(adxl_data.data_readed_register.x);
	  adxl_data.data_converted_g.y = ADXL_CONVERT_REGISTER_DATA(adxl_data.data_readed_register.y);
	  adxl_data.data_converted_g.z = ADXL_CONVERT_REGISTER_DATA(adxl_data.data_readed_register.z);

	  loopCont++;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
 * Inicializa o ADXL345
 */
void adxl_init(void)
{
	if (HAL_I2C_IsDeviceReady(&hi2c1, ADXL345_ADDRESS_WRITE, 2, 10) != HAL_OK)
		Error_Handler();

	adxl_write_byte(DATA_POWER_CTRL_REGISTER, 0x00);
	adxl_write_byte(DATA_POWER_CTRL_REGISTER, 0x08);
	adxl_write_byte(DATA_FORMAT_REGISTER, 0x01);

	adxl_read_byte(DATA_DEVID_REGISTER, &adxl_data.device_id);
}

/**
 * Escreve um byte no ADXL345 dado o registrador
 */
void adxl_write_byte(uint8_t reg, uint8_t data)
{
	uint8_t data_write[2] = {reg, data};

	if (HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDRESS_WRITE, data_write, sizeof(data_write), 100) != HAL_OK)
		Error_Handler();
}

/**
 * Lê um byte no ADXL345 dado o registrador
 */
void adxl_read_byte(uint8_t reg, uint8_t *data_readed)
{
	if (HAL_I2C_Master_Transmit(&hi2c1, ADXL345_ADDRESS_WRITE, &reg, sizeof(reg), 100) != HAL_OK)
		Error_Handler();

	if(HAL_I2C_Master_Receive(&hi2c1, ADXL345_ADDRESS_READ, data_readed, sizeof(data_readed), 100) != HAL_OK)
		Error_Handler();
}

/**
 * Lê os valores dos eixos no ADXL345 dado o registrador inicial
 */
void adxl_read_values (uint8_t reg, uint8_t *data_rec)
{
	if (HAL_I2C_Mem_Read(&hi2c1, ADXL345_ADDRESS_WRITE, reg, 1, data_rec, 6, 100) != HAL_OK)
		Error_Handler();
}

/**
 * Converte um dado little endian de 2 bytes
 */
uint16_t little_endian_2_bytes(uint8_t *data)
{
	return ((data[1]<<8) | data[0]);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
