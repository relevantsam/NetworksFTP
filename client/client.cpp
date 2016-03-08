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
 #include <fstream>
 // For now we will stay with std namespace
 using namespace std;

 int main(int argc, char *argv[]) {
	if (argc != NUM_ARGS) {
		cout << "Usage Error. Expected usage: <Program Name> <Server IP Address> <File Name>" << endl;
		return 0;
	}

	int s; // store the socket id
	string serverIP = argv[1]; // The server ip
	string fileName = argv[2]; // The file name
	// Declare storages for our server and client addresses
	struct sockaddr_in server;
	struct sockaddr_in client; 

	if((s = initSocket()) < 0) {
 		cout << "Error creating socket. Quiting." << endl;
		return 0;
	}

	// Set up the client.
 	memset((char *)&client, 0, sizeof(client)); // Delegate space in memory
 	client.sin_family = AF_INET; // Internet family
 	client.sin_addr.s_addr = htonl(INADDR_ANY); // Doesn't matter what the IP is
 	client.sin_port = htons(CLIENT_PORT); // Set the port to the client port

 	// Set up the server.
 	memset((char *)&server, 0, sizeof(server)); // Delegate space in memory
 	server.sin_family = AF_INET; // Internet family;
 	server.sin_port = htons(SERVER_PORT); // Port is the server port in header file
 	inet_pton(AF_INET, serverIP.c_str(), &(server.sin_addr)); // Set the IP to the IP given

 	// And now let's bind our client to our socket
 	if(bind(s, (struct sockaddr *)&client, sizeof(client)) < 0){
 		cout << "Error binding socket to client. Quiting." << endl;
 		return 1;
 	} else {
 		cout << "Socket successfully bound to client." << endl;
 	}

 	cout << endl << "============================" << endl << "+      CLIENT RUNNING      +" << endl << "============================" << endl;


 	// Send a get request
	if(!sendGet(s, fileName, (struct sockaddr *)&server, sizeof(server))) {
		cout << "Sending GET request failed. Quiting." << endl;
	} else {
		cout << "GET request sent for file " << fileName << endl;
	}

	// Get the file
	int serverSizeInt = sizeof(server);
	getFile(fileName, s, (struct sockaddr *)&server, (socklen_t*)&serverSizeInt);

	return closeSocket(s);
}

int initSocket() {
	int sock;
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
 		return -1;
 	} else {
 		cout << "Socket " << sock << " created!" << endl;
 		return sock;
 	}
}

// Function to send get request
bool sendGet(int s, string fileName, struct sockaddr * server, int serverSize) {
 	string message = "GET " + fileName;
	return (sendto(s, message.c_str(), PACKETSIZE, 0, server, serverSize)) >= 0; // 0 is flags
}

bool sendACK(int s, struct sockaddr * server, int serverSize) {
	string message = "ACK";
	return (sendto(s, message.c_str(), PACKETSIZE, 0, server, serverSize)) >= 0; // 0 is flags
}

bool sendNAK(int s, struct sockaddr * server, int serverSize) {
	string message = "NAK";
	return (sendto(s, message.c_str(), PACKETSIZE, 0, server, serverSize)) >= 0; // 0 is flags
}

bool getFile(string fileName, int s, struct sockaddr * server, socklen_t * serverSize) {
	cout << "Waiting for file from the server." << endl;
	ofstream output;
	fileName = "OUTPUT-" + fileName;
	output.open(fileName.c_str());
	for(;;) {
		byte packet[PACKETSIZE];
		recvfrom(s, packet, PACKETSIZE, 0, server, serverSize); // 0 is flags
		cout << "Receiving packet!" << endl;
		if(packet[0] == '\0') break; // If the content is a null character, it is the end of the file

		// VALIDATE PACKET

		// OUTPUT CONTENT TO FILE
		output << packet;
	}
	output.close();
	cout << "Received final packet" << endl;
	return true;
}

// Function to close the socket
int closeSocket(int s) {
	close(s); // Don't forget to close the other end!
	return 1;
}

