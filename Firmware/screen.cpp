#include "screen.h" 
#include "controlboard.h" 
#include DISPLAY_CONTROL_CLASS_HEADER

void screen::init() { 
  m_currentMenuItem=0;
  m_menu_startWithitem=0;
  m_prevHeating = -1;
  m_selected=false;
  
  setButtonFunction(m_defbuttonbarFunction);
  
  void (screen::*initFunction)() = m_function;  
  (this->*initFunction)();
  controlboard::getInstance()->getDc()->invalidateScreen();
  
  if (m_numMenuItems>0){
    if (m_items[m_currentMenuItem].hoverbuttonsmenu!=NULL)
      setButtonFunction(m_items[m_currentMenuItem].hoverbuttonsmenu);
    else
      setButtonFunction(m_defbuttonbarFunction);
    m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_ARROW);  
  }
  
};

void screen::setButtonFunction(buttonbarfunction f) { 
  if(m_buttonbarFunction == f) 
    return;
    
  clearButtons(); 
  m_buttonbarFunction = f; 
  m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_BUTTONS);  
};
    

void screen::moveRotary(float delta) {

  if (m_selected) {
    if ((m_items[m_currentMenuItem].type==menuType_NUMBER) || (m_items[m_currentMenuItem].type==menuType_FLOAT)) {
      if (m_items[m_currentMenuItem].value+delta <m_items[m_currentMenuItem].valMin )
        m_items[m_currentMenuItem].value = m_items[m_currentMenuItem].valMin;
      else if (m_items[m_currentMenuItem].value+ delta >= m_items[m_currentMenuItem].valMax )  
        m_items[m_currentMenuItem].value = m_items[m_currentMenuItem].valMax;
      else 
        m_items[m_currentMenuItem].value += delta;
      
      
      m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
    }
  }
  else {
  
    if (m_currentMenuItem+delta <0 )
      m_currentMenuItem = m_numMenuItems-1;
    else if (m_currentMenuItem+ delta >= m_numMenuItems )  
      m_currentMenuItem =0;
    else 
      m_currentMenuItem+= delta;
      
    if (m_numMenuItems>0){
      if (m_items[m_currentMenuItem].hoverbuttonsmenu!=NULL)
        setButtonFunction(m_items[m_currentMenuItem].hoverbuttonsmenu);
      else
        setButtonFunction(m_defbuttonbarFunction);
      m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_ARROW);  
    }
    else {
      
      controlboard::getInstance()->markPrintingSpeedUpdate( delta);
    }
    if (m_currentMenuItem < m_menu_startWithitem) {
      m_menu_startWithitem = m_currentMenuItem;
      m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
    }
    else if (m_currentMenuItem >= m_menu_startWithitem + MAX_CONTENT_LINES) { 
      m_menu_startWithitem = m_currentMenuItem-MAX_CONTENT_LINES+1;
      m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
    }
  }
};

bool screen::btnSelectMenuItem(button &btn) {
  if ((m_items[m_currentMenuItem].type==menuType_NUMBER) || (m_items[m_currentMenuItem].type==menuType_FLOAT))  {
    if (m_selected) {
      
      if (m_items[m_currentMenuItem].action!=NULL ) {
    
        void (screen::*menuactionfunction)(int,menuItem&) = m_items[m_currentMenuItem].action;  
        (m_dc->getCurrentScreen()->*menuactionfunction)(m_items[m_currentMenuItem].value,m_items[m_currentMenuItem]);
      }
      m_selected =  false;
      if (m_items[m_currentMenuItem].hoverbuttonsmenu!=NULL)
        setButtonFunction(m_items[m_currentMenuItem].hoverbuttonsmenu);
      else
        setButtonFunction(m_defbuttonbarFunction);
      m_dc->invalidateScreen();
    }
    else {
      m_items[m_currentMenuItem].initvalue = m_items[m_currentMenuItem].value;
      m_selected =  true;
      setButtonFunction(&screen::getButtonsMenu);
      m_dc->invalidateScreen();
    }
  }
  else if (m_items[m_currentMenuItem].type==menuType_NORMAL) {
    void (screen::*menuactionfunction)(int,menuItem&) = m_items[m_currentMenuItem].action;  
    (m_dc->getCurrentScreen()->*menuactionfunction)(m_items[m_currentMenuItem].value,m_items[m_currentMenuItem]);
  }
}


bool screen::btnCancelSelectMenuItem(button &btn) {
  if (m_selected) {
    m_items[m_currentMenuItem].value = m_items[m_currentMenuItem].initvalue;
    m_selected =  false;
    m_dc->invalidateScreen();
    return false;  
  }
  
  return true;
  
}

bool screen::btnScanPrinters(button &btn) {
  controlboard::getInstance()->scanPrinters();
  controlboard::getInstance()->getDc()->invalidateScreen( DASHBOARDSTATE_REDRAW_CONTENT);
  buildPrinterList();
  return true;
}

bool screen::btnSelectPrinter(button &btn) {
  controlboard::getInstance()->setCurrentPrinter(m_currentMenuItem);
  controlboard::getInstance()->getDc()->setCurrentScreen(btn.getScreen());
  return true;

}

void screen::getButtonsMenu() {
  
  clearButtons();
  
  if (m_selected) {
    char title[BUTTON_TITLE_LENGTH];
    String s;
    for (int i = 0; i<5; i++) {
      if (m_items[m_currentMenuItem].stepchange[i]!=0) {
        if (m_items[m_currentMenuItem].stepchange[i]>0) 
          s="+";
        else 
          s="";
        if ((m_items[m_currentMenuItem].stepchange[i]<1) &&(m_items[m_currentMenuItem].stepchange[i]>-1) )
          s.concat(m_items[m_currentMenuItem].stepchange[i]);
        else
          s.concat((int) m_items[m_currentMenuItem].stepchange[i]);
        s.toCharArray(title,BUTTON_TITLE_LENGTH);
        getButton(i)->set(title,&screen::btnChangeValue);
      }
    }
  }
  

  getButton(5)->set("",&screen::btnCancelSelectMenuItem,0); //back to status
  getButton(6)->set("",&screen::btnSelectMenuItem); 
}

void screen::getButtonsDashboard() {
  printerStatus * s = controlboard::getInstance()->getCurrentPrinter()->getStatus();
  
  if ((s->flags & FLAG_SD_FILE_OPEN)==FLAG_SD_FILE_OPEN) {
    if ((s->flags & FLAG_SD_IS_PRINTING)==FLAG_SD_IS_PRINTING) {
      getButton(0)->set("Pause",&screen::btnPausePrint);
      getButton(1)->set("");
    }
    else if ((s->flags & FLAG_SD_IS_DONE)==FLAG_SD_IS_DONE ){
      getButton(0)->set("Start", -1);
      getButton(1)->set("");
    }
    else {
      getButton(0)->set("Resume",&screen::btnResumePrint);
      getButton(1)->set("Stop",&screen::btnStopPrint);
    }
  }
  else {
    getButton(0)->set("Start", 3);
    getButton(1)->set("");
  }
  getButton(2)->set("Move",5);
  getButton(3)->set("Prntr",1);
  getButton(4)->set("Ctrl",4);
  getButton(6)->set("");
}

bool screen::btnChangeValue(button &btn) {
  moveRotary(m_items[m_currentMenuItem].stepchange[btn.getId()]);
}


void screen::buildPrinterList() { 
  if (m_items!=0)
    delete m_items;
  m_items = new menuItem[controlboard::getInstance()->getNumPrinters()];
  m_numMenuItems = controlboard::getInstance()->getNumPrinters();
    
  for (int i=0; i <controlboard::getInstance()->getNumPrinters(); i++) {
     controlboard::getInstance()->getPrinter(i)->getName(m_items[i].name,26);
  
  }
  
}

/* move */


void screen::menuHomeAll(int value,menuItem& item) {
  controlboard::getInstance()->getCurrentPrinter()->homeAxis(CB_AXIS_ALL);
  
}

bool screen::btnMove(button &btn){
  controlboard::getInstance()->getCurrentPrinter()->moveAxis(m_items[m_currentMenuItem].extraParam,btn.getValue(),false);
  controlboard::getInstance()->getCurrentPrinter()->updateStatus();
  m_items[m_currentMenuItem].value = controlboard::getInstance()->getCurrentPrinter()->getAxisPos(m_items[m_currentMenuItem].extraParam);
  m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
}

bool screen::btnMoveRelative(button &btn){
  controlboard::getInstance()->getCurrentPrinter()->moveAxis(m_items[m_currentMenuItem].extraParam,btn.getValue(),true);
  controlboard::getInstance()->getCurrentPrinter()->updateStatus();
  m_items[m_currentMenuItem].value = controlboard::getInstance()->getCurrentPrinter()->getAxisPos(m_items[m_currentMenuItem].extraParam);
  m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
}

void screen::btnExtrude(int value,menuItem& item){
  controlboard::getInstance()->getCurrentPrinter()->extrude(m_items[m_currentMenuItem].extraParam,m_items[m_currentMenuItem].value);
  m_items[m_currentMenuItem].value = 0;
  m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
}

void screen::btnSetAxisPos(int value,menuItem& item){
  controlboard::getInstance()->getCurrentPrinter()->moveAxis(m_items[m_currentMenuItem].extraParam,m_items[m_currentMenuItem].value,false);
}


void screen::getButtonsHoverAxisZ() {
    getButton(0)->set("Home",&screen::btnHomeAxis);
    getButton(1)->set("-0.1",&screen::btnMoveRelative,-0.1,-1);
    getButton(2)->set("+0.1",&screen::btnMoveRelative,+0.1,-1);
    getButton(3)->set("-1",&screen::btnMoveRelative,-1,-1);
    getButton(4)->set("+1",&screen::btnMoveRelative,+1,-1); 
    
    getButton(5)->set("",&screen::btnCancelSelectMenuItem,0); //back to status
    getButton(6)->set("",&screen::btnSelectMenuItem); 
}

void screen::getButtonsHoverAxis() {
  getButton(0)->set("Home",&screen::btnHomeAxis);
  

    getButton(1)->set("-10",&screen::btnMoveRelative,-10,-1);
    getButton(2)->set("+10",&screen::btnMoveRelative,+10,-1);
    getButton(3)->set("-100",&screen::btnMoveRelative,-100,-1);
    getButton(4)->set("+100",&screen::btnMoveRelative,+100,-1);
  
    getButton(5)->set("",&screen::btnCancelSelectMenuItem,0); //back to status
    getButton(6)->set("",&screen::btnSelectMenuItem); 
  

}

void screen::getButtonsHoverExtruder() {
  getButton(0)->set("Off",&screen::btnSetExtruderTempOff);
  getButton(1)->set("Heat PLA",&screen::btnSetExtruderTempPLA);
  getButton(2)->set("Heat ABS",&screen::btnSetExtruderTempABS);
  getButton(3)->set("Load",&screen::btnExtruderLoad);
  getButton(4)->set("Unload",&screen::btnExtruderUnload);
  
  getButton(5)->set("",&screen::btnCancelSelectMenuItem,0); //back to status
  getButton(6)->set("",&screen::btnSelectMenuItem); 
}

void screen::getButtonsHoverSpeed() {
  getButton(0)->set("100%",&screen::btnSetPrintingSpeed,100,-1);
  getButton(1)->set("50%",&screen::btnSetPrintingSpeed,50,-1);
  getButton(2)->set("150%",&screen::btnSetPrintingSpeed,150,-1);
  getButton(3)->set("200%",&screen::btnSetPrintingSpeed,200,-1);
  
  getButton(5)->set("",&screen::btnCancelSelectMenuItem,0); //back to status
  getButton(6)->set("",&screen::btnSelectMenuItem); 
}

bool screen::btnHomeAxis(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->homeAxis(m_items[m_currentMenuItem].extraParam);
}

void screen::buildMoveMenu() {
  printer *p = controlboard::getInstance()->getCurrentPrinter();
  
  int numExtruders = p->getNumExtruders();
  setNumMenuItems(4+numExtruders );
  
  setMenuItem(0,"Home all",&screen::menuHomeAll);
  setMenuItem(1,"Move X", &screen::btnSetAxisPos, &screen::getButtonsHoverAxis,controlboard::getInstance()->getCurrentPrinter()->getStatus()->x,-500,500,' ',-1,1,-10,10,0,CB_AXIS_X,true);
  setMenuItem(2,"Move Y", &screen::btnSetAxisPos, &screen::getButtonsHoverAxis,controlboard::getInstance()->getCurrentPrinter()->getStatus()->y,-500,500,' ',-1,1,-10,10,0,CB_AXIS_Y,true);
  setMenuItem(3,"Move Z", &screen::btnSetAxisPos, &screen::getButtonsHoverAxisZ,controlboard::getInstance()->getCurrentPrinter()->getStatus()->z,-500,500,' ',-0.1,0.1,-1,1,0,CB_AXIS_Z,true);
  
  char line[26];
  for (int i = 0; i < numExtruders; i++)  {
    String s = "Extrude extruder ";
    s.concat(i);
    s.toCharArray(line,26);
    setMenuItem(4+i,line,&screen::btnExtrude, &screen::getButtonsHoverExtruder,0,-1000,1000,' ',-5,+5,-10,+10,0,i,false).extraParam2=1;
  }
  
}

/* control */
void screen::btnSetExtruderTemp(int value,menuItem& item){
  controlboard::getInstance()->getCurrentPrinter()->setExtruderTemp(item.extraParam, value);
}

void screen::btnSetHBTemp(int value,menuItem& item){
  controlboard::getInstance()->getCurrentPrinter()->setHBTemp(value);
}

void screen::btnSetSpeedOverride(int value,menuItem& item){
  controlboard::getInstance()->getCurrentPrinter()->setPrintingSpeed(value);
}

void screen::btnSetExtruderSpeedOverride(int value,menuItem& item){
  controlboard::getInstance()->getCurrentPrinter()->setExtruderSpeed(value);
}

void screen::getButtonsHoverMoveExtruder(){

  getButton(0)->set("Off",&screen::btnSetExtruderTempOff);
  getButton(1)->set("Heat PLA",&screen::btnSetExtruderTempPLA);
  getButton(2)->set("Heat ABS",&screen::btnSetExtruderTempABS);
  
  getButton(5)->set("",&screen::btnCancelSelectMenuItem,0); //back to status
  getButton(6)->set("",&screen::btnSelectMenuItem); 
}

bool screen::btnSetPrintingSpeed(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->setPrintingSpeed(btn.getValue());
  m_items[m_currentMenuItem].value = btn.getValue();
  m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
}

bool screen::btnSetExtruderTempOff(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->setExtruderTemp(m_items[m_currentMenuItem].extraParam, 0);
  m_items[m_currentMenuItem].value = 0;
  m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
}

bool screen::btnSetExtruderTempPLA(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->setExtruderTemp(m_items[m_currentMenuItem].extraParam, -1);
  if (m_items[m_currentMenuItem].extraParam2==0){
    controlboard::getInstance()->getCurrentPrinter()->updateStatus();
    m_items[m_currentMenuItem].value = controlboard::getInstance()->getCurrentPrinter()->getExtruderSetTemp(m_items[m_currentMenuItem].extraParam);
    m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
  }
}

bool screen::btnSetExtruderTempABS(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->setExtruderTemp(m_items[m_currentMenuItem].extraParam, -2);
  if (m_items[m_currentMenuItem].extraParam2==0){
    controlboard::getInstance()->getCurrentPrinter()->updateStatus();
    m_items[m_currentMenuItem].value = controlboard::getInstance()->getCurrentPrinter()->getExtruderSetTemp(m_items[m_currentMenuItem].extraParam);
    m_dc->invalidateScreen(DASHBOARDSTATE_REDRAW_CONTENT);
  }
}

bool screen::btnExtruderLoad(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->loadFilament(m_items[m_currentMenuItem].extraParam);

}

bool screen::btnExtruderUnload(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->unloadFilament(m_items[m_currentMenuItem].extraParam);

}


void screen::buildCtrlMenu() {

  
  printer *p = controlboard::getInstance()->getCurrentPrinter();
  
  p->updateSpeedSettings();
  
  int numExtruders = p->getNumExtruders();
  bool hasHB = p->hasHeatedBed();
  
  setNumMenuItems(2+numExtruders+ (hasHB ? 1 :0) );
  setMenuItem(0,"Set printing speed",&screen::btnSetSpeedOverride, &screen::getButtonsHoverSpeed, p->getPrintingSpeed(),0,1000,'%',-5,+5,-10,+10);
  setMenuItem(1,"Set extruder speed",&screen::btnSetExtruderSpeedOverride, p->getExtruderSpeed(),0,1000,'%',-5,+5,-10,+10);
  
  char line[26];
  for (int i = 0; i < numExtruders; i++)  {
    String s = "Extruder ";
    s.concat(i);
    s.concat("  temp");
    s.toCharArray(line,26);
    setMenuItem(2+i,line,&screen::btnSetExtruderTemp, &screen::getButtonsHoverMoveExtruder,p->getExtruderSetTemp(i) ,0,500,' ',-5,+5,-10,+10,0,i).extraParam2=0;
  }
  
  if (hasHB)
    setMenuItem(2+numExtruders,"Heated bed temp",&screen::btnSetHBTemp, &screen::getButtonsHoverExtruder,p->getHBSetTemp(),0,500,' ',-5,+5,-10,+10,0,255);
}

/* SD */

bool screen::btnInitCard(button &btn) {
  buildSDFileList();
  return true;
}

bool screen::btnReleaseCard(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->SDReleaseCard();
  return true;
}

bool screen::btnDeleteFileCard(button &btn) {
  String filename;filename = String(m_items[m_currentMenuItem].name);
  filename.toLowerCase();
  controlboard::getInstance()->getCurrentPrinter()->SDDeleteFile( filename.c_str());  
  
  buildSDFileList();
  return true;
}

bool screen::btnStartPrint(button &btn) {
  String filename;filename = String(m_items[m_currentMenuItem].name);
  filename.toLowerCase();
  controlboard::getInstance()->getCurrentPrinter()->SDStartPrint( filename.c_str());  
  return true;
}

bool screen::btnPausePrint(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->pausePrint();
  return true;
}

bool screen::btnStopPrint(button &btn) {
  controlboard::getInstance()->getCurrentPrinter()->stopPrint();
  return true;
}


bool screen::btnResumePrint(button &btn){
  controlboard::getInstance()->getCurrentPrinter()->resumePrint();
  return true;
}

void screen::buildSDFileList() {

  byte numFiles;
  if (controlboard::getInstance()->getCurrentPrinter()->SDGetNumFiles(numFiles)==GETDATA_RET_OK) {  
  
    setNumMenuItems(numFiles);
  
    
    for (int i =0; i<numFiles; i++) {
      
      controlboard::getInstance()->getCurrentPrinter()->SDGetFilename(i,&m_items[i].name[0],26);  
  
    }
  }
  else
  setNumMenuItems(0);
}
