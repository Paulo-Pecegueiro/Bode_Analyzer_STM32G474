#ifndef GRAPHICSSCREENVIEW_HPP
#define GRAPHICSSCREENVIEW_HPP

#include <gui_generated/graphicsscreen_screen/GraphicsScreenViewBase.hpp>
#include <gui/graphicsscreen_screen/GraphicsScreenPresenter.hpp>

class GraphicsScreenView : public GraphicsScreenViewBase
{
public:
	void UpdateMagGraph();
	void UpdatePheGraph();
	GraphicsScreenView();
	virtual ~GraphicsScreenView() {}
	virtual void setupScreen();
	virtual void tearDownScreen();
	virtual void TransitionEnds();
	virtual void Back_Click();
protected:
};

#endif // GRAPHICSSCREENVIEW_HPP
