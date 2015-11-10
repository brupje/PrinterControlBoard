#ifndef DISPLAY_CONTROL_H
#define DISPLAY_CONTROL_H


#include "printer.h"
#include "AdvButton.h"
#include "defines.h"
#include "configuration.h"
#include SCREEN_CLASS_HEADER

class display_control {
  public:
    void init();
    
    virtual void showConnecting() = 0;
    

    
    display_control();
    void invalidateScreen(unsigned int redrawArea = DASHBOARDSTATE_REDRAW_ALL) {
      m_screenDashboardState = m_screenDashboardState | redrawArea;
    }
    
    void notifyStateChange() {
      m_screenDashboardState = DASHBOARDSTATE_REDRAW_ALL;
      currentScreen->init();
    }
    
    inline unsigned int getState() { return m_screenDashboardState; };    
    void clearState() { m_screenDashboardState= DASHBOARDSTATE_REDRAW_NONE; };
    void handleKeysDown(AdvButton* but);
    void handleKeysUp(AdvButton* but);
    void handleKeysPress(AdvButton* but);
    
    void setCurrentScreen(int id) { 
      currentScreen=screenTree[id];
      currentScreen->init();
      invalidateScreen();
    };
    

    inline screen * getCurrentScreen() { return currentScreen; };
    
    virtual void drawScreen(printer * p ) =0; 

    
 protected:
   display_control& self() { return *this; }
   unsigned int m_screenDashboardState;
   
   SCREEN_CLASS * currentScreen;
   
   SCREEN_CLASS * screenTree[NUM_SCREENS];
};

#endif //DISPLAY_CONTROL_H
