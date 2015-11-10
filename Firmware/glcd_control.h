#ifndef GLCD_CONTROL_H
#define GLCD_CONTROL_H

#include "display_control.h"

class glcd_control : public virtual display_control {
  public:
    void init();
    void showConnecting();

    void drawButtons();
    
  
   void drawScreen(printer * p ) { currentScreen->draw(*this,p); };    
  
   void drawInitScreen();
  private:
    int m_prev_state;
};

#endif //GLCD_CONTROL_H
