#ifndef ACQUISITIONSETTINGSVIEW_HPP
#define ACQUISITIONSETTINGSVIEW_HPP

#include <gui_generated/acquisitionsettings_screen/AcquisitionSettingsViewBase.hpp>
#include <gui/acquisitionsettings_screen/AcquisitionSettingsPresenter.hpp>

class AcquisitionSettingsView : public AcquisitionSettingsViewBase
{
public:
    AcquisitionSettingsView();
    virtual ~AcquisitionSettingsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void TransitionEnds();
    virtual void StartAqcuisition_Click();
    virtual void Back_Click();
protected:
};

#endif // ACQUISITIONSETTINGSVIEW_HPP
