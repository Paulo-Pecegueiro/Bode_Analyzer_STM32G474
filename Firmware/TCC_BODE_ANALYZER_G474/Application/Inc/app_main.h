/*
 * app_main.h
 *
 *  Created on: Dec 2, 2023
 *      Author: daniel, paulo
 */

#ifndef INC_APP_MAIN_H_
#define INC_APP_MAIN_H_

/* Exported includes */
#include "main.h"
#include "cmsis_os2.h"

/* Exported Defines */
#define FILTER_WARMING_FREQ				FREQ_1Hz
#define FILTER_WARMING_CYCLES			10

/* Exported Types */
typedef enum
{
	FREQ_1Hz,    FREQ_2Hz,	   FREQ_3Hz,     FREQ_4Hz,    FREQ_5Hz,
	FREQ_6Hz,    FREQ_7Hz,	   FREQ_8Hz,     FREQ_9Hz,	  FREQ_10Hz,
	FREQ_20Hz,   FREQ_30Hz,    FREQ_40Hz,    FREQ_50Hz,   FREQ_60Hz,
	FREQ_70Hz,   FREQ_80Hz,	   FREQ_90Hz,    FREQ_100Hz,  FREQ_200Hz,
	FREQ_300Hz,  FREQ_400Hz,   FREQ_500Hz,   FREQ_600Hz,  FREQ_700Hz,
	FREQ_800Hz,  FREQ_900Hz,   FREQ_1000Hz,  FREQ_2000Hz, FREQ_3000Hz,
	FREQ_4000Hz, FREQ_5000Hz,  FREQ_6000Hz,  FREQ_7000Hz, FREQ_8000Hz,
	FREQ_9000Hz, FREQ_10000Hz, FREQ_20000Hz, FREQ_COUNT
}Frequencies_e;

typedef enum
{
	ACQ_IDLE,
	ACQ_AWAITING_FILTERS,
	ACQ_ACQUIRING,
	ACQ_CALCULATING,
	ACQ_ABORTED,
	ACQ_FINISHED,
	ACQ_COUNT
}AcquisitionStatus_e;

typedef struct
{
	uint16_t repetition;
	Frequencies_e frequency;
	AcquisitionStatus_e status;
	FlagStatus compensation;
	float gain[38];
	float phase[38];
}AcquiringSettings_t;

/* Exported Variables */
extern AcquiringSettings_t Acq;
extern osSemaphoreId_t AcquisitionSemHandle;
extern osSemaphoreId_t DacIntSemHandle;

/* Exported Function Prototypes */
void APP_FinishAcquisition(void);

#endif /* INC_APP_MAIN_H_ */
