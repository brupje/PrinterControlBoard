#include "glcd_screen.h" 
#include "defines.h"
#include <openGLCD.h>

#include "wirecom.h"
#include "printer.h"
#include "glcd_control.h"
#include "controlboard.h"


void glcd_screen::draw(glcd_control& dc,printer * p)
{ 
  
  int screenDashboardState = dc.getState();
  if (screenDashboardState==DASHBOARDSTATE_REDRAW_NONE){
    return;
  }
  
  if ((screenDashboardState & DASHBOARDSTATE_REDRAW_CLEAR) == DASHBOARDSTATE_REDRAW_CLEAR) {
    GLCD.ClearScreen();
  }
  
  
  if ((screenDashboardState & DASHBOARDSTATE_REDRAW_BUTTONS) == DASHBOARDSTATE_REDRAW_BUTTONS) {    
    if (m_buttonbarFunction!=NULL) {
      (this->*m_buttonbarFunction)();
    }  
  }
  
  

  

  
  void (glcd_screen::*drawfunction)(glcd_control&,printer*) = m_function;  
  (this->*drawfunction)(dc,p);
  
  if ((screenDashboardState & DASHBOARDSTATE_REDRAW_BUTTONS) == DASHBOARDSTATE_REDRAW_BUTTONS) {    
    this->drawButtons();
  }
  
  
  dc.clearState();
}

void glcd_screen::drawDashboard(glcd_control &dc,printer * p){  
    printerStatus * s = p->getStatus();

    int screenDashboardState = dc.getState();
    int numExtruders = p->getNumExtruders();
    gText textarea;
    GLCD.SelectFont(System5x7);
    textarea.DefineArea(0, 10, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-BUTTON_HEIGHT-3);  
    textarea.SelectFont(System5x7); 

  
  if ((screenDashboardState & DASHBOARDSTATE_REDRAW_CLEAR) == DASHBOARDSTATE_REDRAW_CLEAR) {
   
    
    drawTitleBar(p->getName());
    
    GLCD.SelectFont(System5x7);
    
    GLCD.SetFontColor(PIXEL_ON);
    
    
    

    for (int e = 0; e< numExtruders; e++) {
      textarea.CursorTo(0,e);
      textarea.print("E");
      textarea.print(e);
      GLCD.DrawBitmap(temp,12,8*(e+1)+2);
    }
  
    
    textarea.CursorTo(0,numExtruders);
    textarea.print("HB");
    GLCD.DrawBitmap(temp,12,8*(numExtruders+1)+2);
    
    textarea.CursorTo(13,0);
    textarea.print("SPD:");
    textarea.CursorTo(13,1);
    textarea.print("X:");
    textarea.CursorTo(13,2);
    textarea.print("Y:");
    textarea.CursorTo(13,3);
    textarea.print("Z:");
    
    if ((s->flags & FLAG_SD_FILE_OPEN)==FLAG_SD_FILE_OPEN) {
      textarea.CursorTo(13,4);
      textarea.print("SD:");
    }
    else {
      textarea.CursorTo(13,4);
      textarea.print("   ");
    }
  }
  
  if ((screenDashboardState & DASHBOARDSTATE_REDRAW_TEMP) == DASHBOARDSTATE_REDRAW_TEMP) {
    GLCD.FillRect(18,8,53,42,PIXEL_OFF);   
    textarea.SelectFont(System5x7);
    
    for (int e = 0; e< numExtruders; e++) {
      textarea.CursorTo(4,e);
      textarea.print(s->extTempCur[e]);
      textarea.print("/");
      textarea.print(s->extTempSet[e]);
    }
    
    textarea.CursorTo(4,numExtruders);
    textarea.print(s->hbTempCur);
    textarea.print("/");
    textarea.print(s->hbTempSet);
    
    GLCD.FillRect(DISPLAY_WIDTH-80,8,80,42,PIXEL_OFF);
    
    char value[10];
    
    textarea.CursorTo(18,0);
    textarea.print( p->getSpeedSettings()->printing);
    textarea.print("%");
    dtostrf(s->x,1,1,value);
    textarea.DrawString( value,gTextfmt_col(18 ),gTextfmt_row(1));
    dtostrf(s->y,1,1,value);
    textarea.DrawString( value,gTextfmt_col(18),gTextfmt_row(2));
    dtostrf(s->z,1,1,value);
    textarea.DrawString( value,gTextfmt_col(18),gTextfmt_row(3));

    if ((s->flags & FLAG_SD_FILE_OPEN)==FLAG_SD_FILE_OPEN) {
      
      textarea.CursorTo(18,4);
      if ((s->flags & FLAG_SD_IS_PRINTING)==FLAG_SD_IS_PRINTING) {
        textarea.CursorTo(18,4);
        SDprintStatus * ps = p->getSDStatus();
        dtostrf(ps->perccomplete,1,1,value);
        textarea.print(value);
        textarea.print("%");
      }
      else if ((s->flags & FLAG_SD_IS_DONE)==FLAG_SD_IS_DONE ){
        textarea.print("DONE"); 
      }
      else {
        textarea.print("PAUSED"); 
      }
      
    }
  }
  

}


void glcd_screen::drawPrinterList(glcd_control &dc,printer * p) {
  int screenDashboardState = dc.getState();
  
  if ((screenDashboardState & DASHBOARDSTATE_REDRAW_CLEAR) == DASHBOARDSTATE_REDRAW_CLEAR) {
    drawTitleBar("Select printer");
  }
  

    
  if (((screenDashboardState & DASHBOARDSTATE_REDRAW_CONTENT) == DASHBOARDSTATE_REDRAW_CONTENT) || ((screenDashboardState & DASHBOARDSTATE_REDRAW_ARROW) == DASHBOARDSTATE_REDRAW_ARROW)) 
    drawMenuItems(dc,m_items,m_numMenuItems);
  

  

}

 void glcd_screen::drawMenu(glcd_control &dc,printer * p) {
   int screenDashboardState = dc.getState();
  //if ((screenDashboardState & DASHBOARDSTATE_REDRAW_CLEAR) == DASHBOARDSTATE_REDRAW_CLEAR) {
    drawTitleBar(p->getName());
  //}  
   
  if (((screenDashboardState & DASHBOARDSTATE_REDRAW_CONTENT) == DASHBOARDSTATE_REDRAW_CONTENT) || ((screenDashboardState & DASHBOARDSTATE_REDRAW_ARROW) == DASHBOARDSTATE_REDRAW_ARROW)) {
    drawMenuItems(dc,m_items,m_numMenuItems);
  }
 }

void glcd_screen::drawMenuItems(glcd_control &dc,menuItem items[], unsigned int numitems) {
  int screenDashboardState = dc.getState();
  gText textarea;
  textarea.DefineArea(0, 10, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-BUTTON_HEIGHT-2);  
  
  if ((screenDashboardState & DASHBOARDSTATE_REDRAW_CONTENT) == DASHBOARDSTATE_REDRAW_CONTENT){
    textarea.SetFontColor(  PIXEL_ON);
    textarea.ClearArea();
    screenDashboardState= screenDashboardState | DASHBOARDSTATE_REDRAW_ARROW;
  }
  else if ((screenDashboardState & DASHBOARDSTATE_REDRAW_ARROW) == DASHBOARDSTATE_REDRAW_ARROW) {
    GLCD.FillRect(0, 10, 7, DISPLAY_HEIGHT-BUTTON_HEIGHT-2-10,PIXEL_OFF);   
  }

  textarea.SelectFont(System5x7); 
  

  
  int row=0;
  for (int i=m_menu_startWithitem; i< (m_menu_startWithitem+MAX_CONTENT_LINES < numitems ? m_menu_startWithitem+MAX_CONTENT_LINES:numitems) ; i++) {
    
    if ((getCurrentMenuItem()==i) && m_selected) 
      textarea.SetFontColor(  PIXEL_OFF);
    else
      textarea.SetFontColor(  PIXEL_ON);
     
    if ((screenDashboardState & DASHBOARDSTATE_REDRAW_CONTENT) == DASHBOARDSTATE_REDRAW_CONTENT){
      String str = items[i].name;
      textarea.DrawString( str.substring(0,str.length() >25 ?  25:str.length() ).c_str(),gTextfmt_col(2),gTextfmt_row(row), eraseFULL_LINE);
      
      if (items[i].type==menuType_NUMBER) {
        char value[10];
        String((int) items[i].value).toCharArray(value,10);
        textarea.DrawString( value,gTextfmt_col(31 - strlen(value)),gTextfmt_row(row));
        textarea.DrawString(String(items[i].postfix).c_str(),gTextfmt_col(31 ),gTextfmt_row(row));
      }
      else if (items[i].type==menuType_FLOAT) {
        char value[10];
        
        dtostrf(items[i].value,1,1,value);
        
        
        textarea.DrawString( value,gTextfmt_col(31 - strlen(value)),gTextfmt_row(row));
        textarea.DrawString(String(items[i].postfix).c_str(),gTextfmt_col(31 ),gTextfmt_row(row));
        
      }
    }
    if (getCurrentMenuItem()==i) {
      textarea.CursorTo(0,row);
      textarea.print("> ");
    }
    row++;
  }
}





void glcd_screen::drawTitleBar(char * title) {
  
  GLCD.CursorTo(0,0);
  GLCD.FillRect(0,0,DISPLAY_WIDTH,8,PIXEL_ON); 
  
  GLCD.SelectFont(Iain5x7);
  GLCD.SetFontColor(PIXEL_OFF); 
  
  GLCD.print(title);
  
  if (m_numMenuItems>0) {
    GLCD.CursorTo(28,0);
    GLCD.print(getCurrentMenuItem()+1);
    GLCD.print("/");
    GLCD.print(m_numMenuItems);
  }
}

void glcd_screen::drawButtons() { 
  int pos=0; 
  char title[BUTTON_TITLE_LENGTH];
  
  GLCD.FillRect( 0, DISPLAY_HEIGHT - BUTTON_HEIGHT, DISPLAY_WIDTH,BUTTON_HEIGHT, WHITE); // Clear
  for (int i=0; i< MAX_MENU_BUTTONS; i++ ){
    GLCD.SelectFont(Iain5x7);
    
    
    
    getButton(i)->getTitle(title);
    
    int strWid = GLCD.StringWidth(title);
    
    int btnWidth = ((strWid+4>BUTTON_WIDTH) ? strWid+8 : BUTTON_WIDTH);
    if (i==MAX_MENU_BUTTONS-1)
      btnWidth = DISPLAY_WIDTH-pos;
    
    if (strcmp(title,"") != 0) { 
      
      if ( getButton(i)->isDown() )
        GLCD.FillRect( pos, DISPLAY_HEIGHT - BUTTON_HEIGHT, btnWidth ,BUTTON_HEIGHT,PIXEL_ON); 
      else
        GLCD.DrawRect( pos, DISPLAY_HEIGHT - BUTTON_HEIGHT, btnWidth ,BUTTON_HEIGHT,PIXEL_ON );

      GLCD.CursorToXY(pos+ btnWidth/2 - strWid/2, DISPLAY_HEIGHT - BUTTON_HEIGHT+3);
      GLCD.SetFontColor( getButton(i)->isDown() ? PIXEL_OFF:PIXEL_ON  );
      GLCD.print(title);
    }
    pos+=btnWidth-1;
  
  }
}
