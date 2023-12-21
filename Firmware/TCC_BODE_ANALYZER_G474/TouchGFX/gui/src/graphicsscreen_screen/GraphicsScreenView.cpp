/* Private includes */
#include <gui/graphicsscreen_screen/GraphicsScreenView.hpp>
#include "cmsis_os2.h"
#include "NT35510.h"
#include "app_main.h"

/* Private Variables */
const uint16_t FrequenciesAbs[37] = {
		0,  54,  85, 108, 125, 139, 151, 162, 172,		// Freq [Hz] : 1, 2, 3, 4, 5, 6, 7, 8, 9
		179, 234, 265, 288, 305, 318, 330, 341, 351,	// Freq [Hz] : 10, 20 ,30, 40, 50, 60, 70, 80, 90
		359, 413, 443, 466, 485, 497, 509, 520, 530,	// Freq [Hz] : 100, 200, 300, 400, 500, 600, 700, 800, 900
		537, 591, 623, 645, 663, 676, 688, 699, 709,	// Freq [Hz] : 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000
		715												// Freq [Hz] : 10000
};

/* Private functions */
void GraphicsScreenView::UpdateMagGraph(void)
{
	/* Local variables */
	float limMinX = Acq.gain[0], labelStep;

	/* Search for the minor gain value */
	for(uint8_t i = 1; i < FREQ_COUNT - 1; i++)
	{
		if(Acq.gain[i] < limMinX)
			limMinX = Acq.gain[i];
	}

	/* Calculate a step of 10dB */
	limMinX = ((limMinX / 10) - 1) * 10;

	/* Adjust the graphic range */
	MagnitudeGraph.setGraphRange(0, 715, limMinX - 1, (float)1.00);
	MagnitudeGraph.invalidate();

	/* Adjust the grid line */
	MagnitudeGraphMajorYAxisGrid.setInterval((float)((limMinX/ 4) * -1));
	MagnitudeGraphMajorYAxisGrid.invalidate();

	/* Calculate the graph labels */
	limMinX = limMinX / 4;
	labelStep = limMinX;

	/* Update the graphic labels */
	Unicode::snprintf(lblMag1Buffer, LBLMAG1_SIZE, "%4d", (int)limMinX);
	lblMag1.invalidate();

	limMinX += labelStep;

	Unicode::snprintf(lblMag2Buffer, LBLMAG2_SIZE, "%4d", (int)limMinX);
	lblMag2.invalidate();

	limMinX += labelStep;

	Unicode::snprintf(lblMag3Buffer, LBLMAG3_SIZE, "%4d", (int)limMinX);
	lblMag3.setWildcard(lblMag3Buffer);
	lblMag3.invalidate();

	limMinX += labelStep;

	Unicode::snprintf(lblMag4Buffer, LBLMAG4_SIZE, "%4d", (int)limMinX);
	lblMag4.invalidate();
}

void GraphicsScreenView::UpdatePheGraph(void)
{
	/* Local variables */
	float maxPhase = Acq.phase[0], minPhase = Acq.phase[0];
	int uplimit, lwlimit, amplitude;
	float labelcount;

	/* Search for the graphic limits */
	for(uint8_t i = 1; i < FREQ_COUNT - 1; i++)
	{
		if(Acq.phase[i] > maxPhase)
			maxPhase = Acq.phase[i];

		if(Acq.phase[i] < minPhase)
			minPhase = Acq.phase[i];
	}

	/* Calculate the limits */
	if(maxPhase != 0)
	{
		uplimit = (int)maxPhase / 30;
		uplimit = (uplimit + 1) * 30;
	}
	else
	{
		uplimit = 0;
	}

	if(minPhase != 0)
	{
		lwlimit = (int)minPhase / 30;
		lwlimit = (lwlimit - 1) * 30;
	}
	else
	{
		lwlimit = 0;
	}
	/* Update the graphic range */
	PhaseGraph.setGraphRange(0, 715, lwlimit, uplimit);
	PhaseGraph.invalidate();

	/* Get the graphic amplitude */
	if(uplimit > 0)
		amplitude = uplimit - lwlimit;

	else
		amplitude = abs(lwlimit) + uplimit;

	/* Update the grids */
	PhaseGraphMajorYAxisGrid.setInterval(amplitude/4);
	PhaseGraphMajorYAxisGrid.invalidate();

	/* Update the graphic labels */
	labelcount = lwlimit;

	Unicode::snprintf(lblPhase5Buffer, LBLPHASE5_SIZE, "%4d", (int)labelcount);
	lblPhase5.invalidate();

	labelcount += amplitude/4;

	Unicode::snprintf(lblPhase4Buffer, LBLPHASE4_SIZE, "%4d", (int)labelcount);
	lblPhase4.invalidate();

	labelcount += amplitude/4;

	Unicode::snprintf(lblPhase3Buffer, LBLPHASE3_SIZE, "%4d", (int)labelcount);
	lblPhase3.invalidate();

	labelcount += amplitude/4;

	Unicode::snprintf(lblPhase2Buffer, LBLPHASE2_SIZE, "%4d", (int)labelcount);
	lblPhase2.invalidate();

	labelcount += amplitude/4;

	Unicode::snprintf(lblPhase1Buffer, LBLPHASE1_SIZE, "%4d", (int)labelcount);
	lblPhase1.invalidate();
}

GraphicsScreenView::GraphicsScreenView()
{

}

void GraphicsScreenView::setupScreen()
{
	GraphicsScreenViewBase::setupScreen();
}

void GraphicsScreenView::tearDownScreen()
{
	GraphicsScreenViewBase::tearDownScreen();
}

void GraphicsScreenView::TransitionEnds()
{
	/* check for invalid value at the beginning */
	if(Acq.phase[0] == 0xFF800000)
	{
		/* check Acq.phase for the first valid value */
		uint8_t fst_valid_phase_indx = FREQ_COUNT - 1;
		for(uint8_t i = 0; i < FREQ_COUNT - 1; i++)
		{
			// invalid phase
			if(Acq.phase[i] != 0xFF800000)
			{
				fst_valid_phase_indx = i;
				break;
			}
		}

		/* replace everything up to fst_valid_phase_indx if it is the start of the Acq.phase array */
		for(uint8_t i = 0; i < fst_valid_phase_indx; i++)
		{
			Acq.phase[i] = Acq.phase[fst_valid_phase_indx];
		}
	}

	/* Adjust the magnitude graphic to be plotted */
	UpdateMagGraph();

	/* Adjust the phase graphic to be plotted */
	UpdatePheGraph();

	/* Plot the Graph */
	for(uint8_t i = 0; i < FREQ_COUNT - 1; i++)
	{
		MagnitudeGraph.addDataPoint((float)FrequenciesAbs[i], Acq.gain[i]);

		if(Acq.phase[i] != 0xFF800000)
			PhaseGraph.addDataPoint((float)FrequenciesAbs[i], Acq.phase[i]);
	}

	/* Wait the screen to be drawn */
	osDelay(200);

	/* Turn the Screen ON */
	NT35510_FadeIn(1);
}

void GraphicsScreenView::Back_Click()
{
	/* Turn the screen off */
	NT35510_FadeOut(1);

	/* Back to main menu */
	BackToMain();
}
