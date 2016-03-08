/*
 * FTP SERVER
 * 
 * Functionality: 
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
 #include "server.h"
 // Other incldues
 #include <iostream>
 #include <cstring>
 #include <fstream>
 // For now we will stay with std namespace
 using namespace std;

 int main(int argc, char *argv[]) {
 	int sock, n;
 	struct sockaddr_in server;
 	struct sockaddr_in client;
 	socklen_t client_len = sizeof(client);

 	byte buf[240];
	ifstream file;

 	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
 		cout << "Socket creation failed." << endl;
 		return 0;
 	} else {
 		cout << "Socket created! Socket " << socket << endl;
 	}

 	// Set up the server.
 	memset((char *)&server, 0, sizeof(server));
 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = htonl(INADDR_ANY);
 	server.sin_port = htons(SERVER_PORT_NUM);

 	cout << "Server allocated" << endl;


 	// Associate socket with server
 	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
 		cout << "Socket failed to bind to server." << endl;
 		return 0;
 	} else {
 		cout << "Socket bound to server." << endl << endl;
 	}
 	
 	cout << "============================" << endl << "+      SERVER RUNNING      +" << endl << "============================" << endl;
 	string fileName;
 	for(;;) {
 		n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&client, &client_len);
 		buf[n] = '\0';
 		char * msg = strtok((char *)buf, " ");
		string word = msg;
		if(word == "GET") {
			cout << "GET REQUEST RECEIVED" << endl;
 			msg = strtok(NULL, " ");
 			fileName = msg;
 			cout << "Request for " << fileName << endl;
 			break;
		} else {
			cout << "Request rejected." << endl;
		}
 	}

 	file.open(fileName.c_str(), ios::in);
 	if(!file.is_open()) {
 		cout << "File failed to open." << fileName << endl;
 		return 0;
 	} else {
 		cout << "File opened" << endl;
 	}
 	while(!file.eof()) {
 		cout << "Reading line: "<< endl;
		file.read((char *)buf, sizeof(buf) - 1);
 		buf[n] = '\0';
 		printf("%.40s\n", buf);
 		sendto(sock, buf, 256, 0, (struct sockaddr *)&client, sizeof(client)); // Send the buffer
 		for(int i = 0; i < sizeof(buf); i++) buf[i] = '\0'; // Empty the buffer. 
 	}
 	sendto(sock, "\0", 1, 0, (struct sockaddr *)&client, sizeof(client));
	file.close();
 	close(sock);
 	return 0;
 }

 int initSocket() {
 	/*
 	 * Declare variables
 	 */
 	// sd is an int representing socket or -1 if socket failed
 	int sd;
 	// Structure modeling the client machine
 	struct sockaddr_in server;

 	/* 
 	 * Fill server socket
 	 */
 	// AF_INET is internet protocol
 	server.sin_family = AF_INET;
 	// Assigns address to IP address of machine
 	server.sin_addr.s_addr = htonl(INADDR_ANY); // htonl takes long int from host to network byte order
 	// Assigns port number to port number specified in head file 
 	server.sin_port = htons(SERVER_PORT_NUM); // htons takes short int from host to network byte order
 	// AF_INET is internet protocol, SOCK_DGRAM is UDP, 0 selects protocol
 	sd = socket(AF_INET, SOCK_DGRAM, 0);
 	// Associate socket with local machine
 	bind(sd, (struct sockaddr *)&server, sizeof(server));
 	return sd;
 }

 void runServer() {
 	
 }

 void closeSocket(int sd) {
 	close(sd); // Need to close at other end as well
 }
