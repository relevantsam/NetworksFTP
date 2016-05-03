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
#define PAKSIZE 256
#define ACK 0
#define NAK 1
#define WINDOW_SIZE 16

using namespace std;

bool init(int argc, char** argv);
bool getFile(string fn);
//char * recvPkt();
bool isvpack(unsigned char * p);
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
  // Check the usage
  if (argc != 4) {
    cout << USAGE << endl;
    return false;

  }
  
  // Get the file name
  fn = argv[2];
  // Initialize using the args
  if(!init(argc, argv)) return -1;

  string message = "GET " + fn;
  cout << message << endl;
 
  // Send GET request
  if(sendto(sock, message.c_str(), PAKSIZE, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
    cout << "Package sending failed. (socket s, server address sa, message m)" << endl;
    return false;
  }
  
  // Run the Get File method
  getFile(fn);

  return 0;
}

 /*Intitialize base*/
bool init(int argc, char** argv) {
  base = 0; // Start with packet 0
  sock = 0; // initialize our socket

  hs = argv[1]; 
  srv_port = 10073; // Target port

  delayT = atoi(argv[3]); // Get our delay value

  // Init socket
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    cout << "Socket creation failed. (socket s)" << endl;
    return false;
  }

  memset((char *)&client, 0, sizeof(client)); // Secure memory for the client
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = htonl(INADDR_ANY);
  client.sin_port = htons(PORT);

  memset((char *)&server, 0, sizeof(server)); // Secure memory for the server
  server.sin_family = AF_INET;
  server.sin_port = htons(srv_port);
  inet_pton(AF_INET, hs.c_str(), &(server.sin_addr));

  // Bind socket for the client
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

bool isvpack(unsigned char * p) { // validate a raw packet

  // Build packet into strings
  char * sequenceNumberString = new char[3];
  memcpy(sequenceNumberString, &p[0], 3);
  sequenceNumberString[2] = '\0';

  char * checksumString = new char[6];
  memcpy(checksumString, &p[2], 5);
  checksumString[5] = '\0';
      
  char * dataBuffer = new char[PAKSIZE + 1];
  memcpy(dataBuffer, &p[8], PAKSIZE);
  dataBuffer[PAKSIZE] = '\0';

  // Convert these stings to ints
  int sn = boost::lexical_cast<int>(sequenceNumberString);
  int cs = boost::lexical_cast<int>(checksumString);

  // Validate SN
  if(sn < (base % 32) && sn > (base % 32) + WINDOW_SIZE - 1) { cout << "Bad sequence number. " << base % 32 << " " << ((base % 32) + WINDOW_SIZE - 1) % 32 << endl; return false; }
  // if SN is less than the current base mod 32 AND sn is bigger than the base mod 32 + 256 - 1 (EX. 11+31 - SN is out of range)
  // Generate a package with sequence #
  Packet pk (sn, dataBuffer);

  int chcksumGn = pk.generateCheckSum();  // Calculate the checksum for the package
  // Validate
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
    cout << endl << endl << "===================" << endl << "RECEIPT OF DATA"  << endl << "===================" << endl;
    if(packet[0] == '\0') { // At EOF
      cout << endl << endl << "===================" << endl << "RECEIVED END OF FILE"  << endl << "===================" << endl;
      break; // Jump to after loop
    }

    for(int i = 0; i < PAKSIZE; i++) {
      packetData[i] = packet[i + 8]; // Copy the data from the packet into the packetData variable with offset for header
    }
  	packetData[PAKSIZE] = '\0'; // Null terminate the string for packet data
  	packet[PAKSIZE + 8] = '\0'; // Terminate the string for the packet to avoid display errors
    if (rlen > 0) { // rlen indicates a packet received. 
  	  char * sequenceNumberString = new char[3]; // ##\0
  	  memcpy(sequenceNumberString, &packet[0], 3); // Copy ## in
  	  sequenceNumberString[2] = '\0'; // null terminate string to avoid display errors
		
      char * checksumString = new char[6]; // #####\0
      memcpy(checksumString, &packet[2], 5); // Copy ######
      checksumString[5] = '\0'; // null terminate string to avoid display errors
	    int pid = boost::lexical_cast<int>(sequenceNumberString); // Use Boost to convert the string to an ID.
      if(isvpack(packet)) { // Validate the package with isvpack 
    		if(pid == base % 32) {  // If the sequence number is the same as the base mod 32, it's what we wanted
          /// Print data
          cout << "Raw Packet: " << packet << endl;
          cout << "Seq. num: " << sequenceNumberString << endl;
          cout << "Checksum: " << checksumString << endl;
          cout << "Payload: " << packetData << endl;
    			file << packetData; // Write to the file
    			file.flush(); // Execute
          base++; //increment base data received to indicate next desired data
    		} else {
          cout << "IGNORING UNWANTED DATA (SN " << pid << " does not match desired: " << base % 32 << ")" << endl;
        }
      } else cout << "ERR " << pid << endl; // Otherwise, it's a corruption error

      cout << "Sent response: ";
      cout << "ACK " << base << endl;

  	  if(packet[6] == '1') {
        usleep(delayT*1000);
        cout << "============================================" << endl << "DELAYING" << endl << "============================================" << endl;
      }

	     string wbs = to_string((long long)base); // String of base
	     const char * ackval = wbs.c_str(); // C_Str of base

      // Send an ACK to the server
      if(sendto(sock, ackval, 10, 0, (struct sockaddr *)&server, server_length) < 0) {
		    perror("Sending ACK Failed");
        return 0;
      }
	    delete sequenceNumberString; // Unset string
      delete checksumString; // Unset string
    }
  }
  cout << "'GET' file transfer complete." << endl;
  file.close();
  return true;
}


