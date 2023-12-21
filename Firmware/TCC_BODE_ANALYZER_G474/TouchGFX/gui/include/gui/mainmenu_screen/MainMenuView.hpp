#ifndef MAINMENUVIEW_HPP
#define MAINMENUVIEW_HPP

#include <gui_generated/mainmenu_screen/MainMenuViewBase.hpp>
#include <gui/mainmenu_screen/MainMenuPresenter.hpp>

class MainMenuView : public MainMenuViewBase
{
public:
    MainMenuView();
    virtual ~MainMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void TransitionEnds();
    virtual void BodeAnalyzer_Click();
    virtual void About_Click();
    virtual void ShowGraphs_Click();
    virtual void FormatFS_Click();
protected:
};

#endif // MAINMENUVIEW_HPP
