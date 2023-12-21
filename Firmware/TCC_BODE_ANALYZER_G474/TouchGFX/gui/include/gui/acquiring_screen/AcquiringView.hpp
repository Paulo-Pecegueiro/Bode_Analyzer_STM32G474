#ifndef ACQUIRINGVIEW_HPP
#define ACQUIRINGVIEW_HPP

#include <gui_generated/acquiring_screen/AcquiringViewBase.hpp>
#include <gui/acquiring_screen/AcquiringPresenter.hpp>

class AcquiringView : public AcquiringViewBase
{
public:
    AcquiringView();
    virtual ~AcquiringView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void TransitionEnds();
    virtual void Abort_Click();
    virtual void Ok_Click();
    virtual void ScreenTick();
protected:
};

#endif // ACQUIRINGVIEW_HPP
