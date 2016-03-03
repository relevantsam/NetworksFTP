/*
 * FTP CLIENT HEADER
 */

 #ifndef CLIENT_H
 #define CLIENT_H
 typedef unsigned char byte;

 const int NUM_ARGS = 2; // 2 args - argv[0] is name
 const int CLIENT_PORT_NUM = 10070; // port number assigned to group

 // Header of packet - contains sequence # (0 or 1) and checksum (sum of bytes)
 struct header {
 	int sequence, checksum;
 };

 // Packet- contains header and data
 struct packet {
 	header h;
 	byte data[256];
 };

 int sendGETRequest(char* fileName);

 int validation(struct packet);

 int reassembly(struct packet);

 #endif 