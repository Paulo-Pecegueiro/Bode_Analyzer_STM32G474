#ifndef ACQUIRINGPRESENTER_HPP
#define ACQUIRINGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class AcquiringView;

class AcquiringPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    AcquiringPresenter(AcquiringView& v);

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

    virtual ~AcquiringPresenter() {}

private:
    AcquiringPresenter();

    AcquiringView& view;
};

#endif // ACQUIRINGPRESENTER_HPP
