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
 // For now we will stay with std namespace
 using namespace std;

 int main(int argc, char *argv[]) {
 	int sock, n;
 	struct sockaddr_in server;
 	char buf[512];

	server.sin_family = AF_INET;
 	// Assigns address to IP address of machine
 	server.sin_addr.s_addr = htonl(INADDR_ANY); // htonl takes long int from host to network byte order
 	// Assigns port number to port number specified in head file 
 	server.sin_port = htons(10072); // htons takes short int from host to network byte order

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


 	// Associate socket with server
 	if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
 		cout << "Socket failed to bind to server." << endl;
 		return 0;
 	} else {
 		cout << "Socket bound to server." << endl << endl;
 	}
 	//cout << "SERVER RUNNING" << endl;
 	int i = 1;
 	for(;;) {
 		n = recv(sock, buf, sizeof(buf), 0);
 		buf[n] = '\0';
 		cout << "Message " << i << ": " << buf << endl;
 		i++;
 	}
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