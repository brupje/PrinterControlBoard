
#include "printer.h"

#include <string.h>
#include <Arduino.h>
#include <openGLCD.h>



int printer::updateSpeedSettings() {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_GETSPEED,0,0,(uint8_t *) &currentSpeedSettings,sizeof(currentSpeedSettings));

  
}

int printer::updateStatus() {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_GETSTATUS,0,0,(uint8_t *) &currentStatus,sizeof(printerStatus));
  
}

int printer::updateSDStatus() {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_GETSTATUS,0,0,(uint8_t *) &currentSDStatus,sizeof(currentSDStatus));
  
}

int printer::pausePrint() {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_PAUSE,0,0,(uint8_t *) &currentStatus,sizeof(printerStatus));
  
}

int printer::stopPrint() {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_STOP,0,0,(uint8_t *) &currentSDStatus,sizeof(currentSDStatus));
  
}

int printer::resumePrint() {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_RESUME,0,0,(uint8_t *) &currentSDStatus,sizeof(currentSDStatus));
  
}

int printer::SDReleaseCard() {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_RELCARD,0,0,NULL,0);
  
}

int printer::SDStartPrint(const char * filename) {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_STARTPRINT,(uint8_t *) filename,strlen(filename)+1,NULL,0);
  
}

int printer::SDGetNumFiles(byte &numFiles) {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_GETNUMFILES,NULL,0,(uint8_t *) &numFiles,1);
  
}

int printer::SDGetFilename(int fileNr, char *filename, int bufferSize) {
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_GETFILENAME,(uint8_t *) &fileNr,sizeof(int),(uint8_t *) filename,bufferSize);
}

int printer::SDDeleteFile(const char * filename) {
  
  return wirecom::getInstance()->sendCommand(printerNr,CMD_SD_DELFILENAME,(uint8_t *) filename,strlen(filename)+1,NULL,0);
  
}
bool printer::connect() {
  int r =wirecom::getInstance()->sendCommand(printerNr,CMD_SCAN,0,0,(uint8_t *) &printerName[0],26);
  printerName[25]=0;

  return r  == GETDATA_RET_OK;
}



void printer::moveAxis(byte axis, float value, bool relative){
  cb_moveAxis t;
  t.axis = axis;
  t.pos = value;
  t.relative = relative;
  
  wirecom::getInstance()->sendCommand(printerNr,CMD_MOVE_AXIS,(uint8_t *) &t,sizeof(t),NULL,0);
}

void printer::extrude(byte extruder, float value) {
  cb_moveAxis t;
  t.axis = CB_AXIS_E;
  t.pos = value;
  t.relative = true;
  
  wirecom::getInstance()->sendCommand(printerNr,CMD_MOVE_AXIS,(uint8_t *) &t,sizeof(t),NULL,0); 
}

void printer::homeAxis(byte axis) {
  
  wirecom::getInstance()->sendCommand(printerNr,CMD_HOME,(uint8_t *) &axis,sizeof(axis),NULL,0);
}

void printer::setExtruderTemp(int extruder, int temp) {
  if (extruder==255) {
    extruderTemp t;
    t.extruder = extruder;
    t.temp = temp;
    wirecom::getInstance()->sendCommand(printerNr,CMD_CONTROL_HB_TEMP,(uint8_t *) &t,sizeof(extruderTemp),NULL,0);
  }
  else {
    extruderTemp t;
    t.extruder = extruder;
    t.temp = temp;
    
    wirecom::getInstance()->sendCommand(printerNr,CMD_CONTROL_EXTRUDER_TEMP,(uint8_t *) &t,sizeof(extruderTemp),NULL,0);
  }
  
}


void printer::loadFilament(int extruder) {
  cb_loadFilament t;
  t.extruder = extruder;
  wirecom::getInstance()->sendCommand(printerNr,CMD_LOADFILAMENT,(uint8_t *) &t,sizeof(cb_loadFilament),NULL,0);
}

void printer::unloadFilament(int extruder) {
  cb_loadFilament t;
  t.extruder = extruder;
  wirecom::getInstance()->sendCommand(printerNr,CMD_UNLOADFILAMENT,(uint8_t *) &t,sizeof(cb_loadFilament),NULL,0); 
}


void printer::setHBTemp(int temp){
  extruderTemp t;
  t.extruder = 255;
  t.temp = temp;
  wirecom::getInstance()->sendCommand(printerNr,CMD_CONTROL_HB_TEMP,(uint8_t *) &t,sizeof(extruderTemp),NULL,0);
}

void printer::setPrintingSpeed(int perc){
  if (perc<0) 
    perc=0;
  else if (perc>10000)
    perc=10000;
  
  currentSpeedSettings.printing = perc;
  wirecom::getInstance()->sendCommand(printerNr,CMD_CONTROL_PRINT_SPEED,(uint8_t *) &perc,sizeof(int),NULL,0);
}

void printer::setExtruderSpeed(int perc){
  wirecom::getInstance()->sendCommand(printerNr,CMD_CONTROL_EXTRUDER_SPEED,(uint8_t *) &perc,sizeof(int),NULL,0);
}

