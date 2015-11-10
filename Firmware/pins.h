#ifndef PINS_H
#define PINS_H


#if CONTROLBOARD==1

#define glcdData0Pin 22
#define glcdData1Pin 23
#define glcdData2Pin 24 
#define glcdData3Pin 25 
#define glcdData4Pin 26 
#define glcdData5Pin 27
#define glcdData6Pin     28
#define glcdData7Pin      29
#define glcdCSEL1          33
#define glcdCSEL2         34
#define glcdRW            35
#define glcdDI           36
#define glcdEN           37
#define glcdRES         A6    // Reset Bit
#define PIN_DBG 13

#define PINS_BUTTONS {19,42,43,44,45,46,47,2,3}

#define PINS_LEDS {17,16,7,8,9,53}


#endif

#endif //PINS_H
