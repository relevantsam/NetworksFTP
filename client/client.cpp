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
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 // Other incldues
 #include <iostream>

 int main() {
 	std::cout << "Hello World";
 }

 