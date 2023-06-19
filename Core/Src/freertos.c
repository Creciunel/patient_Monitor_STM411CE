/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* MLX90614 I2C address */
#define MLX90614_ADDRESS 0x5A

/* MLX90614 memory map */
#define MLX90614_TEMP_AMBIENT 0x06
#define MLX90614_TEMP_OBJECT1 0x07
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern I2C_HandleTypeDef hi2c1;
extern sht3x_handle_t handle;
extern ADC_HandleTypeDef hadc1;
ADC_ChannelConfTypeDef sConfig;

struct data {
	uint8_t encoder, lastEncoder;
	uint16_t temperature, humidity, lastTemperature, lastHumidity;
	uint16_t MQ3, MQ4, MQ7, MQ135, lastMQ3, lastMQ4, lastMQ7, lastMQ135; // Alcohol CH4 CO Air
	uint32_t ECG, startTime;
	float handTemp, lastHandTemp;
} Data = { .encoder = 0 };

struct flag {
	uint8_t temp, humy, gas, hand, ecg, firstECG;
} Flag;
//= { .temp = 0, .humy = 0, .gas = 0, .hand = 0 };

struct send {
	uint16_t temp, humy, MQ3, MQ4, MQ7, MQ135, handTemp;
} sendProtocol;

/* USER CODE END Variables */
/* Definitions for ReadEncoderTask */
osThreadId_t ReadEncoderTaskHandle;
const osThreadAttr_t ReadEncoderTask_attributes = { .name = "ReadEncoderTask",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for ReadADCTask */
osThreadId_t ReadADCTaskHandle;
const osThreadAttr_t ReadADCTask_attributes = { .name = "ReadADCTask",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = { .name = "DisplayTask",
		.stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for MLX90614Task */
osThreadId_t MLX90614TaskHandle;
const osThreadAttr_t MLX90614Task_attributes = { .name = "MLX90614Task",
		.stack_size = 128 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for ECGTask */
osThreadId_t ECGTaskHandle;
const osThreadAttr_t ECGTask_attributes = { .name = "ECGTask", .stack_size = 128
		* 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for QueueEN */
osMessageQueueId_t QueueENHandle;
const osMessageQueueAttr_t QueueEN_attributes = { .name = "QueueEN" };
/* Definitions for QueueADC */
osMessageQueueId_t QueueADCHandle;
const osMessageQueueAttr_t QueueADC_attributes = { .name = "QueueADC" };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartReadEncoderTask(void *argument);
void StartReadADCTask(void *argument);
void StartDisplayTask(void *argument);
void StartMLX90614Task(void *argument);
void StartECGTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the queue(s) */
	/* creation of QueueEN */
	QueueENHandle = osMessageQueueNew(8, sizeof(uint8_t), &QueueEN_attributes);

	/* creation of QueueADC */
	QueueADCHandle = osMessageQueueNew(20, sizeof(uint16_t),
			&QueueADC_attributes);

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of ReadEncoderTask */
	ReadEncoderTaskHandle = osThreadNew(StartReadEncoderTask, NULL,
			&ReadEncoderTask_attributes);

	/* creation of ReadADCTask */
	ReadADCTaskHandle = osThreadNew(StartReadADCTask, NULL,
			&ReadADCTask_attributes);

	/* creation of DisplayTask */
	DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL,
			&DisplayTask_attributes);

	/* creation of MLX90614Task */
	MLX90614TaskHandle = osThreadNew(StartMLX90614Task, NULL,
			&MLX90614Task_attributes);

	/* creation of ECGTask */
	ECGTaskHandle = osThreadNew(StartECGTask, NULL, &ECGTask_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartReadEncoderTask */
/**
 * @brief  Function implementing the ReadEncoderTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadEncoderTask */
void StartReadEncoderTask(void *argument) {
	/* USER CODE BEGIN StartReadEncoderTask */
	float temperature, humidity;
	/* Infinite loop */
	for (;;) {
		Data.encoder = TIM2->CNT;

		sht3x_read_temperature_and_humidity(&handle, &temperature, &humidity);
		Data.temperature = (uint16_t) temperature;
		Data.humidity = (uint16_t) humidity;

		if (HAL_GPIO_ReadPin(Enc_Btn_GPIO_Port, Enc_Btn_Pin) != GPIO_PIN_SET) {
			if (Flag.temp == 0) {
				checkBTN(0, Data.encoder);
				sendProtocol.temp = Data.temperature;
				Flag.temp = 1;
			} else if (Flag.humy == 0) {
				checkBTN(1, Data.encoder);
				sendProtocol.humy = Data.humidity;
				Flag.humy = 1;
			} else if (Flag.gas == 0) {
				checkBTN(2, Data.encoder);
				sendProtocol.MQ3 = Data.MQ3;
				sendProtocol.MQ4 = Data.MQ4;
				sendProtocol.MQ7 = Data.MQ7;
				sendProtocol.MQ135 = Data.MQ135;
				Flag.gas = 1;
			} else if (Flag.hand == 0) {
				checkBTN(3, Data.encoder);
				sendProtocol.handTemp = Data.handTemp;
				Flag.hand = 1;
			} else if (Flag.ecg == 0) {
				Flag.ecg = 1;
				Data.startTime = HAL_GetTick();

				checkBTN(4, Data.encoder);
			} else if (Flag.ecg == 2) {
				checkBTN(5, Data.encoder);

				Flag.temp = 0;
				Flag.humy = 0;
				Flag.gas = 0;
				Flag.hand = 0;
				Flag.ecg = 0;
			}
		}
		osDelay(150);
	}
	/* USER CODE END StartReadEncoderTask */
}

/* USER CODE BEGIN Header_StartReadADCTask */
/**
 * @brief Function implementing the ReadADCTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadADCTask */
void StartReadADCTask(void *argument) {
	/* USER CODE BEGIN StartReadADCTask */
	/* Infinite loop */
	for (;;) {
		HAL_ADCEx_InjectedStart(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 100);

		Data.MQ3 = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
		Data.MQ4 = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
		Data.MQ7 = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
		Data.MQ135 = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_4);

		HAL_ADCEx_InjectedStop(&hadc1);
		osDelay(1000);
	}
	/* USER CODE END StartReadADCTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
 * @brief Function implementing the DisplayTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument) {
	/* USER CODE BEGIN StartDisplayTask */
	/* Infinite loop */
	for (;;) {

		switch (Data.encoder) {
		case 0:
			if (Data.lastEncoder != Data.encoder) {
				Data.lastTemperature = Data.temperature;
				Data.lastHumidity = Data.humidity;

				Display1SHT3X_Init(Data.temperature, Data.humidity);

			} else if (Data.lastTemperature != Data.temperature
					|| Data.lastHumidity != Data.humidity) {
				Data.lastTemperature = Data.temperature;
				Data.lastHumidity = Data.humidity;

				Display1SHT3X(Data.temperature, Data.humidity);
			}
			Data.lastEncoder = 0;
			break;
		case 1:
			if (Data.lastEncoder != Data.encoder) {
				Data.lastMQ3 = Data.MQ3;
				Data.lastMQ4 = Data.MQ4;
				Data.lastMQ7 = Data.MQ7;
				Data.lastMQ135 = Data.MQ135;

				Display2Gas_Init(Data.MQ3, Data.MQ3, Data.MQ3, Data.MQ3);
			} else if (Data.MQ3 != Data.lastMQ3 || Data.lastMQ4 != Data.MQ4
					|| Data.lastMQ7 != Data.MQ7
					|| Data.lastMQ135 != Data.MQ135) {
				Data.lastMQ3 = Data.MQ3;
				Data.lastMQ4 = Data.MQ4;

				Display2Gas(Data.MQ3, Data.MQ4, Data.MQ7, Data.MQ135);
			}
			Data.lastEncoder = 1;
			break;
		case 2:
			if (Data.lastEncoder != Data.encoder) {
				Data.lastHandTemp = Data.handTemp;

				Display3MLX_Init(Data.handTemp);
			} else if (Data.lastHandTemp != Data.handTemp) {
				Display3MLX(Data.handTemp);
			}
			Data.lastEncoder = 2;
			break;
		case 3:
			if (Data.lastEncoder != Data.encoder) {
				Data.lastHandTemp = Data.handTemp;

				Display3MLX_Init(Data.handTemp);
			} else if (Data.lastHandTemp != Data.handTemp) {
				Display3MLX(Data.handTemp);
			}
			Data.lastEncoder = 2;
			break;
		}
		osDelay(100);
	}
	/* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartMLX90614Task */
/**
 * @brief Function implementing the MLX90614Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMLX90614Task */
void StartMLX90614Task(void *argument) {
	/* USER CODE BEGIN StartMLX90614Task */
	uint8_t status;
	float handTemTemperature;
	/* Infinite loop */
	for (;;) {
		status = MLX90614_ReadTemp(MLX90614_TOBJ1, &handTemTemperature);
		if (status == 1)
			Data.handTemp = handTemTemperature;
		osDelay(1000);
	}
	/* USER CODE END StartMLX90614Task */
}

/* USER CODE BEGIN Header_StartECGTask */
/**
 * @brief Function implementing the ECGTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartECGTask */
void StartECGTask(void *argument) {
	/* USER CODE BEGIN StartECGTask */

	/* Infinite loop */
	for (;;) {
		if (Flag.hand == 1) {
			printf(
					"{\"TempOut\":%i,\"HumyOut\":%i,\"MQ3\":%i,\"MQ4\":%i,\"MQ7\":%i,\"MQ135\":%i,\"HandTemp\":%i,\"ecg\":\"",
					sendProtocol.temp, sendProtocol.humy, sendProtocol.MQ3,
					sendProtocol.MQ4, sendProtocol.MQ7, sendProtocol.MQ135,
					sendProtocol.handTemp);
			Flag.hand = 2;
		}
		if (Flag.ecg == 1 && Flag.hand == 2) {
			sConfig.Channel = ADC_CHANNEL_6;
			HAL_ADC_ConfigChannel(&hadc1, &sConfig);

			HAL_ADC_Start(&hadc1);

			if (HAL_GPIO_ReadPin(LOPLUS_GPIO_Port, LOPLUS_Pin) == GPIO_PIN_RESET
					&& HAL_GPIO_ReadPin(LOMINUS_GPIO_Port,
					LOMINUS_Pin) == GPIO_PIN_RESET) {
				if (HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK) {

					Data.ECG = HAL_ADC_GetValue(&hadc1);
					if (Flag.firstECG == 0) {
						printf("%li", Data.ECG);
						Flag.firstECG = 1;
					}
					printf(",%li", Data.ECG);
					if (HAL_GetTick() - Data.startTime > 5000) {
						printf("\"}\n");
						Flag.firstECG = 0;
						Flag.ecg = 2;
					}
				}
			}
		}

		osDelay(2);
	}
	/* USER CODE END StartECGTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

