
#include <openGLCD_Buildinfo.h>
#include <openGLCD.h>
#include <openGLCD_Config.h>

#include "printer.h"

#include "fonts/System5x7.h"         // proportional font
#include "fonts/SystemFont5x7.h"   // system font
#include "fonts/Iain5x7.h"
#include "temp.h"


#include "glcd_control.h"
#include "configuration.h" 
#include "blocksm.h"


void glcd_control::init() {
  
  display_control::init();
  GLCD.Init(NON_INVERTED);  
  drawInitScreen();
   m_prev_state=0; 
}

void glcd_control::drawInitScreen() {
  GLCD.ClearScreen();  
  GLCD.DrawBitmap(blocksm, 7,7, BLACK); //draw logo
  GLCD.SelectFont(System5x7);
  
  GLCD.CursorToXY(DISPLAY_WIDTH -  GLCD.StringWidth(CONTROLBOARD_SW_VERSION),DISPLAY_HEIGHT-7);
  GLCD.print(CONTROLBOARD_SW_VERSION);
  

  int textBoxLeft = DISPLAY_WIDTH -  GLCD.StringWidth(CONTROLBOARD_SW_TITLE);
  
  GLCD.CursorTo(textBoxLeft,0);
  GLCD.print(CONTROLBOARD_SW_TITLE);
  
  GLCD.CursorTo(textBoxLeft,1);
  GLCD.print(CONTROLBOARD_SW_LINE1);
  
  gText connectingArea = gText(64, 30,10,1,System5x7);
  connectingArea.ClearArea();

}
void glcd_control::showConnecting() {
  gText connectingArea = gText(80, 30,GLCD.Right,40);
  connectingArea.SelectFont(System5x7);
  
  if ((millis() % 2000 < 1000) && (m_prev_state==0)) {
    connectingArea.DrawString( STRING_CONNECTING,1,1 ); 
    m_prev_state =1;
  }
  else if ((millis() % 2000 >= 1000) && (m_prev_state==1)) {
    connectingArea.ClearArea();
    m_prev_state =0;
  }
}










