#include <gui/startup_screen/StartupView.hpp>

extern "C"
{
#include "tim.h"
#include "NT35510.h"
}

extern FlagStatus IsTouchInitialized;

StartupView::StartupView()
{

}

void StartupView::setupScreen()
{
    StartupViewBase::setupScreen();
}

void StartupView::tearDownScreen()
{
    StartupViewBase::tearDownScreen();
}

void StartupView::ApplicationStart()
{
	LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableCounter(TIM3);
	NT35510_FadeIn(4);
}

void StartupView::LogoPresentationDelayElapsed()
{
	if(IsTouchInitialized == SET)
	{
		NT35510_FadeOut(4);
		JumpToMainMenu();
	}
	else
	{
	}
}
