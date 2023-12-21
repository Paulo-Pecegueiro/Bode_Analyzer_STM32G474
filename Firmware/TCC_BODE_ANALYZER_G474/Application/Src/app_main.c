/*
 * app_main.c
 *
 *  Created on: Dec 2, 2023
 *      Author: daniel, paulo
 */
/* Private includes */
#include "main.h"

#include "app_main.h"
#include "app_constants.h"
#include "app_analyzer.h"
#include "app_filesystem.h"

#include "dac.h"
#include "adc.h"
#include "tim.h"

/* Private defines */
#define NUMBER_OF_CYCLES				10

/* Private variables */
AcquiringSettings_t Acq = {
		.frequency = FREQ_1Hz,
		.status =ACQ_IDLE
};
uint16_t outputBuffer1[500];		// DAC output collected from ADC1
uint16_t outputBuffer2[500];		// DUT output collected from ADC2

/* Private Functions */
void AcquisitionTaskEntry(void *argument)
{
	/* Local Variables */
	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_4,
			.Mode = GPIO_MODE_ANALOG,
			.Pull = GPIO_NOPULL
	};

	/* Calibrate the ADC */
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

	/* Infinite loop */
	for(;;)
	{
		/* Wait for a semaphore to start the Acquisition */
		osSemaphoreAcquire(AcquisitionSemHandle, osWaitForever);

		/* Connect the DAC into the GPIO Pin */
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Start the DAC in DMA Mode */
		HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)sinewave, sizeof(sinewave)/2, DAC_ALIGN_12B_R);

		/* Configure and start the timer */
		__HAL_TIM_SET_PRESCALER(&htim2, timPrescalers[Acq.frequency]);
		__HAL_TIM_SET_AUTORELOAD(&htim2, timReload[Acq.frequency]);
		HAL_TIM_Base_Start(&htim2);

		while(1)
		{
			/* Wait for the Wave to be generated */
			osSemaphoreAcquire(DacIntSemHandle, osWaitForever);

			if(Acq.status == ACQ_ABORTED)
			{
				/* Update the application status */
				Acq.status = ACQ_IDLE;

				break;
			}
			else if(Acq.status == ACQ_AWAITING_FILTERS)
			{
				/* Update the application status */
				Acq.status = ACQ_ACQUIRING;

				/* Adjust the Frequency to be the first one */
				Acq.frequency = FREQ_1Hz;

				/* Reconfigure the timer */
				__HAL_TIM_SET_PRESCALER(&htim2, timPrescalers[Acq.frequency]);
				__HAL_TIM_SET_AUTORELOAD(&htim2, timReload[Acq.frequency]);
				__HAL_TIM_SET_COUNTER(&htim2, 0);

				/* Start the ADC in DMA Mode */
				HAL_ADC_Start_DMA(&hadc1, (uint32_t *)outputBuffer1, sizeof(outputBuffer1)/2);
				HAL_ADC_Start_DMA(&hadc2, (uint32_t *)outputBuffer2, sizeof(outputBuffer2)/2);

//				HAL_ADC_Start_DMA(&hadc1, (uint32_t *)outputBuffer1, 1);
//				HAL_ADC_Start_DMA(&hadc2, (uint32_t *)outputBuffer2, 2);

				/* Restart the timer */
				__HAL_TIM_ENABLE(&htim2);

			}
			else if(Acq.frequency >= FREQ_COUNT - 1)
			{
				/* Get the results */
				APP_ProcessFrequencyData(Acq.frequency, outputBuffer1, outputBuffer2);

				/* Finish the Acquisition*/
				APP_FinishAcquisition();

				/* Update the application status */
				Acq.status = ACQ_CALCULATING;

				/* Give a delay to update the screen */
				osDelay(10);

				/* Save data into the external memory */
				APP_WriteFileContent();

				/* Update the application status */
				Acq.status = ACQ_FINISHED;

				/* Break the loop */
				break;
			}
			else
			{
				/* Get the results */
				APP_ProcessFrequencyData(Acq.frequency, outputBuffer1, outputBuffer2);

				/* Update to the new frequency*/
				Acq.frequency++;

				/* Reconfigure the timer */
				__HAL_TIM_SET_PRESCALER(&htim2, timPrescalers[Acq.frequency]);
				__HAL_TIM_SET_AUTORELOAD(&htim2, timReload[Acq.frequency]);
				__HAL_TIM_SET_COUNTER(&htim2, 0);

				/* Restart the timer */
				__HAL_TIM_ENABLE(&htim2);
			}

			/* Reset the repetition counter */
			Acq.repetition = 0;
		}
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
	/* Local Variables */
	uint32_t numberOfCycles;

	/* Check the application status */
	if(Acq.status == ACQ_AWAITING_FILTERS)
		numberOfCycles = FILTER_WARMING_CYCLES;
	else
		numberOfCycles = repetition[Acq.frequency];

	/* Check if the waves were generated */
	if(Acq.repetition == numberOfCycles)
	{
		/* Stop the timer */
		__HAL_TIM_DISABLE(&htim2);

		/* Release the semaphore */
		osSemaphoreRelease(DacIntSemHandle);
	}
	else
	{
		/* Increase the counter */
		Acq.repetition++;
	}
}

/* Exported Functions */
void APP_FinishAcquisition(void)
{
	/* Local Variables */
	GPIO_InitTypeDef GPIO_InitStruct = {
			.Pin = GPIO_PIN_4,
			.Mode = GPIO_MODE_OUTPUT_PP,
			.Pull = GPIO_NOPULL
	};

	/* Stop the resources */
	HAL_TIM_Base_Stop(&htim2);
	HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
	HAL_ADC_Stop_DMA(&hadc1);
	HAL_ADC_Stop_DMA(&hadc2);

	/* Configure the GPIO as Output push-pull */
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Restart the frequency */
	Acq.frequency = FREQ_1Hz;

	/* Reset the repetition counter */
	Acq.repetition = 0;
}
