#include <iostream>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib") // tell compiler to link with winsock library

using namespace std; // why is it a bad idea?

int main()
	
{	
	string ipAddress = "127.0.0.1"; // ip adress of server
	int port = 54000; // listing post nr on server or where is it listening

	//Initialize Winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		cerr << "Can't start Winsock, ERR #" << wsResult << endl;
		return 0;
	}

	//Create a socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); // Af adress family / Inet internet style adress family
	if (sock == INVALID_SOCKET) {
		cerr << "Can't create socket, Err #" << WSAGetLastError << endl;
		WSACleanup();
		return 0;
	}

	// Fill in a hint structure
	sockaddr_in hint; // int at end because ipv4
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return 0;
	}

	//Do while loop to send and receive data
	char buf[4096];
	string userInput;

	do {
		// Prompt the user for text
		cout << "> ";
		getline(cin, userInput);
		if (userInput.size() > 0) { // Make sure the user has typed in something
			//Send the text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {
				//Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0) {
					//Echo response to console
					cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
				}
			}
			
		}
	} while (userInput.size() > 0);
	//Close down everything
	closesocket(sock);
	WSACleanup();
	return 0;
}


