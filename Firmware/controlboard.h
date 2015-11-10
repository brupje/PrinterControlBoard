#include "configuration.h"
#include "pins.h" 

#if DISPLAY_CONTROLLER==1
  #include <openGLCD.h>
  #include "fonts/Arial14.h"         // proportional font
  #include "fonts/SystemFont5x7.h"   // system font

  
  #include "glcd_control.h"
  
#endif




class controlboard {
  
  protected:
    static controlboard *s_instance;
    printer m_printers[NUM_PRINTERS];
    DISPLAY_CONTROL_CLASS m_dc;
    int m_currentPrinter = 0;
    int m_numPrinters = 0;
    int m_state=MSTATE_CONNECTING;
    int m_error_causedby=0;
    int m_error_subcode=0;
    int m_error_printer=-1;
    int m_rotateryPos=0;
    int m_ppos=0;
    int m_newprintspeed =-1;
    byte m_prevFlags=0;
    
    uint8_t m_pins_buttons[9] = PINS_BUTTONS;
    int m_pins_leds[6] = PINS_LEDS;
    AdvButton * m_buttons[9];
    unsigned long m_lastCall=0;
    
    
  public:
    static controlboard * getInstance() { 
      if (!s_instance)
        s_instance = new controlboard();
      return s_instance; 
    };
    inline DISPLAY_CONTROL_CLASS * getDc() { return &m_dc; };
  
    int scanPrinters();
    inline printer * getCurrentPrinter() { return &m_printers[m_currentPrinter]; };
    inline void setCurrentPrinter(int printerNr) { m_currentPrinter=printerNr; };
    inline int getNumPrinters() { return m_numPrinters;}; 
    inline printer * getPrinter(int p) { return &m_printers[p]; };
    
    void markPrintingSpeedUpdate(int newspeed) {
      m_newprintspeed=getCurrentPrinter()->getPrintingSpeed() + newspeed;
      if (m_newprintspeed<0) 
        m_newprintspeed=0;
      else if (m_newprintspeed > 10000)
        m_newprintspeed=10000;
    }
    
    void handleState();
    void updateRotaryPos();
    int getRotaryPos();
    void init();
    void moveRotary(int pos);
  
};
