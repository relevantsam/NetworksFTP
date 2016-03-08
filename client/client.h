/*
 * FTP CLIENT HEADER
 */

 #ifndef CLIENT_H
 #define CLIENT_H

 #include <string>

 typedef unsigned char byte;

 #define NUM_ARGS 3 // 2 args - argv[0] is name, argv[1] is the target server, argv[2] is the desired file name
 #define CLIENT_PORT 10070 // port number assigned to group
 #define SERVER_PORT 10073 // port number assigned to group
 #define BUFFSIZE 512 // Set the buffer size. Packet is 256 and must be able to hold the header, so 512 will be safe for sure.

 // Header of packet - contains sequence # (0 or 1) and checksum (sum of bytes)
 struct header {
 	int sequence, checksum;
 };

 // Packet- contains header and data
 struct packet {
 	header h;
 	byte data[256];
 };

 int initSocket();

 bool sendGet(int s, std::string message, struct sockaddr * server, int serverSize);

 void closeSocket(int s);

 int sendGETRequest(int sd, sockaddr_in to, char* fileName);

 int validation(struct packet);

 int reassembly(struct packet);

 #endif 