#ifndef STRUCTS_H
#define STRUCTS_H
#include <Wire.h>
#include <Arduino.h>

#define MAX_DATA_SIZE 27
#define CB_FLAG_MORE_PACKETS 8
#define API_VERSION 1

struct printerStatus {
  byte flags;

  int extTempCur[4];
  int extTempSet[4];

  int hbTempCur;
  int hbTempSet;
  
  float x;
  float y;
  float z;
};

struct printerSpeedSettings {
  int printing;
  int extruder;
};

#define FLAG_SD_FILE_OPEN 1
#define FLAG_SD_IS_PRINTING 2
#define FLAG_SD_IS_DONE 4

#define CB_AXIS_X 1
#define CB_AXIS_Y 2 
#define CB_AXIS_Z 4
#define CB_AXIS_ALL 7

struct SDprintStatus {
  uint8_t perccomplete;
  
};

struct printerCmd {  
  byte nr;
  byte cmd;
  byte retval;
  byte datasize;
  byte crc;

};

struct extruderTemp {
  byte extruder;
  int temp;
};


struct cb_moveAxis {
  byte axis;
  int pos;
};

#define CMD_GETSTATUS 1
#define CMD_SCAN 2
#define CMD_HOME 3
#define CMD_MOVE_AXIS 4
#define CMD_GETSPEED 5

#define CMD_SD_GETSTATUS 20
#define CMD_SD_GETNUMFILES 21
#define CMD_SD_GETFILENAME 22
#define CMD_SD_DELFILENAME 23
#define CMD_SD_RELCARD 24
#define CMD_SD_STARTPRINT 25
#define CMD_SD_PAUSE 26
#define CMD_SD_STOP 27
#define CMD_SD_RESUME 28

#define CMD_CONTROL_EXTRUDER_TEMP 40
#define CMD_CONTROL_HB_TEMP 41
#define CMD_CONTROL_PRINT_SPEED 42
#define CMD_CONTROL_EXTRUDER_SPEED 43



#define CMD_RETVAL_OK 0
#define CMD_RETVAL_ERROR_UNKNOWN_COMMAND 1




#endif //STRUCTS_H


