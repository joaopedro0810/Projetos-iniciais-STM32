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

#define DATA_DEVID_REGISTER			0x00
#define DATA_POWER_CTRL_REGISTER	0x2D
#define DATA_FORMAT_REGISTER		0x31
#define DATA_X0_REGISTER			0x32
#define DATA_X1_REGISTER			0x33
#define DATA_Y0_REGISTER			0x34
#define DATA_Y1_REGISTER			0x35
#define DATA_Z0_REGISTER			0x36
#define DATA_Z1_REGISTER			0x37

#define ADXL345_MULTIBYTE			0x40
#define ADXL345_READ_OPERATION		0x80

#define ADXL_CONST_CONVERT								0.0078
#define ADXL_CONVERT_REGISTER_DATA(data_to_convert)		(data_to_convert * ADXL_CONST_CONVERT)

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 SPI_HandleTypeDef hspi1;

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
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

void adxl_init(void);
void adxl_write(uint8_t reg, uint8_t data);
void adxl_read(uint8_t reg, uint8_t *data_readed, const size_t data_size);
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  adxl_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  adxl_read(DATA_X0_REGISTER, adxl_data.data_register, sizeof(adxl_data.data_register));

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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : CS_ADXL_Pin */
  GPIO_InitStruct.Pin = CS_ADXL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_ADXL_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
 * Inicializa o ADXL345
 */
void adxl_init(void)
{
	adxl_write (DATA_FORMAT_REGISTER, 0x01);
	adxl_write (DATA_POWER_CTRL_REGISTER, 0x00);
	adxl_write (DATA_POWER_CTRL_REGISTER, 0x08);

	adxl_read(DATA_DEVID_REGISTER, &adxl_data.device_id, sizeof(adxl_data.device_id));
}

/**
 * Escreve dados no ADXL345 dado o registrador
 */
void adxl_write(uint8_t reg, uint8_t data)
{
	uint8_t data_write[2] = {(reg | ADXL345_MULTIBYTE), data};

	HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(&hspi1, data_write, sizeof(data_write), 100) != HAL_OK)
		Error_Handler();

	HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_SET);
}

/**
 * Lê dados no ADXL345 dado o registrador
 */
void adxl_read(uint8_t reg, uint8_t *data_readed, const size_t data_size)
{
	reg = ((reg | ADXL345_READ_OPERATION) | ADXL345_MULTIBYTE);

	HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_RESET);

	if (HAL_SPI_Transmit(&hspi1, &reg, sizeof(reg), 100) != HAL_OK)
			Error_Handler();

	if (HAL_SPI_Receive(&hspi1, data_readed, data_size, 100) != HAL_OK)
			Error_Handler();

	HAL_GPIO_WritePin(CS_ADXL_GPIO_Port, CS_ADXL_Pin, GPIO_PIN_SET);
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
