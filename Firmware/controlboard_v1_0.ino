
#include <Wire.h>
#include <wirecom.h>

#include "configuration.h"
#include "pins.h"
#include "printer.h"


#include "AdvButton.h"
#include <ButtonManager.h>

#include "controlboard.h"

#if DISPLAY_CONTROLLER==1
  #include <openGLCD.h>
  #include "glcd_control.h"
#endif










void setup(){
  pinMode(PIN_DBG,OUTPUT);
  digitalWrite(PIN_DBG,HIGH);
  delay(1);
  digitalWrite(PIN_DBG,LOW);
  /* screen init */
  controlboard::getInstance()->init();
  delay(CFG_STARTUP_DELAY);
  Wire.begin(); 
  
  
}





void  loop(){   
  
  
  ButtonManager::instance()->checkButtons();
  
  controlboard::getInstance()->handleState();
  
  
}
