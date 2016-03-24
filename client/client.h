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
 #define PACKETSIZE 256 // Set the buffer size. Packets are 256 so must be 256
 #define BUFFSIZE 240 // Header will be 16 Bytes

 // Header of packet - contains sequence # (0 or 1) and checksum (sum of bytes)
 struct header {
 	bool sequence;
 	int checksum;
 };

 // Packet- contains header and data
 struct packet {
 	header h;
 	byte data[BUFFSIZE];
 };

 int initSocket();

// Send message section
 bool sendGet(int s, std::string fileName, struct sockaddr * server, int serverSize);
 bool sendACK(int s, struct sockaddr * server, int serverSize);
 bool sendNAK(int s, struct sockaddr * server, int serverSize);

 bool getFile(std::string fileName, int s, struct sockaddr * server, socklen_t * serverSize);

 int closeSocket(int s);

 int validation(struct packet);

 packet byteDataToPacket(byte data[PACKETSIZE]);

 int reassembly(struct packet);
 
 int checksumCal(byte packet[]);

 #endif 