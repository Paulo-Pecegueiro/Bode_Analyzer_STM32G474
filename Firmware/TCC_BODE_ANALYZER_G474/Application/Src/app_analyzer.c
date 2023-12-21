/*
 * app_analyzer.c
 *
 *  Created on: Dec 5, 2023
 *      Author: daniel, paulo
 */
/* Private Includes */
#include "app_analyzer.h"
#include "app_compensation.h"
#include <math.h>
#include <stdbool.h>

/* Private Defines */
#define RANGE_TO_EVALUATE 5
#define CRITICAL_MAG -25.0

/* Private global variables */
// statics global variables
static uint16_t adc1_offset = 0;
static uint16_t adc2_offset = 0;

/* Private Functions */
static bool is_rising_zero_crossing_invalid(uint16_t *zero_crossing, int range_to_evaluate)
{
	bool ret = false;

	uint32_t bk_avg = 0;
	uint32_t fwd_avg = 0;
	int bk_indx = (-1*range_to_evaluate) - 1;
	int fwd_indx = range_to_evaluate + 1;

	// evaluates the range of samples behind and in front of zero crossing detection
	for(int i = bk_indx; i < fwd_indx; i++)
	{
		if(i < 0)
		{
			bk_avg += zero_crossing[i];
		}
		else if(i > 0)
		{
			fwd_avg += zero_crossing[i];
		}
	}

	bk_avg /= range_to_evaluate;
	fwd_avg /= range_to_evaluate;

	if(bk_avg >= fwd_avg)
	{
		ret  = true;
	}

	return ret;
}

static bool is_falling_zero_crossing_invalid(uint16_t *zero_crossing, int range_to_evaluate)
{
	bool ret = false;

	uint32_t bk_avg = 0;
	uint32_t fwd_avg = 0;
	int bk_indx = (-1*range_to_evaluate) - 1;
	int fwd_indx = range_to_evaluate + 1;

	// evaluates the range of samples behind and in front of zero crossing detection
	for(int i = bk_indx; i < fwd_indx; i++)
	{
		if(i < 0)
		{
			bk_avg += zero_crossing[i];
		}
		else if(i > 0)
		{
			fwd_avg += zero_crossing[i];
		}
	}

	bk_avg /= range_to_evaluate;
	fwd_avg /= range_to_evaluate;

	if(bk_avg <= fwd_avg)
	{
		ret  = true;
	}

	return ret;
}

static float APP_Calculates_Phase(uint16_t* pDataOut, uint16_t* pDataIn)
{
	float phase = 0.0;
	float z_cross_index1 = 0.0, z_cross_index2 = 0.0;
	bool z_cross1 = false, z_cross2 = false;

	/* Scan the 5 available cycles */
	/* Search the cross in output & input signals */
	for(uint16_t i = 0; i < 500; i++)
	{
		uint16_t next_i = i + 1;

		/* Check if a falling cross happened in generated signal */
		if(pDataIn[i] > adc2_offset && pDataIn[next_i] <= adc2_offset)
		{
			/* Evaluate the crossing */
			if(is_falling_zero_crossing_invalid(&pDataIn[i], RANGE_TO_EVALUATE))
			{
				z_cross1 = false;
				z_cross2 = false;
				i = (1 + (i/100))*100;
			}
			else
			{
				z_cross2 = true;
				if(pDataIn[next_i] == adc2_offset)
				{
					z_cross_index2 += (float)next_i;
				}
				else
				{
					// Calculate the value of z_cross_index using the inverse of line function
					// (x = (y - y1) * (x2 - x1) / (y2 - y1) + x1)
					z_cross_index2 += (adc2_offset - (float)pDataIn[i]) / ((float)pDataIn[i+1] - (float)pDataIn[i]) + (float)i;
				}

				/* If not the first cycle, divide by 2 */
				if(i/100 >= 1)
					z_cross_index2 /= 2.0;
			}
		}

		/* Check if a falling cross happened in acquired signal */
		if(pDataOut[i] > adc1_offset && pDataOut[next_i] <= adc1_offset)
		{
			/* Evaluate the crossing */
			if(is_falling_zero_crossing_invalid(&pDataOut[i], RANGE_TO_EVALUATE))
			{
				z_cross1 = false;
				z_cross2 = false;
				i = (1 + (i/100))*100;
			}
			else
			{
				z_cross1 = true;
				if(pDataIn[next_i] == adc1_offset)
				{
					z_cross_index1 += (float)next_i;
				}
				else
				{
					// Calculate the value of z_cross_index using the inverse of line function
					// (x = (y - y1) * (x2 - x1) / (y2 - y1) + x1)
					z_cross_index1 += (adc1_offset - (float)pDataOut[i]) / ((float)pDataOut[i+1] - (float)pDataOut[i]) + (float)i;
				}

				/* If not the first cycle, get the avg by divide by 2 */
				if(i/100 >= 1)
					z_cross_index1 /= 2.0;
			}
		}

		if(z_cross1 && z_cross2)
		{
			z_cross1 = false;
			z_cross2 = false;
			i = (1 + (i/100))*100;
		}
	}

	/* Compute the phase */
	phase = ((z_cross_index2 - z_cross_index1) * 360)/100;

	return phase;
}

static float APP_Calculates_Magnitude(uint16_t* pDataOut, uint16_t* pDataIn)
{
	/* Local Variables */
	float magnitude = 0.0;
	float in_max_avg = 0, out_max_avg = 0, in_min_avg = 0, out_min_avg = 0;
	uint16_t in_max[5], in_min[5], out_max[5], out_min[5];
	uint16_t cycle_indx = 0, sample_indx = 0;

	/* Search for the peaks */
	for(uint8_t i = 0; i < 5; i++)
	{
		/* get the cycle index */
		cycle_indx = i*100;

		/* presets values ​​at the beginning of each cycle */
		in_max[i] = pDataIn[cycle_indx];
		out_max[i] = pDataOut[cycle_indx];
		in_min[i] = pDataIn[cycle_indx];
		out_min[i] = pDataOut[cycle_indx];

		/* Evaluate the points */
		for(uint16_t j = 1; j < 100; j++)
		{
			/* get the sample index */
			sample_indx = cycle_indx + j;

			if(pDataIn[sample_indx] > in_max[i])			/* Check the input peak */
			{
				in_max[i] = pDataIn[sample_indx];
			}
			else if(pDataIn[sample_indx] < in_min[i]) 		/* Check the input minimal */
			{
				in_min[i] = pDataIn[sample_indx];
			}

			if(pDataOut[sample_indx] > out_max[i])			/* Check the input peak */
			{
				out_max[i] = pDataOut[sample_indx];
			}
			else if(pDataOut[sample_indx] < out_min[i])		/* Check the output minimal */
			{
				out_min[i] = pDataOut[sample_indx];
			}
		}

		/* Add the computed values into the averaging buffer */
		in_max_avg += in_max[i];
		out_max_avg += out_max[i];

		in_min_avg += in_min[i];
		out_min_avg += out_min[i];
	}

	/* Calculate the mean */
	in_max_avg = in_max_avg/5;
	out_max_avg = out_max_avg/5;
	in_min_avg = in_min_avg/5;
	out_min_avg = out_min_avg/5;

	/* Calculate the offset */
	adc1_offset = ((out_max_avg - out_min_avg)/2) + out_min_avg;
	adc2_offset = ((in_max_avg - in_min_avg)/2) + in_min_avg;

	/* Compute the gain */
	magnitude = 20*log10f((float)((out_max_avg - out_min_avg)/(in_max_avg - in_min_avg)));

	return magnitude;
}

void APP_ProcessFrequencyData(Frequencies_e frequency, uint16_t* pDataOut, uint16_t* pDataIn)
{
	/* preset invalid value for last_valid_phase */
	static float last_valid_phase = 0xFF800000;

	/* reset last_valid_phase every new bode plot */
	if(Acq.frequency == FREQ_1Hz)
		last_valid_phase = 0xFF800000;

	/* Calculates the magnitude */
	Acq.gain[Acq.frequency] = APP_Calculates_Magnitude(pDataOut, pDataIn);

	if(Acq.compensation == SET)
	{
		Acq.gain[Acq.frequency] = Acq.gain[Acq.frequency] - compensation_gain_values[Acq.frequency];
	}

	/* inhibit phase calculation if Magnitude is less than CRITICAL_MAG */
	if(Acq.gain[Acq.frequency] < CRITICAL_MAG)
	{
		// 	if last_valid_phase has never been calculated before,
		//	Acq.phase[Acq.frequency] will carry an invalid value of 0xFF800000, i.e. the minimum negative value of a float
		// 	otherwise, last_valid_phase will carry the last valid phase, i.e. when Acq.gain was greater than CRITICAL_MAG
		Acq.phase[Acq.frequency] = last_valid_phase;
	}
	else
	{
		/* Calculates the phase */
		Acq.phase[Acq.frequency] = APP_Calculates_Phase(pDataOut, pDataIn);
		last_valid_phase = Acq.phase[Acq.frequency];

		if(Acq.compensation == SET)
		{
			if(compensation_phase_values[Acq.frequency] > 0.0)
			{
				Acq.phase[Acq.frequency] -= compensation_phase_values[Acq.frequency];
			}
			else
			{
				Acq.phase[Acq.frequency] += (-1.0)*compensation_phase_values[Acq.frequency];
			}
		}
	}
}
