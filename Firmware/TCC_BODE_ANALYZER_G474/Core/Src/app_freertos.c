/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for AcquisitionTask */
osThreadId_t AcquisitionTaskHandle;
const osThreadAttr_t AcquisitionTask_attributes = {
  .name = "AcquisitionTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 4096 * 4
};
/* Definitions for TouchGFXTask */
osThreadId_t TouchGFXTaskHandle;
const osThreadAttr_t TouchGFXTask_attributes = {
  .name = "TouchGFXTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 4096 * 4
};
/* Definitions for SampleTouch */
osThreadId_t SampleTouchHandle;
const osThreadAttr_t SampleTouch_attributes = {
  .name = "SampleTouch",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 4
};
/* Definitions for ExtFlashMtx */
osMutexId_t ExtFlashMtxHandle;
const osMutexAttr_t ExtFlashMtx_attributes = {
  .name = "ExtFlashMtx"
};
/* Definitions for FlashDMASem */
osSemaphoreId_t FlashDMASemHandle;
const osSemaphoreAttr_t FlashDMASem_attributes = {
  .name = "FlashDMASem"
};
/* Definitions for AcquisitionSem */
osSemaphoreId_t AcquisitionSemHandle;
const osSemaphoreAttr_t AcquisitionSem_attributes = {
  .name = "AcquisitionSem"
};
/* Definitions for DacIntSem */
osSemaphoreId_t DacIntSemHandle;
const osSemaphoreAttr_t DacIntSem_attributes = {
  .name = "DacIntSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AcquisitionTaskEntry(void *argument);
extern void TouchGFX_Task(void *argument);
extern void SampleTouch_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of ExtFlashMtx */
  ExtFlashMtxHandle = osMutexNew(&ExtFlashMtx_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of FlashDMASem */
  FlashDMASemHandle = osSemaphoreNew(1, 1, &FlashDMASem_attributes);

  /* creation of AcquisitionSem */
  AcquisitionSemHandle = osSemaphoreNew(1, 1, &AcquisitionSem_attributes);

  /* creation of DacIntSem */
  DacIntSemHandle = osSemaphoreNew(1, 1, &DacIntSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  osSemaphoreAcquire(FlashDMASemHandle, osWaitForever);
  osSemaphoreAcquire(AcquisitionSemHandle, osWaitForever);
  osSemaphoreAcquire(DacIntSemHandle, osWaitForever);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of AcquisitionTask */
  AcquisitionTaskHandle = osThreadNew(AcquisitionTaskEntry, NULL, &AcquisitionTask_attributes);

  /* creation of TouchGFXTask */
  TouchGFXTaskHandle = osThreadNew(TouchGFX_Task, NULL, &TouchGFXTask_attributes);

  /* creation of SampleTouch */
  SampleTouchHandle = osThreadNew(SampleTouch_Task, NULL, &SampleTouch_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AcquisitionTaskEntry */
/**
* @brief Function implementing the AquisitionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AcquisitionTaskEntry */
__weak void AcquisitionTaskEntry(void *argument)
{
  /* USER CODE BEGIN AcquisitionTaskEntry */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END AcquisitionTaskEntry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

