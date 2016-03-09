/*
 * FTP SERVER HEADER
 */

 #ifndef SERVER_H
 #define SERVER_H
 typedef unsigned char byte;

 #define NUM_ARGS 3 // 2 args - argv[0] is name, argv[1] is the target server, argv[2] is the desired file name
 #define CLIENT_PORT 10070 // port number assigned to group
 #define SERVER_PORT 10073 // port number assigned to group
 #define PACKETSIZE 256 // Set the buffer size. Packets are 256 so must be 256
 #define BUFFSIZE 233 // Header will be 16 Bytes

 // Header of packet - contains sequence # (0 or 1) and checksum (sum of bytes)
 struct header {
 	int sequence, checksum;
 };

 // Packet- contains header and data
 struct packet {
 	header h;
 	byte data[BUFFSIZE];
 };

 int initSocket();

 void runServer();

 int checksumCal(byte packet[]);

 void closeSocket(int sd);

 int validation(struct packet);

 int reassembly(struct packet);

 void sendPacket(packet message, int socket, struct sockaddr * client, int size);

 #endif 