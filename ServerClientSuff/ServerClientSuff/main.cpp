#include <iostream> 
#include <WS2tcpip.h> //tcp
#include <string>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")


using namespace std;

int main() {

	// Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);

	if (wsok != 0) {
		cerr << "Can't initialize winsock: Quitting" << endl;
		return 0;
	}

	//Create socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Can't create a socket! Gonna quit" << endl;
		return 0;
	}
	//Bind ip andress and port to socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)& hint, sizeof(hint));
	//Tell winsock the sock1et is for listening
	listen(listening, SOMAXCONN);
	
	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true) {
		fd_set copy = master;

		int socketCount = select(0,&copy, nullptr, nullptr, nullptr);
		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];
			if (sock == listening) {
				//accept connection
				SOCKET client = accept(listening, nullptr, nullptr);
				//Add new connection to list of connected clients
				FD_SET(client, &master);
				//Send a welcome message to client
				string hey = "Welcome to the server!\r\n";
				send(client, hey.c_str(), hey.size() + 1, 0);

			}
			else {
				char buf[4096];
				ZeroMemory(buf, 4096);
				//accept new message 
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					//drop client out
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else {
					//send message to other clients
					for (int i = 0; i < master.fd_count; i++) {
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock) {
							ostringstream ss;
							ss << "SOCKET #" << sock << ":" << buf << "\r\n";
							string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size()+1, 0);
						}
					}
				}


			}
		}
	}
	//Shutdown Winsock
	WSACleanup();

	return 0;
}



//// Wait for a conection
//sockaddr_in client;
//int clientSize = sizeof(client);

//SOCKET clientSocket = accept(listening, (sockaddr*)& client, &clientSize);
//if (clientSocket == INVALID_SOCKET) {
//	cerr << "Invalid client Socket! Piece out" << endl;
//	return 0;
//}

//char host[NI_MAXHOST];
//char service[NI_MAXHOST];
//ZeroMemory(host, NI_MAXHOST);
//ZeroMemory(service, NI_MAXHOST);

//if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
//	cout << host << "connected on port " << endl;
//}
//else {
//	inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
//	cout << host << "connected on port"
//		<< ntohs(client.sin_port) << endl;
//}


//// Close listening socket
//closesocket(listening);

////While loop :accept and echo message back to client
//char buf[4096];
//while (true) {
//	ZeroMemory(buf, 4096);
//	//wait for client data
//	int bytesReceived = recv(clientSocket, buf, 4096, 0);
//	if (bytesReceived == SOCKET_ERROR) {
//		cerr << "Error in recv(). Quiting" << endl;
//		break;
//	}

//	if (bytesReceived == 0) {
//		cout << "Client disconnected" << endl;
//		break;
//	}

//// ECho
//	send(clientSocket, buf, bytesReceived + 1, 0);
//	
