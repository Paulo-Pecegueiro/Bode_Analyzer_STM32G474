#include <gui/about_screen/AboutView.hpp>
#include "NT35510.h"

AboutView::AboutView()
{

}

void AboutView::setupScreen()
{
    AboutViewBase::setupScreen();
}

void AboutView::tearDownScreen()
{
    AboutViewBase::tearDownScreen();
}

void AboutView::Back_Click()
{
	NT35510_FadeOut(1);
	GoToMainMenu();
}

void AboutView::TransitionEnds()
{
	NT35510_FadeIn(1);
}
