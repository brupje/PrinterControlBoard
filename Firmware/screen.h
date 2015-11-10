#ifndef SCREEN_H
#define SCREEN_H


#include "button.h"
#include "Arduino.h"

class printer;
class screen;
class display_control;


enum menuType {menuType_NORMAL,menuType_NUMBER,menuType_FLOAT};


struct menuItem;

typedef void (screen::*initFunction)();
typedef bool (screen::*buttonfunction)();
typedef void (screen::*buttonbarfunction)();
typedef void (screen::*menuactionfunction)(int,menuItem&);

struct menuItem {
  char name[26];
  char postfix;
  menuType type;
  float value;
  float valMin;
  float valMax;
  int id; 
  float initvalue;
  float stepchange[5]= {0,0,0,0,0};
  int extraParam,extraParam2;
  menuactionfunction action = NULL;
  buttonbarfunction hoverbuttonsmenu = NULL;
};

class screen {
  
  protected:
    button m_buttons[7];

    display_control *m_dc;
    unsigned int m_currentMenuItem;

    menuItem * m_items = 0;
    int m_menu_startWithitem;
    int m_numMenuItems=0;
    int m_prevHeating=-1;
    
    bool m_selected;
    initFunction m_function ;
    buttonbarfunction m_buttonbarFunction = &screen::getButtonsBarDummy;
    buttonbarfunction m_defbuttonbarFunction = &screen::getButtonsBarDummy;
    void setButtonFunction(buttonbarfunction f);
  public:
  
    
  
    screen (display_control * dc, initFunction initf=&screen::defaultInit) { 
      m_dc = dc; 
      m_function = initf; 
      for (int i=0; i<7;i++)
        m_buttons[i].setId(i);
    };
    inline button* getButton(int btnNr) { return &m_buttons[btnNr]; };
    inline unsigned int getCurrentMenuItem() { return m_currentMenuItem; };
    
    void setDefaultButtonFunction(buttonbarfunction f) { m_defbuttonbarFunction=f; };
    
    
    
    void clearButtons() {
      for (int i=0; i<7;i++)
        m_buttons[i].clear();
    };
    
    //void setMenuItems(menuItem * items, int numItems) { m_items =items; m_numMenuItems= numItems; };
    
    void setNumMenuItems(int num) {
      if (m_items!=NULL)
        delete m_items;
      m_items = new menuItem[num]; 
      m_numMenuItems = num; 
    };
    
    menuItem&  setMenuItem( int itemnr, char title[MENU_TITLE_LENGTH], menuactionfunction action=NULL) {
      String(title).toCharArray(&m_items[itemnr].name[0],MENU_TITLE_LENGTH);
      m_items[itemnr].type=menuType_NORMAL;
      m_items[itemnr].id=itemnr;
      m_items[itemnr].extraParam = -1;
      m_items[itemnr].hoverbuttonsmenu = NULL;
      m_items[itemnr].action = action;
      return m_items[itemnr];
    }
    
    menuItem& setMenuItem( int itemnr, char title[MENU_TITLE_LENGTH], menuactionfunction action, float defaultValue, float minValue, float maxValue, char postfix = ' ', float stepChange0=0, float stepChange1=0, float stepChange2=0, float stepChange3=0,float stepChange4=0, int extraParam=-1 ) {
      setMenuItem(itemnr,title,action);
      m_items[itemnr].type=menuType_NUMBER;
      m_items[itemnr].value=defaultValue;
      m_items[itemnr].valMin=minValue;
      m_items[itemnr].valMax=maxValue;
      m_items[itemnr].postfix=postfix;
      m_items[itemnr].stepchange[0] = stepChange0;
      m_items[itemnr].stepchange[1] = stepChange1;
      m_items[itemnr].stepchange[2] = stepChange2;
      m_items[itemnr].stepchange[3] = stepChange3;
      m_items[itemnr].stepchange[4] = stepChange4;
      
      m_items[itemnr].extraParam = extraParam;
      
      return m_items[itemnr];
    }
    
    menuItem& setMenuItem( int itemnr, char title[MENU_TITLE_LENGTH], menuactionfunction action, buttonbarfunction hoverbuttonsmenu, float defaultValue, float minValue, float maxValue, char postfix = ' ', float stepChange0=0, float stepChange1=0, float stepChange2=0, float stepChange3=0, float stepChange4=0, int extraParam=-1, bool isFloat=false ) {
      setMenuItem(itemnr,title,action,defaultValue,minValue,maxValue,postfix,stepChange0,stepChange1,stepChange2,stepChange3,stepChange4,extraParam);
      m_items[itemnr].hoverbuttonsmenu=hoverbuttonsmenu;
      
      if (isFloat)
        m_items[itemnr].type=menuType_FLOAT;
        
      return m_items[itemnr];    
    }
    
    void init();
    
    void defaultInit() {
      
    }
    
    void moveRotary(float delta);
    
    bool btnSelectMenuItem(button &btn);
    bool btnCancelSelectMenuItem(button &btn);
    
    bool btnScanPrinters(button &btn);
    bool btnSelectPrinter(button &btn);
    
    bool btnChangeValue(button &btn);
    
    bool getButtonsDummy(){};
    void getButtonsBarDummy(){}
    void getButtonsDashboard();   
    void getButtonsMenu();
    
    void buildPrinterList();
    
    /* move menu */
    void buildMoveMenu();
    void menuHomeAll(int value,menuItem& item);
    void getButtonsHoverAxis();
    void getButtonsHoverAxisZ();
    void getButtonsHoverMoveExtruder();
    bool btnHomeAxis(button &btn);
    bool btnMove(button &btn);
    bool btnMoveRelative(button &btn);
    void btnExtrude(int value,menuItem& item);
    void btnSetAxisPos(int value,menuItem& item);
    
    
    /* control printer menu */
    void btnSetExtruderTemp(int value,menuItem& item);
    void btnSetHBTemp(int value,menuItem& item);
    void btnSetSpeedOverride(int value,menuItem& item);
    void btnSetExtruderSpeedOverride(int value,menuItem& item);
    
    void buildCtrlMenu();
    void getButtonsHoverExtruder();
    void getButtonsHoverSpeed();
    
    bool btnSetPrintingSpeed(button &btn);
    bool btnSetExtruderTempOff(button &btn);
    bool btnSetExtruderTempPLA(button &btn);
    bool btnSetExtruderTempABS(button &btn);
    bool btnExtruderLoad(button &btn);
    bool btnExtruderUnload(button &btn);
    
    /* SD card functions */
    bool btnInitCard(button &btn);
    bool btnReleaseCard(button &btn);
    bool btnDeleteFileCard(button &btn);
    bool btnStartPrint(button &btn);
    bool btnPausePrint(button &btn);
    bool btnStopPrint(button &btn);
    bool btnResumePrint(button &btn);
    void buildSDFileList();
};

#endif SCREEN_H
