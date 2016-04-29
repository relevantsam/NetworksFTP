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
 #include <sys/time.h>
 #include <cstdlib>
 // For now we will stay with std namespace
 using namespace std;

 int main(int argc, char *argv[]) {
	if(argc != 3)
	{
		cout << "Usage Error. Expected usage: <Program Name> <Probability of Packet Damage> (represented as an integer) <Probability of Packet Loss> (also as an integer" << endl;
		return 0;
	}

 	int sock, n, p;
 	struct sockaddr_in server;
 	struct sockaddr_in client;
 	socklen_t client_len = sizeof(client);

	int pD = atoi(argv[1]); //probability packet will be damaged
	int pL = atoi(argv[2]); //probability that a packet will be lost

 	byte buf[BUFFSIZE];
	ifstream file;

	struct packet packetBuffer[16] = NULL;

	struct timeval stTimeOut;

	fd_set stReadFDS;

	//set timeout for 20ms
	stTimeOut.tv_sec = 0;
	stTimeOut.tv_usec = 15000;

	sock = initSocket();
	if(sock < 0) return 0;

 	/*if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
 		cout << "Socket creation failed." << endl;
 		return 0;
 	} else {
 		cout << "Socket created! Socket " << socket << endl;
 	}*/

 	// Set up the server.
 	/*memset((char *)&server, 0, sizeof(server));
 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = htonl(INADDR_ANY);
 	server.sin_port = htons(SERVER_PORT);

 	cout << "Server allocated" << endl;*/


 	// Associate socket with server
 	/*if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
 		cout << "Socket failed to bind to server." << endl;
 		return 0;
 	} else {
 		cout << "Socket bound to server." << endl << endl;
 	}*/

	FD_SET(sock, &stReadFDS);

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
 	int seq = 0; // sequence is integer; need to add current sequence check.....
 	while(!file.eof()) {
		// Init packet buffer
		if(packetBuffer[15] == NULL)
		{
			initPacketBuffer(&file, &packetBuffer, &seq);
		}

		/*message.h.sequence = seq % 32;
		file.read((char *)message.data, BUFFSIZE - 1);
 		//message.data[BUFFSIZE] = '\0';
		message.h.checksum = checksumCal(message.data);
		cout << endl << "============================" 
			<< endl << "*        SEQUENCE " 
			<< message.h.sequence << "        *" 
			<< endl << "============================" << endl;
 		printf("Packet Message:  %.40s...\n", message.data);

		cout << "Packet Checksum: " << message.h.checksum << endl;

		cout << message.data << endl;*/

		sendPacketWindow(message, sock, (struct sockaddr *)&client, sizeof(client), pD, pL);

		//Modify to sendPacketWindow(message, sock, (struct sockaddr *)&client, sizeof(client, pD, pL, currseq, windowMax);
		//Add packet buffer perhaps

 		// Wait for ACK/NACK
		int t = select(-1, &stReadFDS, 0, 0, &stTimeOut);
		if(FD_ISSET(sock, &stReadFDS))
		{
			p = recvfrom(sock, buf, PACKETSIZE, 0, (struct sockaddr *)&client, &client_len);
			buf[p] = '\0';
			char * ACKmsg = strtok((char *)buf, " ");
			//acknum = something figure it out
			string ACKword = ACKmsg;
			if(ACKword == "ACK" && ACKnum == currseq + 1)
			{
				cout << "sequence " << message.h.sequence << " ACK\n"; //packet accepted, in order, slide window forward
				//currseq++
				shiftCongestionWindow(file, packetBuffer, seq)
			}

			else if(ACKword == "NACK")
			{
				cout << "sequence " << message.h.sequence << " NACK\n"; \\Corrupted packet, resend window
				cout << "resending...\n";
				sendPacket(message, sock, (struct sockaddr *)&client, sizeof(client), pD, pL);
			}

		}
		else if(t == 0)
		{
			cout << "sequence " << message.h.sequence << " timeout\n"; \\timeout resend window
			cout << "resending...\n";
			sendPacket(message, sock, (struct sockaddr *)&client, sizeof(client), pD, pL);
		}

 		for(int i = 0; i < BUFFSIZE; i++) message.data[i] = '\0'; // Empty the buffer. 
 		seq = seq + 1;
 	}
 	sendto(sock, "\0", 1, 0, (struct sockaddr *)&client, sizeof(client));
	file.close();
 	close(sock);
 	return 0;
 }

 /*
  * Initialize the socket and bind it to the server
  */
 int initSocket() {
 	/*
 	 * Declare variables
 	 */
 	// sd is an int representing socket or -1 if socket failed
 	int sd;
 	// Structure modeling the client machine
 	struct sockaddr_in server;

 	// Create the socket
	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	 		cout << "Socket creation failed." << endl;
	 		return -1;
 	} else {
 		cout << "Socket created! Socket " << socket << endl;
 	}



 	/* 
 	 * Fill server socket
 	 */
 	memset((char *)&server, 0, sizeof(server));
 	// AF_INET is internet protocol
 	server.sin_family = AF_INET;
 	// Assigns address to IP address of machine
 	server.sin_addr.s_addr = htonl(INADDR_ANY); // htonl takes long int from host to network byte order
 	// Assigns port number to port number specified in head file 
 	server.sin_port = htons(SERVER_PORT); // htons takes short int from host to network byte order
 	// AF_INET is internet protocol, SOCK_DGRAM is UDP, 0 selects protocol
 	// Associate socket with local machine
 	if(bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
 		cout << "Socket failed to bind to server." << endl;
 		return -1;
 	} else {
 		cout << "Socket bound to server." << endl << endl;
 	}
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

void sendPacketWindow(packet[] packetBuffer, int socket, struct sockaddr * client, int size, int pD, int pL) {
	for(int i = 0; i < 16; i++)
	{
		const char * seq = strcat("SEQ: ", packetBuffer[i].h.sequence);
		stringstream strs;
 		strs << message.h.checksum;
  		string temp_str = strs.str();
		char  const* checksum = temp_str.c_str();
		char message_str[PACKETSIZE + 1];
		strcpy(message_str, seq);
		strcat(message_str, "CHECKSUM:");
		strcat(message_str, checksum);
		strcat(message_str, "DATA:");
		strcat(message_str, (char *)message.data);
		message_str[PACKETSIZE] = '\0';

		if(gremlin(message, pD, pL))
		{
			sendto(socket, message_str, PACKETSIZE+1, 0, client, size); // Send the packet
		}
}

 void closeSocket(int sd) {
 	close(sd); // Need to close at other end as well
 }

bool gremlin(packet &message, int pD, int pL)
{
	srand(time(NULL));
	int chance = rand() % 100;

	if(chance < pL)
	{
		return false;
	}

	chance = rand() % 100;
	if(chance < pD)
	{
		chance = rand() % 100;
		if(chance < 70)
		{
			//modify 1 bit
			chance = rand() % (sizeof(message.data));
			message.data[chance]--;
		}

		else if(chance >= 70 && chance < 90)
		{
			//modify 2 bits
			chance = rand() % (sizeof(message.data));
			message.data[chance]--;
			chance = rand() % (sizeof(message.data));
			message.data[chance]--;
		}

		else if(chance >=90 && chance < 100)
		{
			//modify 3 bits
			chance = rand() % (sizeof(message.data));
			message.data[chance]--;
			chance = rand() % (sizeof(message.data));
			message.data[chance]--;
			chance = rand() % (sizeof(message.data));
			message.data[chance]--;
		}

		return true;
	}

	return true;
}

void initPacketBuffer(ifstream &file, packet &packetBuffer[])
{
	struct packet message;
	for(int i = 0; i < 16; i++)
	{
		message.h.sequence = i;
		file.read((char *)message.data, BUFFSIZE - 1);
		message.h.checksum = checksumCal(message.data);

		packetbuffer[i] = message;
	}
}

void shiftCongestionWindow(ifstream &file, packet &packetBuffer[], int &seq)
{
	struct packet message;

	for(int i = 0; i < 15; i++)
	{
		packetBuffer[i] = packetBuffer[i + 1];
	}

	seq = seq + 1;

	message.h.sequence = seq % 32;
	file.read((char *) message.data, BUFFSIZE - 1);
	message.h.checksum = checksumCal(message.data);

	packetBuffer[15] = message;
}	
