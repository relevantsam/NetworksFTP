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

 int main(int argc, char *argv[]) {
	if (argc != NUM_ARGS) {
		cout << "Usage Error. Expected usage: <Program Name> <Server IP Address> <File Name>" << endl;
		return 0;
	}

	int s; // store the socket id
	string serverIP = argv[1]; // The server ip
	string fileName = argv[2];
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

 	// Send a get request
 	string message = "GET " + fileName;
	if(!sendGet(s, message, (struct sockaddr *)&server, sizeof(server))) {
		cout << "Sending GET request failed. Quiting." << endl;
	}

	// Get the file

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
bool sendGet(int s, string message, struct sockaddr * server, int serverSize) {
	return (sendto(s, message.c_str(), BUFFSIZE, 0, server, serverSize)) >= 0; // 0 is flags
}

// Function to close the socket
int closeSocket(int s) {
	close(s); // Don't forget to close the other end!
	return 1;
}

