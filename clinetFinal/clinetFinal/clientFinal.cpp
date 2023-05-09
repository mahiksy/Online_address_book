#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>	
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#define DEFAULT_BUFFER 512
#define SERVER_PORT "1190"
using namespace std;

int __cdecl main(int argc, char** argv) {
	// Defining Begining Sturctures
	char buf[4096];
	char buffer[50];
	bool loggedIN = false;
	WSADATA wsData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL, * ptr = NULL, hint;
	char sendbuf[DEFAULT_BUFFER] = "";
	string userInput;
	char recvbuf[DEFAULT_BUFFER] = "";
	int recvbuflen = DEFAULT_BUFFER;
	// Initialize Winsock
	int count2 = 5;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	while (wsResult != 0) {
		cerr << "Can't start Winsock, trying again. " << count2 << " tries left." << endl;
		Sleep(1000);
		wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);
		if (wsResult == 0) {
			break;
		}
		count2--;
	}
	if (wsResult != 0) {
		cerr << "Can't Initialize winsock." << endl;
		return 0;
	}
	// Fill in a hint structure
	ZeroMemory(&hint, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;
	// Gather address info
	wsResult = getaddrinfo(argv[1], SERVER_PORT, &hint, &result);
	if (wsResult != 0) {
		cerr << "Can't get address." << endl;
		WSACleanup();
	}
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// Connecting socket
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		wsResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (wsResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		cerr << "Unable to connect to server\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 0;
	}
	// Main Driver
	bool cont = true;
	while (cont == true) {
		cout << "\nPlease follow the directions for each option:" << endl;
		if (loggedIN == false) {
			cout << "LOGIN -> type: LOGIN Username Password" << endl;
			cout << "WHO -> type: WHO" << endl;
			cout << "LIST -> type: LIST" << endl;
			cout << "LOOKUP -> type: LOOKUP Number (1, 2, or 3)" << endl;
		}
		if (loggedIN == true) {
			cout << "LOGOUT -> type: LOGOUT" << endl;
			cout << "WHO -> type: WHO" << endl;
			cout << "ADD -> type: ADD FirstName LastName PhoneNumber" << endl;
			cout << "DELETE -> type: DELETE IDNumber" << endl;
			cout << "LIST -> type: LIST" << endl;
			cout << "LOOKUP -> type: LOOKUP Number (1, 2, or 3)" << endl;
			cout << "SHUTDOWN -> type: SHUTDOWN" << endl;
		}
		cout << "QUIT -> type: QUIT\n" << endl;
		cout << ">";
		getline(cin, userInput);
		// Taking user info
		stringstream ss;
		ss.str(userInput);
		string first_word, second_word, loggedName;
		ss >> first_word >> second_word;
		// Send an initial buffer
		wsResult = send(ConnectSocket, userInput.c_str(), (int)strlen(userInput.c_str()), 0);
		if (wsResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
		}
		ZeroMemory(buf, 4096);
		int ConfReceived;
		int bytesReceived = recv(ConnectSocket, buf, 4096, 0);
		if (bytesReceived > 0) {
			//receiving the 200 OK message!
			cout << string(buf, 0, bytesReceived) << endl;

			if (first_word == "LOGIN" && string(buf, 0, bytesReceived) == "200 OK") {
				loggedIN = true;
				loggedName = second_word;

			}
			if (first_word == "LOGOUT") {
				if (string(buf, 0, bytesReceived) == "200 OK") {
					loggedIN = false;
				}
			}
			if (first_word == "QUIT") {
				cont = false;
				loggedIN = false;
				wsResult = shutdown(ConnectSocket, SD_SEND);
				closesocket(ConnectSocket);
				WSACleanup();
				return 0;
			}
			
			if (first_word == "WHO") {
				int sendConf = send(ConnectSocket, "", 1, 0);
				if (sendConf != SOCKET_ERROR) {
					ConfReceived = recv(ConnectSocket, buf, 4096, 0);
					if (bytesReceived > 0) {
						cout << string(buf, 0, ConfReceived) << endl;
					}
				}
				bool loop = true;
				while (loop == true && (string(buf, 0, ConfReceived) != "")) {
					send(ConnectSocket, "", 1, 0);
					ConfReceived = recv(ConnectSocket, buf, 50, 0);
					if (string(buf, 0, ConfReceived) != "") {
						cout << string(buf, 0, ConfReceived) << endl;

					}
					if (string(buf, 0, ConfReceived) == "") {
						loop = false;
						cout << "";
					}
				}
			}
			if (first_word == "LIST") {
				int sendConf = send(ConnectSocket, "", 1, 0);
				if (sendConf != SOCKET_ERROR) {
					ConfReceived = recv(ConnectSocket, buf, 4096, 0);
					if (bytesReceived > 0) {
						cout << string(buf, 0, ConfReceived) << endl;
					}
				}
				bool loop = true;
				while (loop == true && (string(buf, 0, ConfReceived) != "")) {
					send(ConnectSocket, "", 1, 0);
					ConfReceived = recv(ConnectSocket, buf, 50, 0);
					if (string(buf, 0, ConfReceived) != "") {
						cout << string(buf, 0, ConfReceived) << endl;

					}
					if (string(buf, 0, ConfReceived) == "") {
						loop = false;
					}
				}
				send(ConnectSocket, "", 1, 0);
			}
			if (loggedIN == true)
			{
				if (first_word == "LOOKUP") {
					cout << "Here are the address that match:\n";
					int sendConf1 = send(ConnectSocket, "", 1, 0);
					if (sendConf1 != SOCKET_ERROR) {
						ConfReceived = recv(ConnectSocket, buf, 4096, 0);
						if (bytesReceived > 0) {
							cout << string(buf, 0, ConfReceived) << endl;
						}
					}
					bool loop = true;
					while (loop == true && (string(buf, 0, ConfReceived) != "")) {
						send(ConnectSocket, "", 1, 0);
						ConfReceived = recv(ConnectSocket, buf, 50, 0);
						if (string(buf, 0, ConfReceived) != "") {
							cout << string(buf, 0, ConfReceived) << endl;
						}
						if (string(buf, 0, ConfReceived) == "") {
							loop = false;
						}
					}
				}
				// 

				// the only time that user can have access to add and delete is when user has been logged in.

				if (first_word == "SHUTDOWN") {
					cout << loggedName << endl;
					if (string(buf, 0, bytesReceived) == "200 OK") {
						cout << "running";
						cont = false;
						loggedIN = false;
						wsResult = shutdown(ConnectSocket, SD_SEND);
						closesocket(ConnectSocket);
						WSACleanup();
						return 0;
					}
				}
				if (first_word == "ADD") {
					if (first_word == "ADD") {
						int sendConf = send(ConnectSocket, "", 1, 0);
						if (sendConf != SOCKET_ERROR) {
							int ConfReceived = recv(ConnectSocket, buf, 4096, 0);
							if (bytesReceived > 0) {
								cout << string(buf, 0, ConfReceived) << endl;
							}
						}
					}
				}
			}
			if (loggedIN == false && first_word!= "LOGOUT")
			{
				cout << "Error 402! Cannot access this command!" << endl;
			}
		}
	}
}