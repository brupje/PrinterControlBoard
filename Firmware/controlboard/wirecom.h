#ifndef WIRECOM_H
#define WIRECOM_H

#include <Wire.h>
#include <Arduino.h>

//#define CB_DEBUG 1
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
#define CB_AXIS_E 8


struct SDprintStatus {
  uint8_t perccomplete;
  
};

struct printerCmd {  
  byte nr;
  byte cmd;
  byte retval;
  byte datasize;

};

struct extruderTemp {
  byte extruder;
  int temp;
};


struct cb_moveAxis {
  byte axis;
  byte extruder;
  float pos;
  bool relative;
};

struct cb_loadFilament {
  byte extruder;
};


#define CMD_GETSTATUS 1
#define CMD_SCAN 2
#define CMD_HOME 3
#define CMD_MOVE_AXIS 4
#define CMD_GETSPEED 5
#define CMD_LOADFILAMENT 6
#define CMD_UNLOADFILAMENT 7

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
#define CMD_RETVAL_ERROR_SD_INIT_FAILED 2


#define GETDATA_RET_OK 0
#define GETDATA_RET_NODATA 1
#define GETDATA_RET_SIZEMISMATCH 2
#define GETDATA_RET_NRMISMATCH 3
#define GETDATA_RET_CMDMISMATCH 4
#define GETDATA_RET_DATSIZEMISMATCH 5
#define GETDATA_RET_APIVERSIONMISMATCH 6
#define GETDATA_RET_CRCMISMATCH 7
#define GETDATA_RET_TRANSMISSIONERROR 8
#define GETDATA_RET_UNKNOWN -1
#define GETDATA_RET_NODATA -2
#define GETDATA_RET_ERROR -3

static const uint8_t crc_table[] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
    0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
    0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
    0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
    0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
    0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
    0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
    0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
    0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
    0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
    0xfa, 0xfd, 0xf4, 0xf3
};


class wirecom  {
protected:
  static wirecom *s_instance;
  uint8_t * outputBuffer;
  uint8_t outputSize;
  uint8_t bytesSend;
  printerCmd lastCmd;
  uint8_t * lastCmdData;
  void sendBuffer();
  bool hasBuffer() {
    return this->bytesSend < this->outputSize;
  }
  
  void (*callback)(printerCmd& p);
  void sendData(printerCmd *cmd, uint8_t *p,uint8_t size);
  int getData(printerCmd &cmd, uint8_t * buffer, uint8_t size,printerCmd &retcmd,uint8_t& bytesrecv );
  
  volatile int curCmdIdx=0;
  volatile int curFillidx=0;
#ifdef CB_DEBUG
  char debugLog[30][50] = {"","","","","","","","","",""};
  void addLog(String s) { 

    s.toCharArray(&(debugLog[curFillidx  ][0]),50); 
    curFillidx++; 
    if (curFillidx>=30)
      curFillidx=0;
  };
   
#endif  
public:

#ifdef CB_DEBUG
    bool hasLog() { return strlen(debugLog[curCmdIdx])>0;};
    bool getLog(char* dest) { 
      if (strlen(debugLog[curCmdIdx])==0) 
        return false;
      
      memcpy(dest,&(debugLog[curCmdIdx][0]),50);
      debugLog[curCmdIdx][0]=0;
      
      curCmdIdx++;
      if (curCmdIdx>=30)
        curCmdIdx=0;
        
      return true; 
    }; 
#endif     
    
  static wirecom * getInstance() { 
    if (!s_instance)
      s_instance = new wirecom();
    return s_instance; 
  };
  
  void setLastCommand(void * ptr, int len);   
  
  volatile printerCmd& getLastCommand() { return lastCmd; };
  volatile byte getLastDataByte() { return *((byte*) lastCmdData); };
  volatile int getLastDataInt() { return *((int*) lastCmdData); };
  volatile uint8_t * getLastData() { return lastCmdData; };

  int sendCommand(int slave, int command, uint8_t * ptrInput, uint8_t sizeInput, uint8_t * ptrOutput, uint8_t sizeOutput);
  void initSlave(int slavenr, void (*)(printerCmd& p)) ;
  
  void clearOutputBuffer();
  void setOutputBuffer(uint8_t *p,uint8_t size);
  
  
  void handleCommand();
};


#endif
