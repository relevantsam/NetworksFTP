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
 #include <sstream>
 // For now we will stay with std namespace
 using namespace std;

 int main(int argc, char *argv[]) {
 	int sock, n;
 	struct sockaddr_in server;
 	struct sockaddr_in client;
 	socklen_t client_len = sizeof(client);

 	byte buf[BUFFSIZE];
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
 	server.sin_port = htons(SERVER_PORT);

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
 		n = recvfrom(sock, buf, PACKETSIZE, 0, (struct sockaddr *)&client, &client_len);
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
			return 0;
		}
 	}

 	file.open(fileName.c_str(), ios::in);
 	if(!file.is_open()) {
 		cout << "File failed to open." << fileName << endl;
 		return 0;
 	} else {
 		cout << "Opened " << fileName << endl;
 	}
 	bool seq = false; // sequence is binary, 0 / 1, false / true
 	while(!file.eof()) {
		// Init our packet
		struct packet message;
		message.h.sequence = seq;
		file.read((char *)message.data, BUFFSIZE - 1);
 		//message.data[BUFFSIZE] = '\0';
		message.h.checksum = checksumCal(message.data);
		cout << endl << "============================" 
			<< endl << "*        SEQUENCE " 
			<< message.h.sequence << "        *" 
			<< endl << "============================" << endl;
 		printf("Packet Message:  %.40s...\n", message.data);

		cout << "Packet Checksum: " << message.h.checksum << endl;

		cout << message.data << endl;

		sendPacket(message, sock, (struct sockaddr *)&client, sizeof(client));

 		for(int i = 0; i < BUFFSIZE; i++) message.data[i] = '\0'; // Empty the buffer. 
 		seq = !seq;
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
 	server.sin_port = htons(SERVER_PORT); // htons takes short int from host to network byte order
 	// AF_INET is internet protocol, SOCK_DGRAM is UDP, 0 selects protocol
 	sd = socket(AF_INET, SOCK_DGRAM, 0);
 	// Associate socket with local machine
 	bind(sd, (struct sockaddr *)&server, sizeof(server));
 	return sd;
 }

 void runServer() {
 	
 }

// Function to calculate checksum of packet
int checksumCal(byte packet[]) {
	int checksum = 0;
	for(int i = 0; i < BUFFSIZE; i++) {
		checksum += (int) packet[i];
	}
	return checksum;
}

void sendPacket(packet message, int socket, struct sockaddr * client, int size) {
	const char * seq = "SEQ:0";
	stringstream strs;
 	strs << message.h.checksum;
  	string temp_str = strs.str();
	char  const* checksum = temp_str.c_str();
	if(message.h.sequence) seq = "SEQ:1";
	char message_str[PACKETSIZE + 1];
	strcpy(message_str, seq);
	strcat(message_str, "CHECKSUM:");
	strcat(message_str, checksum);
	strcat(message_str, "DATA:");
	strcat(message_str, (char *)message.data);
	message_str[PACKETSIZE] = '\0';

	sendto(socket, message_str, PACKETSIZE+1, 0, client, size); // Send the packet
}

 void closeSocket(int sd) {
 	close(sd); // Need to close at other end as well
 }
