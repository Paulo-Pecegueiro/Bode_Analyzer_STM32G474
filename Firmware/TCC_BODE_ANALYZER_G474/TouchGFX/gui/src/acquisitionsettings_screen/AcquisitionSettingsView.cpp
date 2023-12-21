#include <gui/acquisitionsettings_screen/AcquisitionSettingsView.hpp>
#include "NT35510.h"
#include "app_main.h"

AcquisitionSettingsView::AcquisitionSettingsView()
{

}

void AcquisitionSettingsView::setupScreen()
{
    AcquisitionSettingsViewBase::setupScreen();
}

void AcquisitionSettingsView::tearDownScreen()
{
    AcquisitionSettingsViewBase::tearDownScreen();
}

void AcquisitionSettingsView::TransitionEnds()
{
	/* Turn the screen ON */
	NT35510_FadeIn(1);
}

void AcquisitionSettingsView::StartAqcuisition_Click()
{
	/* Save if the compensation will be applied */
	if(toggleButton1.getState() == true)
		Acq.compensation = SET;
	else
		Acq.compensation = RESET;

	/* Turn the screen off */
	NT35510_FadeOut(1);

	/* Jump to the Acquisition screen */
	GoToAcquiring();
}

void AcquisitionSettingsView::Back_Click()
{
	/* Turn the screen off */
	NT35510_FadeOut(1);

	/* Jump to Main Menu */
	GoToMainMenu();
}
