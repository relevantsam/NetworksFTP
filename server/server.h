/*
 * FTP SERVER HEADER
 */

 #ifndef SERVER_H
 #define SERVER_H
 typedef unsigned char byte;

 const int NUM_ARGS = 3; // 2 args - argv[0] is name, argv[1] is the target server, argv[2] is file name
 const int CLIENT_PORT_NUM = 10070; // port number assigned to group
 const int SERVER_PORT_NUM = 10073; // port number assigned to group

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

 sockaddr_in getServerAddr(char* serverName);

 void closeSocket(int sd);

 int sendGETRequest(int sd, sockaddr_in to, char* fileName);

 int validation(struct packet);

 int reassembly(struct packet);

 #endif 