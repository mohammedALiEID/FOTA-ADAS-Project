/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BLU.h"
#include "DC_Motor.h"
#include "UltraSonic_interface_.h"
#include "BUZZER_interface.h"
#include "LM35.h"
#include "LDR.h"
#include "LED_interface.h"
#include "Line_Detector.h"
#include "mpu6050.h"
#include "LCD_interface.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MOTOR_MAX_SPEED 100
#define MOTOR_MIN_SPEED 0
#define ENGINE_ON       1
#define ENGINE_OFF      0
#define MOTOR_FWD		0
#define MOTOR_BACK		1
#define MANUAL_CONTROL	0
#define AUTO_CONTROL	1

typedef struct
{
	float Distance_left_front;
	float Distance_front;
	float Distance_right_front;
	float Distance_left_back;
	float Distance_right_back;
	float Distance_back;
	float temprature;
	uint16_t LDR_Read;
	uint8_t IR_Left;
	uint8_t IR_Right;
	MPU6050_t Mpu6050;
}SensorsData_t;

SensorsData_t DataAcquistion;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void BLE_Read_Task    			(void  * argument);
void Data_Acq_Task				(void  * argument);
void Action_Task      			(void  * argument);
void Display_Task     			(void  * argument);
//void Emergency_Task				(void  * argument);
void Autonomous_Task			(void  * argument);
void Auto_Motion_Task			(void  * argument);

EventGroupHandle_t EG_Action = NULL;
EventBits_t        EG_Value  = 0;

EventGroupHandle_t EG_Autonomous = NULL;
EventBits_t        EG_State  = 0;

#define EG_ENGINE_START    	(1<<0)
#define EG_ENGINE_STOP     	(1<<1)
#define EG_SPEED_UP        	(1<<2)
#define EG_SPEED_DOWN      	(1<<3)
#define EG_CAR_FWD			(1<<4)
#define EG_CAR_BACK			(1<<5)
#define EG_CAR_RIGHT		(1<<6)
#define EG_CAR_LEFT			(1<<7)

#define EG_STATE_Manual		(1<<0)
#define EG_STATE_ACC		(1<<1)
#define EG_STATE_AP			(1<<2)
#define EG_STATE_OBD		(1<<3)
#define EG_STATE_LKA		(1<<4)

volatile uint8_t Duty = 0;
volatile uint8_t ACC_State = 0;
volatile uint8_t ACC_Enabled = 0;
volatile uint8_t User_Control = 0;
volatile uint8_t Engine_State = 0;
volatile uint8_t Car_State = 0;
volatile uint8_t Car_Direction = 0;
uint8_t Counter = 0;
volatile uint8_t u8DutyFlag = 0;
volatile uint8_t u8LCDFlagInit = 0;
uint8_t u8CLRState = 0;
volatile uint8_t AP_State = 0;
volatile uint8_t OBD_State = 0;
volatile uint8_t LKA_State = 0;
volatile uint8_t Autoparking_VehicleSpeed = 20;
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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  EG_Action = xEventGroupCreate();
  EG_Autonomous = xEventGroupCreate();

  xTaskCreate(BLE_Read_Task, NULL, 100 , NULL , 1 , NULL);
  xTaskCreate(Autonomous_Task, NULL, 50 , NULL , 4 , NULL);
  xTaskCreate(Auto_Motion_Task, NULL, 50, NULL, 5, NULL);
  xTaskCreate(Action_Task, NULL, 100 , NULL , 3 , NULL);
  xTaskCreate(Display_Task, NULL, 50 , NULL , 2 , NULL);
  xTaskCreate(Data_Acq_Task, NULL, 100 , NULL , 6 , NULL);
  //xTaskCreate(Emergency_Task, NULL, 100 , NULL , 6 , NULL);

  while (MPU6050_Init(&hi2c1) == 1);
  DC_voidInit();
  UltraSonic_INIT_TIMER1_ALL_CHANNEL();
  UltraSonic_INIT_TIMER3_Ch1_ch2();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Data_Acq_Task		(void  * argument)
{
	while(1)
	{
		// Read temperature from LM35 sensor
		DataAcquistion.temprature = LM35_fReadTemperature();

		// Read light intensity from LDR sensor
		DataAcquistion.LDR_Read = HAL_u8LDRRead();

		// Read IR sensor values for left and right directions
		LineDetector_voidRead(&(DataAcquistion.IR_Left), &(DataAcquistion.IR_Right));

		// Read distances from ultrasonic sensors
		UltraSonic_Get_Distance_TIMER1_ALL_CHANNEL(&(DataAcquistion.Distance_left_back), &(DataAcquistion.Distance_right_back), &(DataAcquistion.Distance_left_front), &(DataAcquistion.Distance_right_front));
		vTaskDelay(200);
		UltraSonic_Get_Distance_TIMER3_Ch1_ch2(&(DataAcquistion.Distance_back), &(DataAcquistion.Distance_front));

		MPU6050_Read_All(&hi2c1, &(DataAcquistion.Mpu6050));


		vTaskDelay(200);
	}
}

void Auto_Motion_Task			(void  * argument)
{
	while(1)
	{
		// blind spot
		if(DataAcquistion.Distance_left_back <= 5 || DataAcquistion.Distance_right_back <= 5)
		{
			LED_voidTurnOn(LED_Pin);
			BUZZER_voidTurnOn();
		}
		else
		{
			LED_voidTurnOff(LED_Pin);
			BUZZER_voidTurnOff();
		}

		//ACC
		if (ACC_State)
		{
			if(DataAcquistion.Distance_front <= 15.0 && DataAcquistion.Distance_front > 8.0)
			{
				Duty = 50;
				LED_voidToggle(LED_Pin);
				BUZZER_voidToggle();
				//DC_voidMoveForward(Duty);
			}
			else if(DataAcquistion.Distance_front <= 8.0)
			{
				Duty = MOTOR_MIN_SPEED;
				DC_voidStop();
				LED_voidToggle(LED_Pin);
				BUZZER_voidToggle();
				//DC_voidMoveForward(Duty);
			}
			else
			{
				Duty=65;
				if (Duty > MOTOR_MAX_SPEED)
				{
					Duty = MOTOR_MAX_SPEED;
				}
			}
			DC_voidMoveForward(Duty);
		}

		//Obstacle Avoiding
		if(OBD_State)
		{
			if (DataAcquistion.Distance_front <= 15)
			{
				if (DataAcquistion.Distance_right_front > DataAcquistion.Distance_left_front && DataAcquistion.Distance_right_front > 30)
				{
					DC_voidMoveForwardRight (Duty);
					Car_Direction = 1;
				}
				else if (DataAcquistion.Distance_right_front < DataAcquistion.Distance_left_front && DataAcquistion.Distance_left_front > 30)
				{
					DC_voidMoveForwardLeft (Duty);
					Car_Direction = 2;
				}
				else
				{
					DC_voidMoveBackward(Duty);
					Car_State = MOTOR_BACK;
				}
			}
		}

		if(LKA_State)
		{
			if(!DataAcquistion.IR_Left)
			{
				DC_voidMoveForwardRight(Duty);
				Car_Direction = 1;
			}
			if(!DataAcquistion.IR_Right)
			{
				DC_voidMoveForwardLeft(Duty);
				Car_Direction = 2;
			}
			DC_voidMoveForward(Duty);
		}

		/*if(AP_State)
		{
			const float MIN_PARKING_DISTANCE = 10.0f; // Minimum distance required for parking in cm
			const float MIN_DISTANCE = 5.0f; // Minimum distance for stopping in cm

			// Check if parking is possible on the left side based on the distance from the left-front sensor
			if (DataAcquistion.Distance_left_front < MIN_PARKING_DISTANCE)
			{
				// If parking is possible on the left side, execute parking maneuvers
				// The parking maneuver involves moving the vehicle forward, backward, and turning as needed
				while (DataAcquistion.Distance_left_front < 15 || DataAcquistion.Distance_left_back < 15)
				{
					DC_voidMoveForward (Autoparking_VehicleSpeed);
				}

				DC_voidStop();
				// Move forward until reaching a suitable parking spot
				while (DataAcquistion.Distance_left_front > 15 || DataAcquistion.Distance_left_back > 15)
				{
					DC_voidMoveForward (Autoparking_VehicleSpeed);
				}
				DC_voidStop();
				// Perform backward parking maneuver until the vehicle is in position
				while (DataAcquistion.Distance_left_back >MIN_DISTANCE || DataAcquistion.Distance_back > MIN_DISTANCE)
				{
					DC_voidMoveBackwardLeft(Autoparking_VehicleSpeed);
				}
				DC_voidStop();
				// Adjust the vehicle's position by moving forward and backward as needed
				while (DataAcquistion.Distance_left_front > MIN_DISTANCE|| DataAcquistion.Distance_front > MIN_DISTANCE)
				{
					DC_voidMoveForwardRight(Autoparking_VehicleSpeed);
				}
				DC_voidStop();
				while (DataAcquistion.Distance_left_back > MIN_DISTANCE || DataAcquistion.Distance_back >MIN_DISTANCE)
				{
					DC_voidMoveBackwardRight(Autoparking_VehicleSpeed);
				}
				DC_voidStop();
				// Move forward to finalize parking
				while (DataAcquistion.Distance_front > MIN_DISTANCE)
				{
					DC_voidMoveForward (Autoparking_VehicleSpeed);
				}
				DC_voidStop();
			}
			// Check if parking is possible on the right side based on the distance from the right-front sensor
			else if (DataAcquistion.Distance_right_front < MIN_PARKING_DISTANCE)
			{
				// If parking is possible on the right side, execute parking maneuvers
				// The parking maneuver involves moving the vehicle forward, backward, and turning as needed
				// (Similar to the left-side parking maneuver)
				//Continuously check sensor readings while attempting to find a parking spot
				while(DataAcquistion.Distance_right_front<15 && DataAcquistion.Distance_right_back<15)
				{
					DC_voidMoveForward (Autoparking_VehicleSpeed);
				}
				// Move forward until reaching a suitable parking spot
				DC_voidStop();
				while(DataAcquistion.Distance_right_front>15 || DataAcquistion.Distance_right_back>15)
				{
					DC_voidMoveForward (Autoparking_VehicleSpeed);
				}
				DC_voidStop();

				// Perform backward parking maneuver until the vehicle is in position
				while(DataAcquistion.Distance_right_back > MIN_DISTANCE || DataAcquistion.Distance_back > MIN_DISTANCE)
				{
					DC_voidMoveBackwardRight(Autoparking_VehicleSpeed);
				}
				DC_voidStop();

				// Adjust the vehicle's position by moving forward and backward as needed
				while(DataAcquistion.Distance_right_front > MIN_DISTANCE || DataAcquistion.Distance_front > MIN_DISTANCE)
				{
					DC_voidMoveForwardRight(Autoparking_VehicleSpeed);
				}
				DC_voidStop();

				while(DataAcquistion.Distance_right_back > MIN_DISTANCE || DataAcquistion.Distance_back > MIN_DISTANCE  )
				{
					DC_voidMoveBackwardLeft(Autoparking_VehicleSpeed);
				}
				DC_voidStop();
				// Move forward to finalize parking
				while(DataAcquistion.Distance_right_front > MIN_DISTANCE|| DataAcquistion.Distance_front > MIN_DISTANCE)
				{
					DC_voidMoveForward (Autoparking_VehicleSpeed);
				}
				DC_voidStop();
			}
			else
			{
				// If no parking spots are available on either side, handle error or take appropriate action
			}
		}*/
		vTaskDelay(200);
	}
}

void Display_Task     (void  * argument)
{
	while(1)
	{
		if(Engine_State == 1)
		{
			if(u8LCDFlagInit == 0)
			{
				LCD_voidInit();
				u8LCDFlagInit = 1;
			}
			// Display ( Duty & ACC & AutoParkingState On LCD)
			LCD_voidSetCursor(0, 0);
			LCD_voidWriteString((uint8_t*)"Speed:");
			if(Duty == 0)
			{
				if(u8DutyFlag == 1)
				{
					LCD_voidSetCursor(0, 8);
					LCD_voidWriteString((uint8_t*)"  ");
				}
				LCD_voidSetCursor(0, 6);
				LCD_voidWriteNum(Duty);
				LCD_voidWriteString((uint8_t*)"%");
				u8DutyFlag = 0;
			}
			else if(Duty < 100 && Duty > 0)
			{
				if(u8DutyFlag == 2)
				{
					LCD_voidSetCursor(0, 9);
					LCD_voidWriteString((uint8_t*)" ");
				}
				LCD_voidSetCursor(0, 6);
				LCD_voidWriteNum(Duty);
				LCD_voidWriteString((uint8_t*)"%");
				u8DutyFlag = 1;
			}
			else
			{
				LCD_voidWriteNum(Duty);
				LCD_voidWriteString((uint8_t*)"%");
				u8DutyFlag = 2;
			}
			LCD_voidSetCursor(1, 9);
			LCD_voidWriteString((uint8_t*)"ACC:");
			if(ACC_State == 1)
			{
				LCD_voidWriteString((uint8_t*)" ON");
			}
			else
			{
				LCD_voidWriteString((uint8_t*)"OFF");
			}
			LCD_voidSetCursor(0, 10);
			LCD_voidWriteString((uint8_t*)"AP:");
			if(AP_State == 1)
			{
				LCD_voidWriteString((uint8_t*)" ON");
			}
			else
			{
				LCD_voidWriteString((uint8_t*)"OFF");
			}
			LCD_voidSetCursor(1, 0);
			LCD_voidWriteString((uint8_t*)"OBD:");
			if(OBD_State == 1)
			{
				LCD_voidWriteString((uint8_t*)" ON");
			}
			else
			{
				LCD_voidWriteString((uint8_t*)"OFF");
			}

		}
		vTaskDelay(200);
	}
}

void BLE_Read_Task    (void  * argument)
{
	HAL_StatusTypeDef RetValue = HAL_ERROR;
	uint8_t BLE_Rer=0;
	while(1)
	{
		RetValue =  BLU_voidReceivePolling  (&BLE_Rer, sizeof(BLE_Rer), 100);
		if(RetValue == HAL_OK)
		{
			switch(BLE_Rer)
			{
			case 'U' :
				xEventGroupSetBits(EG_Action,EG_SPEED_UP);
				break;
			case 'D' :
				xEventGroupSetBits(EG_Action,EG_SPEED_DOWN);
				break;
			case 'S' :
				xEventGroupSetBits(EG_Action,EG_ENGINE_START);
				break;
			case 'E' :
				xEventGroupSetBits(EG_Action,EG_ENGINE_STOP);
				break;
			case 'F' :
				xEventGroupSetBits(EG_Action, EG_CAR_FWD);
				break;
			case 'B' :
				xEventGroupSetBits(EG_Action,EG_CAR_BACK);
				break;
			case 'R' :
				xEventGroupSetBits(EG_Action,EG_CAR_RIGHT);
				break;
			case 'L' :
				xEventGroupSetBits(EG_Action,EG_CAR_LEFT);
				break;
			case 'M' :
				xEventGroupSetBits(EG_Autonomous,EG_STATE_Manual);
				break;
			case 'P' :
				//Auto Parking
				xEventGroupSetBits(EG_Autonomous,EG_STATE_AP);
				break;
			case 'A' :
				//ACC
				xEventGroupSetBits(EG_Autonomous,EG_STATE_ACC);
				//ACC_State = 1;
				break;
			case 'O' :
				//obstacle avoidance
				xEventGroupSetBits(EG_Autonomous,EG_STATE_OBD);
				break;
			case 'I' :
				//LKA
				xEventGroupSetBits(EG_Autonomous,EG_STATE_LKA);
			default:
				break;
			}
		}
		else
		{

		}
		vTaskDelay(200);
	}
}


void Action_Task      (void  * argument)
{
	while(1)
	{
		EG_Value = xEventGroupWaitBits(EG_Action,
				(EG_ENGINE_START | EG_ENGINE_STOP | EG_SPEED_UP | EG_SPEED_DOWN | EG_CAR_FWD | EG_CAR_BACK | EG_CAR_RIGHT | EG_CAR_LEFT) ,
				1, // Auto Clear
				0, // Or Between bits not wait for all of the events
				portMAX_DELAY);

		if(EG_Value & EG_ENGINE_START)
		{
			// Start Motor with Duty 20%
			Duty = 20;
			DC_voidInit();
			DC_voidMoveForward(Duty);
			Engine_State = ENGINE_ON;
		}
		if(EG_Value & EG_ENGINE_STOP)
		{
			// Stop Motor Duty 0%
			Duty =0;
			DC_voidStop();
			Engine_State = ENGINE_OFF;
		}
		if(EG_Value & EG_CAR_FWD)
		{
			DC_voidMoveForward(Duty);
			Car_State = MOTOR_FWD;
		}
		if(EG_Value & EG_CAR_BACK)
		{
			DC_voidMoveBackward(Duty);
			Car_State = MOTOR_BACK;
			DC_voidTurnLeft();
			//vTaskDelay(30);
		}
		if(EG_Value & EG_CAR_RIGHT)
		{
			DC_voidTurnRight();
			vTaskDelay(100);
			Car_Direction = 1;
		}
		if(EG_Value & EG_CAR_LEFT)
		{
			DC_voidTurnLeft();
			Car_Direction = 2;
		}
		if(EG_Value & EG_SPEED_UP)
		{
			if(ENGINE_ON == Engine_State  && Car_State == MOTOR_FWD)
			{
				Duty+=10;
				if(MOTOR_MAX_SPEED < Duty)
				{
					Duty=MOTOR_MAX_SPEED;
					DC_voidMoveForward(Duty);
				}
				else
				{
					DC_voidMoveForward(Duty);
				}
			}
			else if(ENGINE_ON == Engine_State && Car_State == MOTOR_BACK)
			{
				Duty+=10;
				if(MOTOR_MAX_SPEED < Duty)
				{
					Duty=MOTOR_MAX_SPEED;
					DC_voidMoveBackward(Duty);
				}
				else
				{
					DC_voidMoveBackward(Duty);
				}
			}
		}
		if(EG_Value & EG_SPEED_DOWN)
		{
			if(ENGINE_ON == Engine_State  && Car_State == MOTOR_FWD)
			{
				Duty-=10;
				if(MOTOR_MIN_SPEED > Duty)
				{
					Duty=MOTOR_MAX_SPEED;
					DC_voidMoveForward(Duty);
				}
				else
				{
					DC_voidMoveForward(Duty);
				}
			}
			else if(ENGINE_ON == Engine_State && Car_State == MOTOR_BACK)
			{
				Duty-=10;
				if(MOTOR_MIN_SPEED > Duty)
				{
					Duty=MOTOR_MAX_SPEED;
					DC_voidMoveBackward(Duty);
				}
				else
				{
					DC_voidMoveBackward(Duty);
				}
			}
		}

		vTaskDelay(200);
	}
}

void Autonomous_Task			(void  * argument)
{
	while(1)
	{
		EG_State = xEventGroupWaitBits(EG_Autonomous,
				(EG_STATE_Manual | EG_STATE_ACC | EG_STATE_AP | EG_STATE_OBD | EG_STATE_LKA) ,
				1, // Auto Clear
				0, // Or Between bits not wait for all of the events
				portMAX_DELAY);
		if(EG_State & EG_STATE_Manual)
		{
			User_Control = MANUAL_CONTROL;
			ACC_State = 0;
			AP_State = 0;
			OBD_State = 0;
			LKA_State = 0;
		}
		if(EG_State & EG_STATE_ACC)
		{
			User_Control = AUTO_CONTROL;
			ACC_State = 1;
			AP_State = 0;
			OBD_State = 0;
			LKA_State = 0;
			//ACC
		}
		if(EG_State & EG_STATE_AP)
		{
			User_Control = AUTO_CONTROL;
			AP_State = 1;
			ACC_State = 0;
			LKA_State = 0;
			OBD_State = 0;
			//Autoparking
		}
		if(EG_State & EG_STATE_OBD)
		{
			User_Control = AUTO_CONTROL;
			OBD_State = 1;
			ACC_State = 0;
			AP_State = 0 ;
			LKA_State = 0;
			//Obsticale avoidance

		}
		if(EG_State & EG_STATE_LKA)
		{
			User_Control = AUTO_CONTROL;
			LKA_State = 1;
			ACC_State = 0;
			AP_State = 0 ;
			OBD_State = 0;
			//Lane Keep Assistance
		}

		vTaskDelay(200);
	}

}

/*
void Emergency_Task				(void  * argument)
{
	float differencex = 0 ;
	float differencey = 0 ;
	float differencez = 0 ;
	float inputx[2] = {0};
	float inputy[2] = {0};
	float inputz[2] = {0};

	static uint8_t i = 0 ;
	double ax=0;
	double ay=0;
	double az=0;
	//double total_a=0,ay=0,az=0;
	float critical_value = 0;

	while(1)
	{

		ax = DataAcquistion.Mpu6050.Ax;
		ay = DataAcquistion.Mpu6050.Ay;
		az = DataAcquistion.Mpu6050.Az;
		//total_a = ax*ax + ay*ay + az*az;
		inputx[i] = ax;
		inputy[i] = ay;
		inputz[i] = az;

		if(i==1){
			differencex = inputx[1]-inputx[0];
			differencey = inputy[1]-inputy[0];
			differencez = inputz[1]-inputz[0];
		}

		while(differencex> critical_value || differencey> critical_value || differencez> critical_value){
			LED_voidToggle(LED_Pin);
			BUZZER_voidToggle();
			vTaskDelay(100);
		}

		i++;
		if(i==1){
			i = 0 ;
			memset(inputx, 0, sizeof(inputx));
			memset(inputy, 0, sizeof(inputy));
			memset(inputz, 0, sizeof(inputz));
		}

	}
}
*/
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
