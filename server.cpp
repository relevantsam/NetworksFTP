#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "packet.h"

#define USAGE "Usage Error. Expected usage: <Program Name> <Probability of Packet Damage> (represented as an integer) <Probability of Packet Loss> (also as an integer) <Probability of Packet Delay> (also an integer)"
#define PORT 10073
#define PACKETSIZE 256
#define ACK 0
#define NAK 1
#define BUFFSIZE 505
#define TIMEOUT 15 //in ms
#define WINDOW_SIZE 16

using namespace std;

bool isvpack(unsigned char * p);
bool init(int argc, char** argv);
bool loadFile(string fileName);
bool sendFile();
bool isAck();
void handleAck();
void handleNak(int& x);
bool* gremlin(Packet * pack, int pC, int pL, int pD);
Packet createPacket(int index);
void loadWindow();
bool sendPacket();
string getGet();

struct sockaddr_in server;
struct sockaddr_in client;
socklen_t client_length;
int rlen;
int sock;
bool ack;
string fstring;
char * file;
string fileName;
int pC;
int pL;
int pD;
int delayT;
Packet packet;
Packet window[16];
int length;
bool dropPacket;
bool delayPacket;
int timeoutMS;
unsigned char buffer[BUFFSIZE];
int base;

int main(int argc, char** argv) {
  // Initialize file using args
  if(!init(argc, argv)) return -1;
  
  // Send the file now
  if(sendFile()) cout << "GET file complete." << endl;
  
  return 0;
}

bool init(int argc, char** argv){
  // Check args to make sure there are enough
  if(argc != 4) { 
    cout << USAGE << endl;
    return false;
  }

  // Pick up the percent params
  pC = atoi(argv[1]);
  pL = atoi(argv[2]);
  pD = atoi(argv[3]);

  // Evaluate what the timeout is (15ms)
  struct timeval timeout;
  timeout.tv_usec = TIMEOUT * 1000;
  timeoutMS = TIMEOUT;

  /* Create our socket. */
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    cout << "Socket creation failed." << endl;
    return 0;
  }

  // Set timeout options
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

  
  memset((char *)&server, 0, sizeof(server)); // Allocate memory for server
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);

  // Bind socket for server
  if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    cout << "Socket binding failed. (socket sock, address server)" << endl;
    return 0;
  }
  
  // Get the file name
  fileName = getGet();

  // Load the file
  if (!loadFile(fileName)) {
    cout << "Loading file failed. (filename " << fileName << ")" << endl;
    return false;
  }
  fstring = string(file); // Stick the file into a string

  //cout << "====================================" << endl << endl << "File preview: " << endl << fstring << endl << endl << "====================================" << endl << endl ;
  // ^ you could echo the file, but at 80kb it's huge.

  dropPacket = false; // Don't drop it from the start!
  
  return true;
}

string getGet(){

  fd_set stReadFDS;
	string fileName;
	int n;
 	for(;;) {
    // Wait to pick up a request for a file
    cout << "WAITING FOR FILE REQUEST" << endl;
    // Allocate memory to store the client in
    malloc(sizeof(client)); 
    client_length = sizeof(client);

 		n = recvfrom(sock, buffer, PACKETSIZE, 0, (struct sockaddr *)&client, &client_length);

 		buffer[n] = '\0'; // Null terminate buffer to prevent display errors
 		char * msg = strtok((char *)buffer, " "); // Grab the message
		string word = msg; // note that the first word should be GET
		if(word == "GET") {
			cout << "GET REQUEST RECEIVED" << endl << endl;
 			msg = strtok(NULL, " "); // Get the first word
 			fileName = msg; // Yup, that's the file name
 			cout << "Request for " << fileName << endl << endl;
 			break;
		} else {
			cout << "Request rejected." << endl;
			return NULL;
		}
 	}
	return fileName;
}

bool isvpack(unsigned char * p) {
  cout << endl << "Packet is valid " << endl;

  char * sequenceNumberString = new char[2];
  memcpy(sequenceNumberString, &p[0], 1);
  sequenceNumberString[1] = '\0';

  char * checksumString = new char[7];
  memcpy(checksumString, &p[1], 6);
  checksumString[6] = '\0';
      
  char * databuffer = new char[121 + 1];
  memcpy(databuffer, &p[2], 121);
  databuffer[121] = '\0';

  cout << "Packet: " << p << endl;
  cout << "Seq. num: " << sequenceNumberString << endl;
  cout << "Checksum: " << checksumString << endl;
  cout << "Message: " << databuffer << endl;

  int sequenceNum = boost::lexical_cast<int>(sequenceNumberString);
  int checksum = boost::lexical_cast<int>(checksumString);

  Packet pk (0, databuffer);
  pk.setSequenceNum(sequenceNum);

  // change to validate based on checksum and sequence number

  if(sequenceNum == 0) return false;
  if(checksum != pk.generateCheckSum()) return false;
  return true;
}


bool loadFile(string fileName) {
  // This is basic
  ifstream fileStream;
  fileStream.open(fileName, ifstream::binary);

  if(fileStream) {
    fileStream.seekg(0, fileStream.end);
    length = fileStream.tellg();
    fileStream.seekg(0, fileStream.beg);

    file = new char[length];

    cout << "Reading " << length << " characters..." << endl;
    cout << "File requires " << length / 256 << " packets" << endl;

    fileStream.read(file, length);

    if(!fileStream) { cout << "File reading failed. (filename " << fileName << "). Only " << fileStream.gcount() << " could be read."; return false; }
    fileStream.close();
  }
  return true;
}

void loadWindow(){
  cout << endl << "===============================" << endl << "CREATING WINDOW FROM " << 
    base << " TO " << base + WINDOW_SIZE - 1 << endl << "===============================" << endl;
	for(int i = base; i < base + WINDOW_SIZE; i++) {
		window[i-base] = createPacket(i); // EX. 0 - 15 
    // Deal with the last packet
		if(strlen(window[i-base].getDataBuffer()) < PACKETSIZE && window[i-base].chksm()) { 
			for(++i; i < base + WINDOW_SIZE; i++){
				window[i-base].loadDataBuffer("\0");
			}
      // Should we break here?
		}
	}
}

bool sendFile() {
	fd_set stReadFDS; 

	struct timeval stTimeOut;

  // Set up the FD options
	FD_ZERO(&stReadFDS);
	stTimeOut.tv_sec = 0;
	stTimeOut.tv_usec = 1000 * TIMEOUT; // 15ms
	FD_SET(sock, &stReadFDS); // Set in the socket

	base = 0; // Make sure the base is 0
	int fd_ready;
	
	int max_sd;
	bool hasRead = false;
	while(base * PACKETSIZE < length) { // While current base times the PACKETSIZE is less than the length of the file
    // Previously, BUFFSIZE but that didn't make sense. 
    // IE Base = 1 (1*256 is < length of the file)
		int final = 0;
		loadWindow(); // Load up the window in above function
		
		for(int i = 0; i < WINDOW_SIZE; i++) {
			packet = window[i]; // Select a packet
      if(packet.str()[0] == '\0') final = packet.getSequenceNum(); // If it's the final packet, get its sequence number
			if(!sendPacket()) continue; // if it doesn't send, go to the next iteration
		}
		while(final < WINDOW_SIZE - 1) { // When the last one in the window
      // Set the settings again.
			FD_ZERO(&stReadFDS);
			stTimeOut.tv_sec = 0;
			stTimeOut.tv_usec = 1000 * TIMEOUT;
			FD_SET(sock, &stReadFDS);

      // We're about to listen
			int t = select(sock + 1, &stReadFDS, NULL, NULL, &stTimeOut);
			if (t == -1){
				perror("select() err");
			}
			if (t == 0) {
				cout << "ACK Timed out" << endl;
				cout << "Timed out packet: " << base << endl;
				break;
			} 
      // We have a live one
			fd_ready = t;
			for(int u = 0; u <= sock &&  fd_ready > 0; u++){
				if(final + fd_ready < WINDOW_SIZE){
					if(final++ == WINDOW_SIZE - 2) break;
				}
        client_length = sizeof(client);
				if(recvfrom(sock, buffer, PACKETSIZE, 0, (struct sockaddr *)&client, &client_length) < 0) {
					cout << "ACK Timed out" << endl;
				} else hasRead = true;
				if(!hasRead) continue;
				if(isAck()) {
					handleAck();
					final++;
					if (final == WINDOW_SIZE - 1) break;
				} else { 
					cout << "Not an ACK!" << endl; 
				}
			}
		
		}

	}
  // Need to add sending the second to last package here
  struct sockaddr* clientAddr = (struct sockaddr*)&client;
	if(sendto(sock, "\0", PACKETSIZE, 0, clientAddr, client_length) < 0) {
		cout << "Final package sending failed. (socket sock, server address sa, message m)" << endl;
		return false;
	}
	return true;
}

Packet createPacket(int index){
  string message = fstring.substr(index * PACKETSIZE, PACKETSIZE); // Create the message 
	if(message.length() < PACKETSIZE) { // If the message is too small for the buffer
		message[length - index * PACKETSIZE] = '\0'; // Terminate the message with a null char 
  }
  return Packet (index % 32, message.c_str()); // Return the packet. The index is a valid sequence number
} 

bool sendPacket(){
	cout << endl << "=========================" << endl << "BEGIN PACKET SEND" << endl << "=========================" << endl;
	bool* pckStatus = gremlin(&packet, pC, pL, pD); // Tell us if something is going to be messed up

	dropPacket = pckStatus[0];
	delayPacket = pckStatus[1];

	if (dropPacket == true) return false; // Drop the bass
	if (delayPacket == true) packet.setAckNack(1); // If it's a delay, we'll deal with that on the client
  const char * pack = (const char *)packet.str(); // Get a string of the packet
  cout << "Packet: " << pack << endl; // Let's see it
  if(sendto(sock, pack, PACKETSIZE + 8, 0, (struct sockaddr *)&client, client_length) < 0) { // Send it over
    perror("Sending package error");
	  return false;
  }
  return true;
}
bool isAck() {
  cout << "====================" << endl << "Evaluating Received Data" << endl << "===================" << endl;
    cout << "Data: " << buffer << endl;
    if(buffer[6] == '\0') return true;
    else return false;
}
void handleAck() {
  cout << "====================" << endl << "RECEIVED ACK" << endl << "===================" << endl;
	int ack = boost::lexical_cast<int>(buffer);
	if(base < ack) base = ack;
	cout << "Window: " << base << endl;
}
void handleNak(int& x) {

      char * sequenceNumberString = new char[2];
      memcpy(sequenceNumberString, &buffer[0], 1);
      sequenceNumberString[1] = '\0';

      char * checksumString = new char[5];
      memcpy(checksumString, &buffer[1], 5);
      
      char * dataBuffer = new char[BUFFSIZE + 1];
      memcpy(dataBuffer, &buffer[2], BUFFSIZE);
      dataBuffer[BUFFSIZE] = '\0';

      cout << "Sequence number: " << sequenceNumberString << endl;
      cout << "Checksum: " << checksumString << endl;

      Packet pk (0, dataBuffer);
      pk.setSequenceNum(boost::lexical_cast<int>(sequenceNumberString));
      pk.setCheckSum(boost::lexical_cast<int>(checksumString));

      if(!pk.chksm()) x--; 
      else x = (x - 2 > 0) ? x - 2 : 0;
}
bool* gremlin(Packet * pack, int pC, int pL, int pD){

  bool* packStatus = new bool[2];
  int r = rand() % 100;

  packStatus[0] = false;
  packStatus[1] = false;

  if(r <= (pL)){
	packStatus[0] = true;
  }
  else if(r <= (pD)){
	  packStatus[1] = true;
  }
  else if(r <= (pC)){
    pack->loadDataBuffer((char*)"GREMLIN");
  }

  char substring[49];
  memcpy(substring, (pack->getDataBuffer()), 48);
  substring[48] = '\0';

  cout << "Seq. num: " << pack->getSequenceNum() << endl;
  cout << "Checksum: " << pack->getCheckSum() << endl;
  cout << "Message: "  << substring << endl;

  return packStatus;
}

