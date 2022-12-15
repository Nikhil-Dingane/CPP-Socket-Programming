#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <sys/time.h>

#include "License.pb.h"

using namespace std;
using namespace license;

#define PORT 8484
#define MAX_CLIENTS 50

int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread, activity, sd, max_sd;
	int client_socket[MAX_CLIENTS] = {0};
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char* hello = "Hello from server";
	fd_set readfds;

	// Creating socket file descriptor
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

	// Forcefully attaching socket to the port 8484
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8484
	if (bind(server_fd, (struct sockaddr*)&address,
				sizeof(address))
			< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
//	cout << "Waiting for client" << endl;
//	if ((new_socket
//				= accept(server_fd, (struct sockaddr*)&address,
//					(socklen_t*)&addrlen))
//			< 0) {
//		perror("accept");
//		exit(EXIT_FAILURE);
//	}
//	cout << "client connected" << endl;

	license::License lic;
	lic.set_global_nodes(10);
	lic.set_node_sessions(10000);
	lic.set_node_session_rate(1000);

	fstream output("lic.ser", ios::out | ios::trunc | ios::binary);

	if (!lic.SerializeToOstream(&output))
	{
		cerr << "Failed to write address book." << endl;
		return -1;
	}
	else
	{
		cout << "license object has been successfully serialized." << endl;
	}

	stringstream ss;
	ifstream licFile("lic.ser");
	string line;
	if(licFile.is_open())
	{
		cout << "file is opened" << endl;
		while(licFile.good())
		{
			cout << "Reading the file" << endl;
			getline(licFile, line);
			ss << line << endl;
		}
	}
	else
	{
		cout << "unable to open the file" << endl;
	}

	cout << "file: " << ss.str() << endl;
	string msg;
	lic.SerializeToString(&msg);
	string msgFromClient;
	string msgForClient;
//	while(1)
//	{
//		memset(buffer, 0, 1024);
//		valread = read(new_socket, buffer, 1024);
//		cout << "Message from client: " << endl << string(buffer) << endl;
//		send(new_socket, msg.c_str(), msg.size(), 0);
//	}
	while(1)
	{
		FD_ZERO(&readfds);

		FD_SET(server_fd, &readfds);
		max_sd = server_fd;

		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			sd = client_socket[i];	

			if(sd > 0)
			{
				FD_SET(sd, &readfds);
			}

			if(sd > max_sd)
			{
				max_sd = sd;
			}
		}

		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if((activity < 0) && (errno != EINTR))
		{
			cout << "select error" << endl;
		}

		if(FD_ISSET(server_fd, &readfds))
		{
			if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			cout << "connected" << endl;
			send(new_socket, msg.c_str(), msg.size(), 0);

			for(int i = 0; i < MAX_CLIENTS; i++)
			{
				if(client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					break;
				}
			}
		}
		
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			sd = client_socket[i];

			if(FD_ISSET(sd, &readfds))
			{
				memset(buffer, 0, 1024);
				if((valread = read(new_socket, buffer, 1024)) == 0)
				{
					getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
					cout << "Host disconnected" << endl;
					close(sd);
					client_socket[i] = 0;
				}
				else
				{
					send(new_socket, msg.c_str(), msg.size(), 0);
				}
			}
		}
	}

	// closing the connected socket
	close(new_socket);
	// closing the listening socket
	shutdown(server_fd, SHUT_RDWR);

	License tempLic;
	tempLic.ParseFromString(msg);
	cout << "Message from string: " << tempLic.DebugString() << endl;

	return 0;
}
