#include "packet.h"
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <sstream>

  //Constructor
  Packet::Packet () {
	sequenceNum = 0;
	checkSum = 0;
	ackNack = 0;
        dataBuff[512];
  }
  Packet::Packet (int sn, const char dataBuffer[505]){
    sequenceNum = sn % 32;
    strcpy(dataBuff, dataBuffer);
    checkSum = generateCheckSum();
    ackNack =0;
  }
  //Setter Methods
  void Packet::setSequenceNum(int sn){
     sequenceNum = sn;
  }
 
  void Packet::setCheckSum(int cs){
     checkSum = cs;
  }
 
  void Packet::setAckNack(int an){
     ackNack = an;
  }

  void Packet::loadDataBuffer(char* data){
    strcpy(dataBuff, data);
  }
  char* Packet::getDataBuffer() {
    return dataBuff;
  }
  //Attach header to the data array
  char* Packet::str(){
    std::string tempStr(dataBuff);
    std::string packetString;
    std::string checksumString;
    std::string sequenceNumberString;
	
    checksumString = std::to_string((long long int)checkSum);
    while(checksumString.length() < 5) checksumString += '0';

	sequenceNumberString = std::to_string((long long int)sequenceNum);
	if(sequenceNumberString.length() < 2) sequenceNumberString.insert(0, 1, '0');

    packetString = sequenceNumberString + checksumString + std::to_string((long long int)ackNack) + tempStr;

    strcpy(packet, packetString.c_str());
    return packet;
  }
  //Getter Methods
  int Packet::getSequenceNum(){
    return sequenceNum;
  }
 
  int Packet::getCheckSum(){
    return checkSum;
  }

  int Packet::getAckNack(){
    return ackNack;
  }
  bool Packet::chksm() {
    return (checkSum) == generateCheckSum();
  }
  int Packet::generateCheckSum() {
    int checksum = 0;
    if(dataBuff == NULL){
      return -1;
    }
    
    for(int x = 0; x < sizeof(dataBuff); x++) {
      if(dataBuff[x] == '\0') {
        x = sizeof(dataBuff);
        break;
      }
      checksum += dataBuff[x];
    }

    if(checksum <= 9999) checksum *= 10;

    if(checksum > 0) return checksum;
    return -1;
  }
