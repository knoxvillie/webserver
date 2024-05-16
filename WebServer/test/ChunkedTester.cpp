/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkedTester.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/16 12:41:52 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/16 15:25:52 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream> 
#include <cerrno>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

using namespace std;

const std::string getSizeInHex(const std::string& str) {
    std::stringstream ss;
    ss << std::hex << str.size(); // Convert size to hexadecimal
    std::string hexSize;
    ss >> hexSize;
    return hexSize;
}

int main(int argc, char** argv) {
	
	string address = "0.0.0.0"; // Server address
	int port = 8080; // Server port

	string filename = "/upload"; // Name of the file to request to the server

	const char *def[3] = { "default ", "request", NULL }; // Default request if none is provided

	struct sockaddr_in server_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); // Server port
    server_addr.sin_addr.s_addr = inet_addr(address.c_str());

	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		cerr << ANSI_COLOR_RED << "ERROR: couldn't connect to server on " << address << ":" << port << " due to \'" << strerror(errno) << "\'" << ANSI_COLOR_RESET << endl;
		exit(1);
	}

	cout << ANSI_COLOR_GREEN << "Connect to server " << address << ":" << port << "!\nSending chunked request...\n" << ANSI_COLOR_RESET << endl;

	stringstream header;

	header	<< "POST " << filename << " HTTP/1.1" << "\r\n"
			<< "Transfer-Encoding: chunked\r\n"
			<< "Accept: */*\r\n"
			<< "Content-Type: text/plain\r\n"
			<< "User-Agent: YourMom\r\n"
			<< "\r\n";

	string output = header.str();
	if (send(sockfd, output.c_str(), output.size(), MSG_DONTWAIT) < 0) {
		cerr << ANSI_COLOR_RED << "ERROR: sending to server " << address << ":" << port << " failed because of \'" << strerror(errno) << "\'" << ANSI_COLOR_RESET << endl;
		close(sockfd);
		exit(1);
	}
	
	if (argc < 2) {
		cout << ANSI_COLOR_YELLOW << "WARNING: no arguments, sending \"default request\"" << ANSI_COLOR_RESET << endl;
		
		for (int i = 0; def[i] != NULL; i++) {
			output = getSizeInHex(def[i]) + "\r\n" + def[i] + "\r\n";
			cout << "Line: " << output;
			cout << ANSI_COLOR_GREEN << "Chunk sent!" << ANSI_COLOR_RESET << endl;
			if (send(sockfd, output.c_str(), output.size(), 0) < 0) {
				cerr << ANSI_COLOR_RED << "ERROR: sending to server " << address << ":" << port << " failed because of \'" << strerror(errno) << "\'" << ANSI_COLOR_RESET << endl;
				close(sockfd);
				exit(1);
			}
		}
	}

	for (int i = 1; i < argc; i++) {

		output = getSizeInHex(argv[i]) + "\r\n"; // chunk size
		output += argv[i];	// chunk data
		output += "\r\n";

		if (send(sockfd, output.c_str(), output.size(), 0) < 0) {
			cerr << ANSI_COLOR_RED << "ERROR: sending to server " << address << ":" << port << " failed because of \'" << strerror(errno) << "\'" << ANSI_COLOR_RESET << endl;
			close(sockfd);
			exit(1);
		}
		cout << "Line: " << output;
		cout << ANSI_COLOR_GREEN << "Chunk sent!" << ANSI_COLOR_RESET << endl;
	}

	// Sent one empty chunk to signal the end of this request
	send(sockfd, "0\r\n", 3, 0);


	char buf[4096] = {0};
	recv(sockfd, buf, 4096, 0);
	cout << "\n\n" << ANSI_COLOR_GREEN << "Response received: \n==================\n" << ANSI_COLOR_RESET
		 << buf << ANSI_COLOR_GREEN << "\n==================" << ANSI_COLOR_RESET << endl;

	close(sockfd);
	return 0;
}