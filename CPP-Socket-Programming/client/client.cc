// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>

#include "../server/License.pb.h"

using namespace std;
using namespace license;

#define PORT 8484

int main(int argc, char const* argv[])
{
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
			<= 0) {
		printf(
				"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((client_fd
				= connect(sock, (struct sockaddr*)&serv_addr,
					sizeof(serv_addr)))
			< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	License lic;
	string str;
	while(1)
	{
		str = to_string((long long int)(rand() % 2));
		send(sock, str.c_str(), str.size(), 0);
		memset(buffer, 0, 1024);
		valread = read(sock, buffer, 1024);

		lic.ParseFromString(string(buffer));
		cout << "License from server: " << endl << lic.DebugString() << endl;
		sleep(2);
	}
	fstream output("lic.ser",ios::out | ios::trunc | ios::binary);

	if(!lic.SerializeToOstream(&output))
	{
		cout << "License object can't be serialized" << endl;
	}
	else
	{
		cout << "License object serialized successfully" << endl;
	}
	
	// closing the connected socket
	close(client_fd);
	return 0;
}
