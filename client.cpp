#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "packet.h"



#define USAGE "Usage Error. Expected usage: <Program Name> <Server IP Address> <File Name> <Packet Delay>"
#define BUFFSIZE 505
#define PORT 10070
#define PAKSIZE 512
#define ACK 0
#define NAK 1
#define WINDOW_SIZE 16

using namespace std;

bool init(int argc, char** argv);
bool getFile(string fn);
char * recvPkt();
bool isvpack(unsigned char * p);
Packet createPacket(int index);
bool sendPacket();
bool isAck();
void handleAck();
void handleNak(int& x);
int sequenceNum;
string hs;
short int srv_port;
char * file;
unsigned char* window[16]; //packet window
int base; //used to determine position in window of arriving packets
int length;
struct sockaddr_in client;
struct sockaddr_in server;
socklen_t server_length;
string fstr;
bool dropPacket;
Packet packet;
int delayT;
int sock;
unsigned char b[BUFFSIZE];
string fn;

int main(int argc, char** argv) {
 
  if (argc != 4) {
    cout << USAGE << endl;
    return false;

  }
 
  fn = argv[2];
  if(!init(argc, argv)) return -1;

  string message = "GET " + fn;
  cout << message << endl;
 
  if(sendto(sock, message.c_str(), BUFFSIZE + 7, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
    cout << "Package sending failed. (socket s, server address sa, message m)" << endl;
    return false;
  }
  
  getFile(fn);

  return 0;
}

 /*Intitialize base*/
bool init(int argc, char** argv) {
  base = 0;
  sock = 0;

  hs = argv[1]; 
  srv_port = 10073; 

  delayT = atoi(argv[3]);


  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    cout << "Socket creation failed. (socket s)" << endl;
    return false;
  }

  memset((char *)&client, 0, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = htonl(INADDR_ANY);
  client.sin_port = htons(PORT);

  memset((char *)&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(srv_port);
  inet_pton(AF_INET, hs.c_str(), &(server.sin_addr));

  if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
    cout << "Socket binding failed. (socket s, address a)" << endl;
    return false;
  }

  cout << endl;

  cout << "Server address (inet mode): " << inet_ntoa(server.sin_addr) << endl;
  cout << "Port: " << ntohs(server.sin_port) << endl;

  cout << endl << endl;


  sequenceNum = 0;
  dropPacket = false;
  return true;
}



Packet createPacket(int index){
    cout << endl;
    cout << "=== TRANSMISSION START ===" << endl;
    string mstr = fstr.substr(index * BUFFSIZE, BUFFSIZE);
    if(index * BUFFSIZE + BUFFSIZE > length) {
      mstr[length - (index * BUFFSIZE)] = '\0';
    }
    return Packet (sequenceNum, mstr.c_str());
}

bool sendPacket(){
    if(sendto(sock, packet.str(), BUFFSIZE + 7, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
      cout << "Package sending failed. (socket s, server address sa, message m)" << endl;
      return false;
    }
    else return true;
}
bool isAck() {
    recvfrom(sock, b, BUFFSIZE + 7, 0, (struct sockaddr *)&server, &server_length);

    cout << endl << "=== RESPONSE ===" << endl;
    cout << "Data: " << b << endl;
    if(b[6] == '0') return true;
    else return false;
}
void handleAck() {

}
void handleNak(int& x) {

      char * sequenceNumberString = new char[2];
      memcpy(sequenceNumberString, &b[0], 1);
      sequenceNumberString[1] = '\0';

      char * checksumString = new char[5];
      memcpy(checksumString, &b[1], 5);
      
      char * dataBuffer = new char[BUFFSIZE + 1];
      memcpy(dataBuffer, &b[2], BUFFSIZE);
      dataBuffer[BUFFSIZE] = '\0';

      cout << "Sequence number: " << sequenceNumberString << endl;
      cout << "Checksum: " << checksumString << endl;

      Packet pk (0, dataBuffer);
      pk.setSequenceNum(boost::lexical_cast<int>(sequenceNumberString));
      pk.setCheckSum(boost::lexical_cast<int>(checksumString));

      if(!pk.chksm()) x--; 
      else x = (x - 2 > 0) ? x - 2 : 0;
}

bool isvpack(unsigned char * p) {

  char * sequenceNumberString = new char[3];
  memcpy(sequenceNumberString, &p[0], 3);
  sequenceNumberString[2] = '\0';

  char * checksumString = new char[6];
  memcpy(checksumString, &p[2], 5);
  checksumString[5] = '\0';
      
  char * dataBuffer = new char[PAKSIZE + 1];
  memcpy(dataBuffer, &p[8], PAKSIZE);
  dataBuffer[PAKSIZE] = '\0';

  int sn = boost::lexical_cast<int>(sequenceNumberString);
  int cs = boost::lexical_cast<int>(checksumString);

  Packet pk (0, dataBuffer);
  pk.setSequenceNum(sn);



  if(!(sn >= (base % 32) && sn <= (base % 32) + WINDOW_SIZE - 1)) { cout << "Bad sequence number." << endl; return false; }
  int chcksumGn = pk.generateCheckSum();
  if(cs != chcksumGn) { cout << "Bad checksum. Expected: " << cs << " Generated: " << chcksumGn << endl; return false; }
  return true;
}


bool getFile(string fn){
  /* Loop forever, waiting for messages from a server. */
  cout << "Waiting on port " << PORT << "..." << endl;

  ofstream file;
  string fileName = "OUTPUT-" + fn;
  file.open(fileName.c_str());

  int rlen;
  int ack;
  
  for (;;) {
    unsigned char packet[PAKSIZE + 8];
    unsigned char packetData[PAKSIZE];
    rlen = recvfrom(sock, packet, PAKSIZE + 8, 0, (struct sockaddr *)&server, &server_length);
    cout << "RECEIPT OF DATA" << endl;
	if(packet[0] == '\0') break;

	for(int i = 0; i < PAKSIZE; i++) {
      packetData[i] = packet[i + 8];
    }
	packetData[PAKSIZE] = '\0';
    if (rlen > 0) {
	  char * sequenceNumberString = new char[3];
	  memcpy(sequenceNumberString, &packet[0], 3);
	  sequenceNumberString[2] = '\0';
		
      char * checksumString = new char[6];
      memcpy(checksumString, &packet[2], 5);
      checksumString[5] = '\0';
      cout << endl << endl << "DATA" << endl;
      cout << "Packet: " << packet << endl;
      cout << "Seq. num: " << sequenceNumberString << endl;
      cout << "Checksum: " << checksumString << endl;
	  cout << "Payload: " << packetData << endl;
	  int pid = boost::lexical_cast<int>(sequenceNumberString);
      if(isvpack(packet)) {
		if(pid == base % 32) { 
			base++; //increment base of window
			file << packetData;
			file.flush();
		}
      } else cout << "ERR " << pid << endl;

      cout << "Sent response: ";
      cout << "ACK " << base << endl;

	  if(packet[6] == '1') usleep(delayT*1000);

	  string wbs = to_string((long long)base);
	  const char * ackval = wbs.c_str();

      if(sendto(sock, ackval, 10, 0, (struct sockaddr *)&server, server_length) < 0) {
        cout << "Acknowledgement failed. (socket s, acknowledgement message ack, client address ca, client address length calen)" << endl;
		perror("sendto()");
        return 0;
      }
	  delete sequenceNumberString;
      delete checksumString;
    }
  }
  cout << "'GET' file transfer complete." << endl;
  file.close();
  return true;
}


