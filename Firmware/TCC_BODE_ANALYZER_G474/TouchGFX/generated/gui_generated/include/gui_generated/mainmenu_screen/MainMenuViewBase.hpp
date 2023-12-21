/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef MAINMENUVIEWBASE_HPP
#define MAINMENUVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/mainmenu_screen/MainMenuPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/ButtonWithLabel.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <gui/containers/FormatEnd.hpp>

class MainMenuViewBase : public touchgfx::View<MainMenuPresenter>
{
public:
    MainMenuViewBase();
    virtual ~MainMenuViewBase();
    virtual void setupScreen();
    virtual void afterTransition();

    /*
     * Custom Actions
     */
    virtual void GoToBodeAnalyzer();
    virtual void GoToAbout();
    virtual void GoToGraphs();

    /*
     * Virtual Action Handlers
     */
    virtual void TransitionEnds()
    {
        // Override and implement this function in MainMenu
    }
    virtual void BodeAnalyzer_Click()
    {
        // Override and implement this function in MainMenu
    }
    virtual void About_Click()
    {
        // Override and implement this function in MainMenu
    }
    virtual void ShowGraphs_Click()
    {
        // Override and implement this function in MainMenu
    }
    virtual void FormatFS_Click()
    {
        // Override and implement this function in MainMenu
    }

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::Box Background;
    touchgfx::ButtonWithLabel btnAbout;
    touchgfx::ButtonWithLabel btnFormatFS;
    touchgfx::ButtonWithLabel btnShowGraphs;
    touchgfx::ButtonWithLabel btnBodeAnalyzer;
    touchgfx::Box boxTitleBack;
    touchgfx::TextArea lblTitle;
    touchgfx::Image IFSP_LOGO;
    touchgfx::Image MIYOSHI_LOGO;
    touchgfx::Image ST_LOGO;
    FormatEnd formatEnd1;

private:

    /*
     * Callback Declarations
     */
    touchgfx::Callback<MainMenuViewBase, const touchgfx::AbstractButton&> buttonCallback;
    touchgfx::Callback<MainMenuViewBase> formatEnd1CloseContainerCallback;

    /*
     * Callback Handler Declarations
     */
    void buttonCallbackHandler(const touchgfx::AbstractButton& src);
    void formatEnd1CloseContainerCallbackHandler();

};

#endif // MAINMENUVIEWBASE_HPP
