#ifndef BUTTON_H
#define BUTTON_H

#include <string.h>
#include "configuration.h"

class display_control;
class screen;
class button;
class printer;

typedef bool (screen::*executeBtnAction)(button&);

class button {
  protected:
    char title[BUTTON_TITLE_LENGTH] ="";
    bool m_down=false;
    int m_id=-1;
    int m_screenid=-1;
    float m_value=0;
    executeBtnAction m_function;
    bool m_actionAssigned=false;
  public:
    void set(char title[BUTTON_TITLE_LENGTH]);
    
    void set(char title[BUTTON_TITLE_LENGTH], int screenid);
    
    void set(char title[BUTTON_TITLE_LENGTH], executeBtnAction action) ;
    
    void set(char title[BUTTON_TITLE_LENGTH], executeBtnAction action, int screenid);
    void set(char title[BUTTON_TITLE_LENGTH], executeBtnAction action, float value,int screenid);
    
    void getTitle(char *title) { memcpy(title,this->title,BUTTON_TITLE_LENGTH); };
    int getScreen() { return m_screenid;};
    
    void KeyDown(display_control * dc);
    void KeyUp(display_control * dc);
    void KeyPress(display_control * dc);
    void setId(int id) { this->m_id = id; };
    int getId() { return this->m_id; };
    float getValue() { return m_value; };
    void clear();
    
    inline bool isDown() { return m_down; };
};

#endif //BUTTON_H
