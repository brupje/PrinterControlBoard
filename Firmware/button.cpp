#include "button.h"
#include "display_control.h"
#include "screen.h"
#include "controlboard.h"

void button::KeyDown(display_control * dc){
  m_down = true;
  dc->invalidateScreen( DASHBOARDSTATE_REDRAW_BUTTONS);
}

void button::KeyUp(display_control * dc){
  
  bool buttonRes = true;
  m_down = false;
  dc->invalidateScreen( DASHBOARDSTATE_REDRAW_BUTTONS);
  
  
  
  if (m_actionAssigned) {
    bool (screen::*executeBtnAction)(button&) = m_function;  
    buttonRes = (dc->getCurrentScreen()->*executeBtnAction)(*this);

    
  }
  if ((m_screenid!=-1) && (buttonRes)) {
    dc->setCurrentScreen(m_screenid); 
    //dc->invalidateScreen();
  }
}

void button::KeyPress(display_control * dc){

}

void button::clear() {
  this->m_screenid = -1;
  this->title[0]=0;
  this->m_function = NULL;
  this->m_actionAssigned = false;
}

  void button::set(char title[BUTTON_TITLE_LENGTH]) { 
    clear();    
      
   
      int len = strlen(title)+1; 
      memcpy(this->title, title, len > BUTTON_TITLE_LENGTH-1 ? BUTTON_TITLE_LENGTH-1 : len); 
      this->title[len > BUTTON_TITLE_LENGTH-1 ? BUTTON_TITLE_LENGTH-1 : len] = 0; 
    controlboard::getInstance()->getDc()->invalidateScreen( DASHBOARDSTATE_REDRAW_BUTTONS);
    
  };
  
  void button::set(char title[BUTTON_TITLE_LENGTH], int screenid) {
    this->set(title);
    this->m_screenid = screenid;
    
  }
  
  void button::set(char title[BUTTON_TITLE_LENGTH], executeBtnAction action) {
    this->set(title);
    this->m_function = action;
    this->m_actionAssigned = true;

  }
  
   void button::set(char title[BUTTON_TITLE_LENGTH], executeBtnAction action, int screenid) {
    this->set(title);
    this->m_function = action;
    this->m_actionAssigned = true;
    this->m_screenid = screenid;

  }
  
  void button::set(char title[BUTTON_TITLE_LENGTH], executeBtnAction action, float value,int screenid) {
    this->set(title);
    this->m_function = action;
    this->m_actionAssigned = true;
    this->m_screenid = screenid;
    this->m_value=value;
  }
