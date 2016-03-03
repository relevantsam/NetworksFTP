/*
 * FTP CLIENT
 * 
 * Functionality: 
 *
 * Sends FTP request to server at IP with ports 10070, 10071, 10072, 10073
 *
 * Implement GET request only (GET TestFile)
 *
 * Receives packets (256-byte packets)
 * 
 * Process packet in error detection function
 *
 * -- Check Checksum 
 * ---- Sum all bytes in packet and compare to checksum in header
 * ---- if correct, return ACK and sequence 
 * ---- if incorrect, return NAK and sequence 
 * ------- print that packet has errors and sequence number
 *
 * Print each packet and ACK/NAK and data
 *
 * Process packet in re-assembly function 
 *
 * Close file when receive 1-byte file with null character
 *
 * Send FTP response message with header GET successfully completed
 * 
 *  
 */

 // INCLUDE SOCKET REQUIREMENTS
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <netdb.h>
 // Include header file
 #include "client.h"
 // Other incldues
 #include <iostream>
 // For now we will stay with std namespace
 using namespace std;

 // Main running function to receive commandline arguments
 int main(int argc, char *argv[]) {
 	if(argc == NUM_ARGS) { // check for expected number of inputs
 		cout << "===================================\n      Welcome to B(arlett)TP\n===================================" << endl;
 		int get = sendGETRequest(argv[1]);
 		cout << to_string(get) << endl;
 	} else {
 		// Output usage
 		cout << "ERROR: Expected usage is with " << to_string(NUM_ARGS) << " arguments." << endl;
 	}
 	return 0;
 }
 
 // Function to send get request to server program. 
 int sendGETRequest(char* fileName) {
 	cout << "Sending GET request for file " << fileName << endl;
 	// sd is an int representing socket or -1 if socket failed
 	int sd;
 	// Create a structure modeling the client machine
 	struct sockaddr_in client;
 	// AF_INET is internet protocol
 	client.sin_family = AF_INET;
 	// Assigns address to IP address of machine
 	client.sin_addr.s_addr = htonl(INADDR_ANY); // htonl takes long int from host to network byte order
 	// Assigns port number to port number specified in head file 
 	client.sin_port = htons(CLIENT_PORT_NUM); // htons takes short int from host to network byte order
 	// AF_INET is internet protocol, SOCK_DGRAM is UDP, 0 selects protocol
 	sd = socket(AF_INET, SOCK_DGRAM, 0);
 	// Associate socket with local machine
 	bind(sd, (struct sockaddr *)&client, sizeof(client));
 	return sd;
 }
 