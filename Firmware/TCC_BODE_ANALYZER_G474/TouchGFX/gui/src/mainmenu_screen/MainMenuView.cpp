#include <gui/mainmenu_screen/MainMenuView.hpp>
extern "C"
{
#include "main.h"
#include "NT35510.h"
#include "app_filesystem.h"
}

extern FlagStatus IsTouchInitialized;

MainMenuView::MainMenuView()
{

}

void MainMenuView::setupScreen()
{
    MainMenuViewBase::setupScreen();
}

void MainMenuView::tearDownScreen()
{
    MainMenuViewBase::tearDownScreen();
}

void MainMenuView::TransitionEnds()
{
	/* Turn the screen ON */
	NT35510_FadeIn(1);
}

void MainMenuView::BodeAnalyzer_Click()
{
	/* Turn the screen OFF */
	NT35510_FadeOut(1);

	/* Jump to the next screen */
	GoToBodeAnalyzer();
}

void MainMenuView::About_Click()
{
	/* Turn the screen OFF */
	NT35510_FadeOut(1);

	/* Jump to the next screen */
	GoToAbout();
}

void MainMenuView::ShowGraphs_Click()
{
	/* Turn the screen OFF */
	NT35510_FadeOut(1);

	/* Jump to the next screen */
	GoToGraphs();
}

void MainMenuView::FormatFS_Click()
{
	/* Format the File System */
	APP_FormatFS();

	/* Show the confirmation container */
	formatEnd1.setVisible(true);

	/* Render the container */
	formatEnd1.invalidate();
}
