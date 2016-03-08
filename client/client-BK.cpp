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
 #include <sys/types.h> // Basic sys data types
 #include <sys/socket.h> // Basic socked definitions
 #include <netinet/in.h> // Basic internet definitiions
 #include <arpa/inet.h> 
 #include <netdb.h> // Needed for obtaining server address
 #include <unistd.h> // Needed to close socket
 // Include header file
 #include "client.h"
 // Other incldues
 #include <iostream>
 #include <string.h>
 // For now we will stay with std namespace
 using namespace std;

 // Main running function to receive commandline arguments
 int main(int argc, char *argv[]) {
 	//if(argc == NUM_ARGS - 1) { // check for expected number of inputs
 		// Change to readable names
 		//char * serverName = argv[1];
 		//char * fileName = argv[2];
 		cout << "===================================\n      Welcome to B(arlett)TP\n===================================" << endl;
 		//int socketID = initSocket();
 		//sockaddr_in server = getServerAddr(serverName);
 		//int get = sendGETRequest(socketID, server, fileName);
 		//closeSocket(socketID);

 		int sd;
 		struct sockaddr_in client;
 		struct sockaddr_in server;
 		struct hostent *hp;

 		string ip = string("131.204.14.175/24");
 		int port = CLIENT_PORT_NUM;

		//sd = socket(AF_INET, SOCK_DGRAM, 0);
		if((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) { // Let's try the IPPROTO_UDP thing
			cout << "Socket failure." << endl;
			return 1;
		} else {
			cout << "Socket created. Socket Num: " << sd << endl;
		}

 		server.sin_family = AF_INET;
 		server.sin_port = htons(10073);
 		cout << "Configuring server." << endl;
 		//hp = gethostbyname(argv[1]);
 		//hp = gethostbyname("131.204.14.175/24");

  		inet_pton(AF_INET, .c_str(), AF_INET);
 		cout << "Configuring server." << endl;

 		memcpy(hp->h_addr,&(server.sin_addr), hp->h_length);
	 	for(;;) {
		 	sendto(sd, "HI", 2, 0, (struct sockaddr *)&server, sizeof(server)); // 0 is flags
		 	sleep(2);
		}
		close(sd);
 		//cout << 1 << endl;
 	//} else {
 		// Output usage
 	//	cout << "ERROR: Expected usage for BTP is with " << NUM_ARGS << " arguments: <Program Name> <Server IP> <Requested File Name>" << endl;
 	//}
 	return 0;
 }

 int initSocket() {
 	/*
 	 * Declare variables
 	 */
 	// sd is an int representing socket or -1 if socket failed
 	int sd;
 	// Structure modeling the client machine
 	struct sockaddr_in client;

 	/* 
 	 * Fill client socket
 	 */
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

 sockaddr_in getServerAddr(char* serverName) {
 	// Represents host
 	struct hostent *hp;
 	// Structure modeling server
 	struct sockaddr_in server;
 	// Obtain server address
 	hp = gethostbyname(serverName);
 	// copy address from hp into server struct
 	memcpy(hp->h_addr,&(server.sin_addr), hp->h_length);
 	server.sin_family = AF_INET; // Set family to internet protocol
 	server.sin_port = htons(SERVER_PORT_NUM); // htons short int from host -> network byte order
 	return server;
 }
 
 void closeSocket(int sd) {
 	close(sd); // Need to close at other end as well
 }

 // Function to send get request to server program. 
 int sendGETRequest(int sd, sockaddr_in to, char* fileName) {
 	cout << "BTP Sending GET request for file " << fileName << endl;
 	//char *buffer = "Sample message";
 	// Send a message! How exciting
 	for(;;) {
	 	int sending = sendto(sd, "hi!", 2, 0, (struct sockaddr *)&to, sizeof(to)); // 0 is flags
	 	cout << "Sending hi" << endl;
	 	sleep(2);
	 }
 	return 1;
 }
 