
#include "display_control.h"
#include "controlboard.h"
#include SCREEN_CLASS_HEADER

void display_control::init() {
  screenTree[0] = new SCREEN_CLASS(this,&SCREEN_CLASS::drawDashboard);
  
  currentScreen = screenTree[0];
  
  screenTree[0]->setDefaultButtonFunction(&screen::getButtonsDashboard);
  //screenTree[0]->getButton(0)->set("Start", 3);
  

  
  /* Select printer */
  screenTree[1] = new SCREEN_CLASS(this,&SCREEN_CLASS::drawPrinterList,&screen::buildPrinterList);
  screenTree[1]->getButton(0)->set("Scan",&screen::btnScanPrinters);
  screenTree[1]->getButton(5)->set("",0); // bback to status
  screenTree[1]->getButton(6)->set("",&screen::btnSelectPrinter,0); // back to main menu
  
  /* main menu */
  screenTree[2] = new SCREEN_CLASS(this,&SCREEN_CLASS::drawMenu);
  
  screenTree[2]->setNumMenuItems(4);

  

  screenTree[2]->getButton(5)->set("",0); // back to status
  //screenTree[2]->getButton(6)->set("",&screen::btnScanPrinters); // back to main menu
  
  
  /* start print */
  screenTree[3] = new SCREEN_CLASS(this,&SCREEN_CLASS::drawMenu,&screen::buildSDFileList);

  screenTree[3]->getButton(0)->set("Start",&screen::btnStartPrint,0);
  screenTree[3]->getButton(2)->set("Init card",&screen::btnInitCard);
  screenTree[3]->getButton(3)->set("Rls card",&screen::btnReleaseCard,0);
  screenTree[3]->getButton(4)->set("Del file",&screen::btnDeleteFileCard);
  screenTree[3]->getButton(5)->set("",0); //back to status
  screenTree[3]->getButton(6)->set("",&screen::btnStartPrint,0); 
  

  /* control */
  screenTree[4] = new SCREEN_CLASS(this,&SCREEN_CLASS::drawMenu,&screen::buildCtrlMenu);
  screenTree[4]->setDefaultButtonFunction(&screen::getButtonsMenu);
  
  /* move */
  screenTree[5] = new SCREEN_CLASS(this,&SCREEN_CLASS::drawMenu,&screen::buildMoveMenu);
  screenTree[5]->setDefaultButtonFunction(&screen::getButtonsMenu);

  currentScreen->init();
}


display_control::display_control() {
   m_screenDashboardState = DASHBOARDSTATE_REDRAW_ALL;
}

void display_control::handleKeysDown(AdvButton* but) {
  currentScreen->getButton(but->getID())->KeyDown(this);
}

void display_control::handleKeysUp(AdvButton* but){
  currentScreen->getButton(but->getID())->KeyUp(this);
}

void display_control::handleKeysPress(AdvButton* but){
  currentScreen->getButton(but->getID())->KeyPress(this);
}

