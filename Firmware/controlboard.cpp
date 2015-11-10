
#include "controlboard.h" 

controlboard * controlboard::s_instance = 0;

void updateRotary() {
  controlboard::getInstance()->updateRotaryPos();
}

void handleKeysDown(AdvButton* but) {
  
  if ((but->getID() == 7) || (but->getID() == 8)) {
    //controlboard::getInstance()->updateRotaryPos();

  }
  else
    controlboard::getInstance()->getDc()->handleKeysDown(but);

  
}

void handleKeysUp(AdvButton* but){
  if ((but->getID() == 7) || (but->getID() == 8)) {
    //controlboard::getInstance()->updateRotaryPos();

  }
  else
    controlboard::getInstance()->getDc()->handleKeysUp(but);


}

void handleKeysPress(AdvButton* but){
  controlboard::getInstance()->getDc()->handleKeysPress(but);

}

int controlboard::getRotaryPos() {
  int enc_a = digitalRead(m_pins_buttons[7]);
  int enc_b = digitalRead(m_pins_buttons[8]);
  
  int newpos;
  
  if ((enc_a==LOW) && (enc_b==LOW)) {
    newpos = 0;
  }
  else if ((enc_a==LOW) && (enc_b==HIGH)) {
    newpos = 1;
    m_ppos=1;
  }
  else if ((enc_a==HIGH) && (enc_b==HIGH)) 
  {
    newpos = 2;
    
  }
  else if ((enc_a==HIGH) && (enc_b==LOW)) {
    
    newpos = 3; 
    m_ppos=3;
  }
   
    
  return newpos;
}


void controlboard::updateRotaryPos() {
  int newpos = getRotaryPos();
  
  
  if (newpos == 2) {
    if (m_ppos==1) {
      getDc()->getCurrentScreen()->moveRotary(-1);
    }
    else if (m_ppos==3) {
      getDc()->getCurrentScreen()->moveRotary(1);
    }
    m_ppos=0;
    
  }
  else   if (newpos == 0) {
    if (m_ppos==1) {
      getDc()->getCurrentScreen()->moveRotary(1);
    }
    else if (m_ppos==3) {
      getDc()->getCurrentScreen()->moveRotary(-1);
    }
    m_ppos=0;
    
  }
  
  m_rotateryPos = newpos;
}

void controlboard::init() {
  
  
  attachInterrupt(digitalPinToInterrupt(m_pins_buttons[7]), updateRotary, CHANGE);
  attachInterrupt(digitalPinToInterrupt(m_pins_buttons[8]), updateRotary, CHANGE);



  for (int i = 0; i< SizeOfArray(m_buttons); i++ ) {
   m_buttons[i] = new AdvButton(m_pins_buttons[i],handleKeysPress,handleKeysDown,handleKeysUp);
   m_buttons[i]->setID(i);
  }
  
  for (int i = 0; i< SizeOfArray(m_pins_leds); i++ ) {
    pinMode(m_pins_leds[i],OUTPUT);
    digitalWrite(m_pins_leds[i],LOW);
  }
  
  m_dc.init();
  
  m_rotateryPos = getRotaryPos();
};

int controlboard::scanPrinters() {
  

  int r;
  m_currentPrinter = 0;
  m_numPrinters = 0;
  for (byte i = 1; i < 120; i++)
  {
      
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      m_printers[m_numPrinters].setPrinterNr(i);
      delay(10);
      r=m_printers[m_numPrinters].connect();
      if (r!=0) { 
        m_error_causedby = 1;
        m_error_subcode = r;
        m_error_printer = i;
        //m_state = MSTATE_ERROR;
      }
      //return;

      m_numPrinters++;
    }
  }
  
  return m_numPrinters;
}


void controlboard::handleState() {
  
 int r;
 switch (m_state) {
    
    
    case MSTATE_CONNECTING: // connecting
      
      controlboard::getInstance()->scanPrinters();
      
      if (m_numPrinters==0) {
        controlboard::getInstance()->getDc()->showConnecting();
      }
      else {
        controlboard::getInstance()->getDc()->notifyStateChange();
        m_state = MSTATE_DASHBOARD;
      }
    break;
    
    case MSTATE_DASHBOARD: //


        if ((m_newprintspeed>-1) && ( controlboard::getInstance()->getCurrentPrinter()->getSpeedSettings()->printing!=m_newprintspeed)) {
           controlboard::getInstance()->getCurrentPrinter()->getSpeedSettings()->printing= m_newprintspeed;
           m_dc.invalidateScreen( DASHBOARDSTATE_REDRAW_TEMP);
        }
    
      if ((m_lastCall + CFG_REFRESH_STATUS < millis()) || ( millis() < m_lastCall) || (m_lastCall == 0)) {
        
        r  = controlboard::getInstance()->getCurrentPrinter()->updateStatus();
       
        
        if (r!=0) { 
          if (r==-2) {
            m_state = MSTATE_CONNECTING;
            m_dc.init();
            return;
          }
          else {
            m_error_causedby = 1;
            m_error_subcode = r;
            #ifdef CRASH_ON_COMM_ERROR
            m_state = MSTATE_ERROR;
            #else
            m_state = MSTATE_CONNECTING;
            m_dc.drawInitScreen();
            return;
            #endif
            //return;
          }
        }
        
        if (m_newprintspeed>-1) {
           controlboard::getInstance()->getCurrentPrinter()->setPrintingSpeed(m_newprintspeed);
           m_dc.invalidateScreen( DASHBOARDSTATE_REDRAW_TEMP);
           m_newprintspeed=-1; 
        }
        
        
        r  = controlboard::getInstance()->getCurrentPrinter()->updateSpeedSettings();
        if (r!=0) { 
          if (r==-2) {
            m_state = MSTATE_CONNECTING;
            m_dc.init();
            return;
          }
          else {
            m_error_causedby = 1;
            m_error_subcode = r;
            #ifdef CRASH_ON_COMM_ERROR
            m_state = MSTATE_ERROR;
            #else
            m_state = MSTATE_CONNECTING;
            m_dc.drawInitScreen();
            return;
            #endif
            //return;
          }
        }


        printerStatus * ps = controlboard::getInstance()->getCurrentPrinter()->getStatus();
        
        if (m_prevFlags!=ps->flags) {
          m_dc.invalidateScreen( DASHBOARDSTATE_REDRAW_ALL);  
          m_prevFlags = ps->flags;  
        }
        
        if (ps->flags & FLAG_SD_FILE_OPEN == FLAG_SD_FILE_OPEN) {
          r  = controlboard::getInstance()->getCurrentPrinter()->updateSDStatus();
        }
        
        int numExtruders = controlboard::getInstance()->getCurrentPrinter()->getNumExtruders();
        for (int i = 0; i<numExtruders ; i++ ) {
          pinMode(m_pins_leds[i],OUTPUT);
          
          
          digitalWrite(m_pins_leds[i], ps->extTempSet[i]>0 ? HIGH : LOW);
        }
        
        digitalWrite(m_pins_leds[4], ps->hbTempSet>0 ? HIGH : LOW);
        
        m_lastCall = millis();
        m_dc.invalidateScreen( DASHBOARDSTATE_REDRAW_TEMP);
      }
        
       m_dc.drawScreen(&m_printers[m_currentPrinter]);
    break;
    
    case MSTATE_ERROR:
      GLCD.ClearScreen();
      GLCD.CursorTo(0,0);
      GLCD.print("ERROR");
      
      GLCD.CursorTo(0,1);
      GLCD.print("Caused by: ");
      GLCD.print(m_error_causedby);
      GLCD.CursorTo(0,2);
      GLCD.print("Code: ");
      GLCD.print(m_error_subcode);
      GLCD.CursorTo(0,3);
      GLCD.print("Printer: ");
      GLCD.print(m_error_printer);
      
      delay(10000);
    break;
  } 
  
}
