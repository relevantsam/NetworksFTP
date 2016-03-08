
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

 int main() {
 	struct sockaddr_in client;
 	struct sockaddr_in server;
 	int sock = 0;
 	string ip = string("131.204.14.183"); // TUX183
 	int port = CLIENT_PORT_NUM;

 	// Let's make a socket..
 	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
 		cout << "Error creating socket. Quiting." << endl;
 		return 1;
 	} else {
 		cout << "Socket " << sock << " created!" << endl;
 	}

 	// Set up the client.
 	memset((char *)&client, 0, sizeof(client)); // removable? 
 	client.sin_family = AF_INET;
 	client.sin_addr.s_addr = htonl(INADDR_ANY);
 	client.sin_port = htons(port);

 	// And now let's bind our client to our socket
 	if(bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0){
 		cout << "Error binding socket to client. Quiting." << endl;
 		return 1;
 	} else {
 		cout << "Socket successfully bound to client." << endl;
 	}

 	// Set up the server.
 	memset((char *)&server, 0, sizeof(server));
 	server.sin_family = AF_INET;
 	server.sin_port = htons(10073);
 	inet_pton(AF_INET, ip.c_str(), &(server.sin_addr));

 	cout << "Bound IP address " << inet_ntoa(server.sin_addr) << " to the server" << endl << endl;
 	int i = 1;
 	for(;;) {
		 	cout << "Sending Message " << i << " to " << inet_ntoa(server.sin_addr) << endl;
		 	sendto(sock, "MESSAGE GOES HERE", 50, 0, (struct sockaddr *)&server, sizeof(server)); // 0 is flags
		 	sleep(2);
		 	i++;
		}

 }