/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : STM32TouchController.cpp
 ******************************************************************************
 * This file was created by TouchGFX Generator 4.22.1. This file is only
 * generated once! Delete this file from your project and re-generate code
 * using STM32CubeMX or change this file manually to update it.
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

/* USER CODE BEGIN STM32TouchController */
/* Private Includes */
extern "C"
{
#include "GT9147.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
}

#include <STM32TouchController.hpp>

/* Imported variables */
extern FlagStatus ICAddress;

/* Private variables */
FlagStatus IsTouchInitialized = RESET;

/* Private Function Prototypes */
extern "C"{
void SampleTouch_Task(void *argument);
}

/* Private Functions */
void SampleTouch_Task(void *argument)
{
	/* Wait for the touch to be initialized */
	while(IsTouchInitialized == RESET)
	{
		osDelay(1);
	}

	/* Infinite loop */
	for(;;)
	{
		/* Check if there is a touch */
		GT9147_Scan();

		/* Wait for 50ms */
		osDelay(50);
	}
}

void STM32TouchController::init()
{

}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
	/* Check if the touch is initialized */
	if(IsTouchInitialized == SET)
	{
		/* Check if there is a touch */
		if(sLcdTouchInfos[0].touch != 0)
		{
			/* Save the touch coordinates */
			x = sLcdTouchInfos[0].x;
			y = sLcdTouchInfos[0].y;

			/* Return true to indicate a touch */
			return true;
		}
	}

	/* Return false to indicate no touch */
	return false;
}

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
