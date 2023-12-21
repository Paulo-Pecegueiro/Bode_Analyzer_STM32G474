/* Private Includes */
#include <gui/acquiring_screen/AcquiringView.hpp>
extern "C"
{
#include "app_main.h"
}
#include "NT35510.h"

/* Private Variables */


/* Private Functions */
AcquiringView::AcquiringView()
{

}

void AcquiringView::setupScreen()
{
	AcquiringViewBase::setupScreen();
}

void AcquiringView::tearDownScreen()
{
	AcquiringViewBase::tearDownScreen();
}

void AcquiringView::TransitionEnds()
{
	/* Adjust the labels visibility */
	lblFilterON.setVisible(true);
	lblFilterSettingTime.setVisible(false);
	lblTestDone.setVisible(false);
	lblTestingDevice.setVisible(false);

	/* Invalidate the labels to be rendered */
	lblFilterON.invalidate();
	lblFilterSettingTime.invalidate();
	lblTestDone.invalidate();
	lblTestingDevice.invalidate();

	/* Hide the warning message */
	acquisitionAborted1.setVisible(false);
	acquisitionAborted1.invalidate();

	/* Turn the screen ON */
	NT35510_FadeIn(1);

	/* Wait */
	HAL_Delay(500);

	/* Hide the first label */
	lblFilterON.setVisible(false);
	lblFilterON.invalidate();

	/* Update the progress indicators */
	imgProgress.setValue(5);
	imgProgress.invalidate();
	txtProgress.setValue(5);
	txtProgress.invalidate();

	/* Show the filter setting time label */
	lblFilterSettingTime.setVisible(true);
	lblFilterSettingTime.invalidate();

	/* Update the application status */
	Acq.status = ACQ_AWAITING_FILTERS;

	/* Update the Frequency */
	Acq.frequency = FILTER_WARMING_FREQ;

	/* Release the Semaphore to start the Acquisition */
	osSemaphoreRelease(AcquisitionSemHandle);
}

void AcquiringView::Abort_Click()
{
	/* Check the application status */
	if(Acq.status != ACQ_IDLE)
	{
		/* Update the application status */
		Acq.status = ACQ_ABORTED;

		/* Finish the acquisition */
		APP_FinishAcquisition();

		/* Release the semaphore to update the application loop */
		osSemaphoreRelease(DacIntSemHandle);

		/* Show the warning message */
		acquisitionAborted1.setVisible(true);
		acquisitionAborted1.invalidate();
	}
}

void AcquiringView::Ok_Click()
{
	/* Turn the screen off */
	NT35510_FadeOut(1);

	/* Back to main menu */
	GoToMainMenu();
}

void AcquiringView::ScreenTick()
{
	/* Local Variables */
	static AcquisitionStatus_e oldStatus = Acq.status;
	static Frequencies_e oldFrequency = Acq.frequency;

	/* Check if something has changed */
	if((oldStatus != Acq.status) || (oldFrequency != Acq.frequency))
	{
		switch((uint8_t)Acq.status)
		{
			case ACQ_ACQUIRING:

				if(lblFilterSettingTime.isVisible() == true)
				{
					lblFilterSettingTime.setVisible(false);
					lblTestingDevice.setVisible(true);
					lblFilterSettingTime.invalidate();
					lblTestingDevice.invalidate();

					imgProgress.setValue(10);
					txtProgress.setValue(10);
					imgProgress.invalidate();
					txtProgress.invalidate();
				}

				if(Acq.frequency == FREQ_1Hz)
				{
					Unicode::snprintf(lblTestingDeviceBuffer, LBLTESTINGDEVICE_SIZE, "%d", 1);
					lblTestingDevice.invalidate();
				}
				else if(Acq.frequency == FREQ_10Hz)
				{
					Unicode::snprintf(lblTestingDeviceBuffer, LBLTESTINGDEVICE_SIZE, "%d", 2);
					lblTestingDevice.invalidate();
				}
				else if(Acq.frequency == FREQ_100Hz)
				{
					Unicode::snprintf(lblTestingDeviceBuffer, LBLTESTINGDEVICE_SIZE, "%d", 3);
					lblTestingDevice.invalidate();
				}
				else if(Acq.frequency == FREQ_1000Hz)
				{
					Unicode::snprintf(lblTestingDeviceBuffer, LBLTESTINGDEVICE_SIZE, "%d", 4);
					lblTestingDevice.invalidate();
				}

				imgProgress.setValue(10 + (Acq.frequency * 2.4));
				txtProgress.setValue(10 + (Acq.frequency * 2.4));
				imgProgress.invalidate();
				txtProgress.invalidate();

				break;


			case ACQ_CALCULATING:
				if(txtProgress.getValue() != 100)
				{
					lblTestingDevice.setVisible(false);
					lblTestingDevice.invalidate();
					lblTestDone.setVisible(true);
					lblTestDone.invalidate();

					imgProgress.setValue(100);
					txtProgress.setValue(100);
					imgProgress.invalidate();
					txtProgress.invalidate();
				}
				break;


			case ACQ_FINISHED:

				/* Turn the Screen off */
				NT35510_FadeOut(1);

				/* Jump to the results */
				GoToGraphs();

				break;
		}

		/* Save the older values */
		oldStatus = Acq.status;
		oldFrequency = Acq.frequency;
	}
}
