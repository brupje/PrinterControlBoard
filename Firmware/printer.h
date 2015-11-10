#ifndef PRINTER_H
#define PRINTER_H

#include "wirecom.h"



class printer {
  
  protected:
    char printerNr;
    char printerName[26]; 
    byte m_cmdNr=0;
    printerStatus currentStatus;
    printerSpeedSettings currentSpeedSettings;
    SDprintStatus currentSDStatus;
  public:
    int updateStatus();
    int updateSDStatus();
    void setPrinterNr(int nr) { this->printerNr = nr; };
    bool connect();
    printerStatus * getStatus() { return &currentStatus; };
    printerSpeedSettings * getSpeedSettings() { return &currentSpeedSettings; };
    
    SDprintStatus * getSDStatus() { return &currentSDStatus; };
    
    
    
    char * getName() {return printerName;}
    void getName(char *title,int size) { memcpy(title,this->printerName,size); title[size]=0; };
      
    void loadFilament(int extruder);
    void unloadFilament(int extruder);
    void setExtruderTemp(int extruder, int temp);
    void setHBTemp(int temp);
    int getExtruderTemp(int extruder) { return currentStatus.extTempCur[extruder]; };
    int getExtruderSetTemp(int extruder) { if(extruder==255) return currentStatus.hbTempSet; else  return currentStatus.extTempSet[extruder]; };
    int getHBSetTemp() { return currentStatus.hbTempSet; };
    
    float getAxisPos(byte axis) { 
     switch (axis) {
       case CB_AXIS_X:
         return  currentStatus.x;
       case CB_AXIS_Y:
         return  currentStatus.y;
       case CB_AXIS_Z:
         return  currentStatus.z; 
       default:
          return 0;   
     }
    };
    
    int getPrintingSpeed() { return currentSpeedSettings.printing; };
    int getExtruderSpeed() { return currentSpeedSettings.extruder; };
    
    int updateSpeedSettings();
    int SDReleaseCard();
    int SDStartPrint(const char * filename);
    int SDDeleteFile(const char * filename);
    int SDGetNumFiles(byte &numFiles);
    int SDGetFilename(int fileNr, char *filename, int bufferSize);
    
    void setPrintingSpeed(int perc);
    void setExtruderSpeed(int perc);
    void homeAxis(byte axis);
    void moveAxis(byte axis, float value, bool relative);
    void extrude(byte extruder, float value);
    
    int pausePrint();
    int stopPrint();
    int resumePrint();
    
    int getNumExtruders() { 
      for (int i=0; i<4; i++) 
        if (currentStatus.extTempCur[i]==-1)
          return i;
      return 4; 
    };
    bool hasHeatedBed() { return currentStatus.hbTempCur!=-1; };
};

#endif //PRINTER_H
