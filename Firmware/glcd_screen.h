#ifndef GLCD_SCREEN_H
#define GLCD_SCREEN_H


#include "screen.h" 

class printer;
class glcd_screen;
class glcd_control;

typedef void (glcd_screen::*drawfunction)(glcd_control&,printer*);




class glcd_screen : public   screen {
  
  protected:

    drawfunction m_function;
  public:

    glcd_screen ( display_control * dc, drawfunction f, initFunction initf=&screen::defaultInit ) :screen (dc,initf) { m_function =f; };
    void drawDashboard(glcd_control &dc,printer * p);
    void drawPrinterList(glcd_control &dc,printer * p);
    void drawMenu(glcd_control &dc,printer * p);
    
    void drawMenuItems(glcd_control &dc,menuItem items[], unsigned int numitems);
    void drawButtons();
    
    void draw(glcd_control& dc,printer * p);
   
    void drawTitleBar(char * title);
    
};

#endif GLCD_SCREEN_H
