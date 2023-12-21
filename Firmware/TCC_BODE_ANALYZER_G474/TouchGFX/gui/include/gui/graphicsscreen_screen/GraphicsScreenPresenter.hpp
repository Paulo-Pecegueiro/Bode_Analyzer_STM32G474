#ifndef GRAPHICSSCREENPRESENTER_HPP
#define GRAPHICSSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GraphicsScreenView;

class GraphicsScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GraphicsScreenPresenter(GraphicsScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~GraphicsScreenPresenter() {}

private:
    GraphicsScreenPresenter();

    GraphicsScreenView& view;
};

#endif // GRAPHICSSCREENPRESENTER_HPP
