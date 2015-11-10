#include <wirecom.h>


wirecom * wirecom::s_instance = 0;


uint8_t
crc8(uint8_t *p, uint8_t len)
{
        uint16_t i;
        uint16_t crc = 0x0;

        while (len--) {
                i = (crc ^ *p++) & 0xFF;
                crc = (crc_table[i] ^ (crc << 8)) & 0xFF;
        }

        return crc & 0xFF;
}

void wirecom::sendData(printerCmd *cmd, uint8_t *p,uint8_t size) {
  cmd->datasize = size;
  
  char buf[ sizeof(printerCmd)+MAX_DATA_SIZE];
  char sendbuf[ sizeof(printerCmd)+MAX_DATA_SIZE+1];
  
  memset(&buf,0, sizeof(printerCmd)+MAX_DATA_SIZE);
  
  memcpy(&buf[0],(uint8_t *) cmd, sizeof(printerCmd));
  memcpy(&buf[sizeof(printerCmd)],p, size);
  /*
#ifdef CB_DEBUG 
  Serial.print("Buffer: ");
  for (int i =0; i<sizeof(printerCmd)+MAX_DATA_SIZE; i++){
    Serial.print(buf[i],HEX);
    Serial.print(" ");
  }
  Serial.println("END");
#endif*/
  byte crc = crc8((uint8_t *)buf,sizeof(printerCmd)+MAX_DATA_SIZE);

  
  
  sendbuf[0]=crc;
  memcpy(&sendbuf[1],(uint8_t *) buf, sizeof(printerCmd)+MAX_DATA_SIZE);
  
  Wire.write(sendbuf, sizeof(printerCmd)+MAX_DATA_SIZE+1);

}

int wirecom::getData(printerCmd &cmd, uint8_t * buffer, uint8_t size,printerCmd &retcmd,uint8_t& bytesrecv ) {
  bool retval;
  int a,cCount;
  char c[sizeof(printerCmd)+MAX_DATA_SIZE];

  
  a= Wire.available();
  if (a==0){
    return GETDATA_RET_NODATA;
  }

  byte crc = Wire.read();
  a=a-1;
  cCount=0;
  while (cCount<a){
    c[cCount] = Wire.read();
    cCount++;
  }
  
  byte crccalc=crc8((uint8_t*) c,sizeof(printerCmd)+MAX_DATA_SIZE);
  
  #ifdef CB_DEBUG 
    Serial.print(cCount+1);
    Serial.println(" bytes read");

  #endif  
  /*if (cCount != sizeof(printerCmd) + size) {
    
    return GETDATA_RET_SIZEMISMATCH;
  }
  else {  */

    //printerCmd retCmd;
    
    memcpy(&retcmd,&c[0],sizeof(printerCmd));
    
    byte cmdNr = retcmd.nr & 15;
  #ifdef CB_DEBUG 
    
    Serial.print(retcmd.datasize);
    Serial.println(" bytes in packet");
    Serial.print(cCount+retcmd.datasize);
    Serial.println(" bytes received total");
    Serial.print("Cmd: ");
    Serial.print(retcmd.cmd);
    Serial.print(" Cmd ret: ");
    Serial.print(cmd.cmd);
    Serial.print(" Command retval: ");
    Serial.print(retcmd.retval);
    Serial.print(" crc: ");
    Serial.print(crc);
    Serial.print(" crc calc: ");
    Serial.println(crccalc);
    
  #endif  

    /*if (retcmd.cmd != cmd.cmd) 
      return GETDATA_RET_CMDMISMATCH;
    else*/ if ((retcmd.nr>>4) != API_VERSION) 
      return GETDATA_RET_APIVERSIONMISMATCH;  
    //else if (retcmd.datasize != size) 
    //  return GETDATA_RET_DATSIZEMISMATCH;  
    else if (crc!=crccalc)
      return GETDATA_RET_CRCMISMATCH;
    else if ((retcmd.datasize<0)||(retcmd.datasize>MAX_DATA_SIZE))
      return GETDATA_RET_DATSIZEMISMATCH;  
    else if (retcmd.retval != CMD_RETVAL_OK){
      return GETDATA_RET_ERROR;
    }
    else {
      if (size>0) {
        memcpy(buffer+bytesrecv,((uint8_t *) &c[0])+sizeof(printerCmd),retcmd.datasize);
        bytesrecv+=retcmd.datasize;
      }
      return GETDATA_RET_OK;
    }
  //}
  
  return GETDATA_RET_UNKNOWN;
}

int wirecom::sendCommand(int slave, int command, uint8_t * ptrInput, uint8_t sizeInput, uint8_t * ptrOutput, uint8_t sizeOutput) {
  digitalWrite(13,HIGH);
  delay(1);
  digitalWrite(13,LOW);
   
  bool morepackets=true;
  int r = GETDATA_RET_OK;
/*
    #ifdef CB_DEBUG 
      Serial.print("Sending ");
      Serial.print(sizeInput);
       Serial.println(" bytes");
    #endif  
    */
  delay(10);
  Wire.beginTransmission(slave);
  printerCmd cmd;
  printerCmd retcmd;
  cmd.nr = 0| (API_VERSION<<4);
  cmd.cmd = command;
  

    
  sendData(&cmd,ptrInput,sizeInput);
  
  if (Wire.endTransmission()!=0)
    return GETDATA_RET_TRANSMISSIONERROR;

  
  int c = 0;
  r = GETDATA_RET_NODATA;
  uint8_t bytesrecv=0;
  while (morepackets) {
    delay(10);
    /*
    #ifdef CB_DEBUG 
      Serial.print("Requesting packet of size ");
      Serial.println(sizeof(printerCmd) + MAX_DATA_SIZE);
    #endif  */
    
    byte numRead = Wire.requestFrom(slave, sizeof(printerCmd) + MAX_DATA_SIZE +1);
    /*
    #ifdef CB_DEBUG 
      Serial.println("Reading response...");
    #endif   */
    morepackets=false;
    if (Wire.available() && (numRead>0)){
      r= getData(cmd,ptrOutput,sizeOutput,retcmd,bytesrecv);
      morepackets =((retcmd.nr & CB_FLAG_MORE_PACKETS )== CB_FLAG_MORE_PACKETS);
    }
    else 
      return GETDATA_RET_TRANSMISSIONERROR;
    
    
        /*
    #ifdef CB_DEBUG 
      Serial.print(bytesrecv);
      Serial.print(" bytes received. ");  
      if (morepackets) {
        
        Serial.println("More packets!");  
      }
      else
        Serial.println("Done");
    #endif */
    c++;
    if (c>10) break;
  }

  return r;
}

void wirecom::setLastCommand(void * ptr, int len) {
  clearOutputBuffer();
  memcpy(&lastCmd,ptr,sizeof(printerCmd));
  if (lastCmd.datasize>0)
    memcpy(lastCmdData,ptr+sizeof(printerCmd),lastCmd.datasize);
    
#ifdef CB_DEBUG  
  String s = "Got command: ";
  s.concat(lastCmd.cmd);
  s.concat(" size: ");
  s.concat(lastCmd.datasize);
  addLog(s);
#endif
};


void requestEvent() {

  wirecom::getInstance()->handleCommand();
}


void receiveEvent(int howMany)
{
  char c[100];
  int cCount=0;
  byte crc=Wire.read();
  
  while(Wire.available()) // loop through all but the last
  {
    c[cCount] = Wire.read(); // receive byte as a character
    cCount++;
  }

  wirecom::getInstance()->setLastCommand( (void *) &c[0],cCount );
}


void wirecom::handleCommand() {

  if (hasBuffer()) {
    #ifdef CB_DEBUG 
      String s = "Buffer not empty, continue command ";
      s.concat(lastCmd.cmd);
      addLog(s);
    #endif
    sendBuffer();
    return;
  }

#ifdef CB_DEBUG 

  String s = "Processing command: ";
  s.concat(lastCmd.cmd);
  addLog(s);
#endif
  callback(lastCmd);
  

}


void wirecom::sendBuffer() {
  lastCmd.datasize = ( this->outputSize-this->bytesSend < MAX_DATA_SIZE) ?this->outputSize- this->bytesSend : MAX_DATA_SIZE;
  lastCmd.nr = ((this->bytesSend+lastCmd.datasize<this->outputSize) ? CB_FLAG_MORE_PACKETS:0) | (API_VERSION<<4);
  
#ifdef CB_DEBUG 
  String s = "Send ";
  s.concat(lastCmd.datasize);
  s.concat(" bytes, nr ");
  s.concat(lastCmd.nr & 15 );
  s.concat(" start ");
  s.concat(this->bytesSend );
  s.concat("/");
  s.concat(this->outputSize );
  addLog(s);
#endif  
  
  sendData(&lastCmd, this->outputBuffer+this->bytesSend, lastCmd.datasize);  
  //sendData(&lastCmd,NULL,0);
  this->bytesSend += lastCmd.datasize;
  

  //clearOutputBuffer();
}

void wirecom::setOutputBuffer(uint8_t *p,uint8_t size) {
  

  clearOutputBuffer();
#ifdef CB_DEBUG 

  String s = "Queuing send buffer for ";
  s.concat(size);
  s.concat(" bytes");
  addLog(s);
#endif 
  
  this->outputSize=size;
  if (size > 0) {
    memcpy(this->outputBuffer,p,size); 
  }
  lastCmd.nr = 0;
  
  sendBuffer();
}

void wirecom::clearOutputBuffer() {
#ifdef CB_DEBUG 
  String s = "Clearing outputbuffer";
  addLog(s);
#endif  
  
  this->outputSize=0;
  this->bytesSend=0;
}


void wirecom::initSlave(int slavenr, void (*f)(printerCmd& p)) {
  lastCmdData = (uint8_t*) malloc(26);
  outputBuffer = (uint8_t*) malloc(100);
  callback = f;
  Wire.begin(slavenr);
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
}
