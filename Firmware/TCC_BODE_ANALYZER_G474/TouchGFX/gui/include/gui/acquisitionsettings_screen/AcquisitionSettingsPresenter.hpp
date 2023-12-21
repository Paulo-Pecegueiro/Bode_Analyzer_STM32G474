#ifndef ACQUISITIONSETTINGSPRESENTER_HPP
#define ACQUISITIONSETTINGSPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class AcquisitionSettingsView;

class AcquisitionSettingsPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    AcquisitionSettingsPresenter(AcquisitionSettingsView& v);

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

    virtual ~AcquisitionSettingsPresenter() {}

private:
    AcquisitionSettingsPresenter();

    AcquisitionSettingsView& view;
};

#endif // ACQUISITIONSETTINGSPRESENTER_HPP
