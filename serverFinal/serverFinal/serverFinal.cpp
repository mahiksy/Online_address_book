#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable : 4996)
#define SERVER_PORT 1190
#define DEFAULT_BUFFER 512
#define MAX_CLIENTS 50
using namespace std;

char recvbuf[DEFAULT_BUFFER] = "";
int len, receiveres, clients_connected = 0;
SOCKET server_socket = INVALID_SOCKET;
SOCKET client_fd;
struct _clients_b {
	bool connected;
	SOCKET ss;
};
_clients_b clients[MAX_CLIENTS];

struct user
{
	string name;
	string password;
	string ID;
	int connectedNumber;
	bool connected = false;
};

int main() {
	// Define Begining Structures
	string combinedMsg;
	user list[4];
	list[0].name = "john";
	list[0].password = "john01";
	list[1].name = "root";
	list[1].password = "root01";
	list[2].name = "mary";
	list[2].password = "mary01";
	list[3].name = "david";
	list[3].password = "david01";
	string loggedArray[2][5];
	int i = 1;
	fstream file;
	ofstream temp;
	char buffer[50];
	char buf[4096];
	// Initialize WinSock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0) {
		cerr << "Can't initialize winsock\n";
		return 0;
	}
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		cerr << "Can't create a socket\n";
		WSACleanup();
	}
	// Hint Structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_addr.s_addr = INADDR_ANY;
	hint.sin_port = htons(SERVER_PORT);
	// Checking Connections
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&i, sizeof(i));
	wsOk = bind(server_socket, (sockaddr*)&hint, sizeof(hint));
	if (wsOk == SOCKET_ERROR) {
		cerr << ("Bind failed\n");
		closesocket(server_socket);
		WSACleanup();
	}
	wsOk = listen(server_socket, 5);
	unsigned long b = 1;
	ioctlsocket(server_socket, FIONBIO, &b);
	if (wsOk == SOCKET_ERROR) {
		cerr << ("Listening failed\n");
		closesocket(server_socket);
		WSACleanup();
	}
	cout << "Server Active\n";
	int count = 1000;
	bool active = TRUE;
	while (active) {
		// Connecting to Clients
		len = sizeof(hint);
		client_fd = accept(server_socket, (struct sockaddr*)&hint, &len);
		if (client_fd != INVALID_SOCKET) {
			clients[clients_connected].ss = client_fd;
			clients[clients_connected].connected = TRUE;
			clients_connected++;
			cout << "New client connected\n";
		}
		if (clients_connected > 0) {
			for (int cc = 0; cc < clients_connected; cc++) {
				memset(&recvbuf, 0, sizeof(recvbuf));
				if (clients[cc].connected) {
					receiveres = recv(clients[cc].ss, recvbuf, DEFAULT_BUFFER, 0);
					if (receiveres > 0) {
						string clientInput = recvbuf;
						if (clientInput != "") {
							cout << "Client " << clients[cc].ss << "> " << recvbuf << endl;
						}
						string firstWord, secondWord, thirdWord, fourthWord, fifthWord, line, firstWordLine, secondWordLine, thirdWordLine, fourthWordLine;
						stringstream ss, aa, bb;
						ss.str(clientInput);
						ss >> firstWord >> secondWord >> thirdWord >> fourthWord >> fifthWord;
						// Delete function
						if (firstWord == "DELETE")
						{
							bool checkifLOG = false;
							for (int i = 0; i < 4; i++)
							{
								if (list[i].connectedNumber == cc && list[i].connected == true)
								{
									checkifLOG = true;
								}
							}
							
							if (checkifLOG == true)
							{
								
								
									if (secondWord == "") {
										cout << "403 message format error\n";
										send(clients[cc].ss, "403 message format error", 25, 0);
									}
									else if (thirdWord > "" || fourthWord > "" || fifthWord > "") {
										cout << "403 message format error\n";
										send(clients[cc].ss, "403 message format error", 25, 0);
									}
									else if (secondWord.length() != 4) {
										cout << "403 message format error\n";
										send(clients[cc].ss, "403 message format error", 25, 0);
									}
									else {
										file.open("book.txt");
										int secondWordNumber = stoi(secondWord);
										if (secondWordNumber > 1000 && secondWordNumber <= count) {
											temp.open("temp.txt");
											while (getline(file, line)) {
												bb.str(line);
												bb >> firstWordLine;
												if (firstWordLine != secondWord) {
													temp << line << endl;
												}
											}
											cout << "200 OK\n";
											send(clients[cc].ss, "200 OK", 7, 0);
											file.close();
											temp.close();
											remove("book.txt");
											rename("temp.txt", "book.txt");
										}
										else {
											cout << "403 record ID does not exist\n";
											send(clients[cc].ss, "403 record ID does not exist", 29, 0);
										}
									}
							}
						}
						// Lookup Function
						else if (firstWord == "LOOKUP")
						{
							bool check = false;
							if (secondWord == "1" || secondWord == "2" || secondWord == "3") {
								check = true;
							}
							else if (check == false || thirdWord == "") {
								send(clients[cc].ss, "403 message format error 1", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else if (fourthWord > "") {
								cout << "403 message format error\n";
								send(clients[cc].ss, "403 message format error 2", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else {
								cout << "200 OK\n";
								send(clients[cc].ss, "200 OK", 7, 0);
								file.open("book.txt");
								cout << "Here are the address that match:\n";
								while (getline(file, line)) {
									bb.str(line);
									bb >> firstWordLine >> secondWordLine >> thirdWordLine >> fourthWordLine;
									if (secondWord == "1") {
										if (secondWordLine == thirdWord) {
											strcpy_s(buffer, line.c_str());
											send(clients[cc].ss, buffer, 50, 0);
											cout << line << endl;
										}
									}
									if (secondWord == "2") {
										if (thirdWordLine == thirdWord) {
											strcpy_s(buffer, line.c_str());
											send(clients[cc].ss, buffer, 50, 0);
											cout << line << endl;
										}
									}
									if (secondWord == "3") {
										if (fourthWordLine == thirdWord) {
											strcpy_s(buffer, line.c_str());
											send(clients[cc].ss, buffer, 50, 0);
											cout << line << endl;
										}
									}
								}
								send(clients[cc].ss, "", 1, 0);
								file.close();
							}
						}

						// Shutdown function
						else if (firstWord == "SHUTDOWN") {
							string name;
							if (secondWord > "") {
								cout << "403 message format error\n";
								send(clients[cc].ss, "403 message format error", 25, 0);
							}
							else {
								for (int i = 0; i < 4; i++) {
									if (list[i].connectedNumber == cc) {
										name = list[i].name;
									}
								}
								if (name == "root") {
									cout << "200 OK\n";
									send(clients[cc].ss, "200 OK", 7, 0);
									closesocket(server_socket);
									WSACleanup();
									return 0;
								}
								else {
									cout << "name = " << name;
									send(clients[cc].ss, "402 User not allowed to execute this command", 45, 0);
									send(clients[cc].ss, "", 1, 0);
								}
							}
						}
						// Add function
						else if (firstWord == "ADD") {
							if (secondWord == "" || thirdWord == "" || fourthWord == "") {
								cout << "403 message format error\n";
								send(clients[cc].ss, "403 message format error", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else if (fifthWord > "") {
								cout << "403 message format error\n";
								send(clients[cc].ss, "403 message format error", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else if (secondWord.length() > 8 || thirdWord.length() > 8 || fourthWord.length() != 12) {
								cout << "403 message format error\n";
								send(clients[cc].ss, "403 message format error", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else {
								file.open("book.txt", ios::app);
								count++;
								file << count << " " << secondWord << " " << thirdWord << " " << fourthWord << endl;
								string id = "the new record ID is ";
								aa << count;
								string countString = aa.str();
								id += countString;
								cout << "200 OK\n";
								send(clients[cc].ss, "200 OK", 7, 0);
								strcpy_s(buffer, id.c_str());
								cout << id << endl;
								send(clients[cc].ss, buffer, 50, 0);
								file.close();
							}
						}
						// Login function
						else if (firstWord == "LOGIN") 
						{
							if (secondWord == "" || thirdWord == "") {
								cout << "403 message format error\n";
								send(clients[cc].ss, "403 message format error", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else if (fourthWord > "") {
								cout << "403 message format error\n";
								send(clients[cc].ss, "403 message format error", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else {
								if (secondWord == "root" && thirdWord == "root01") {
									cout << "200 OK\n";
									send(clients[cc].ss, "200 OK", 7, 0);
								}
								else if (secondWord == "john" && thirdWord == "john01") {
									cout << "200 OK\n";
									send(clients[cc].ss, "200 OK", 7, 0);
								}
								else if (secondWord == "david" && thirdWord == "david01") {
									cout << "200 OK\n";
									send(clients[cc].ss, "200 OK", 7, 0);
								}
								else if (secondWord == "mary" && thirdWord == "mary01") {
									cout << "200 OK\n";
									send(clients[cc].ss, "200 OK", 7, 0);
								}
								else {
									cout << "410 Wrong UserID or Password";
									send(clients[cc].ss, "410 Wrong UserID or Password", 29, 0);
								}
								for (int i = 0; i < 4; i++) {
									if (list[i].name == secondWord) {
										list[i].connected = true;
										char* ip = inet_ntoa(hint.sin_addr);
										list[i].ID = ip;
										list[i].connectedNumber = cc;
									}
								}
								for (int cc = 0; cc < clients_connected; cc++) {
									cout << cc << endl;
								}

							}
						}
						// Logout function
						else if (firstWord == "LOGOUT")
						{
							string name;
							if (secondWord > "") {
								cout << "403 message format error";
								send(clients[cc].ss, "403 message format error", 25, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							else {
								
								for (int i = 0; i < 4; i++) {
									if (list[i].connectedNumber == cc) {
										name = list[i].name;
										cout << "200 OK\n";
										send(clients[cc].ss, "200 OK", 7, 0);
										list[i].connectedNumber == -1;
									}
								}
							}

						}
						// Who function
						else if (firstWord == "WHO")
						{
							int countWho = 0;
							cout << "200 OK" << endl;
							send(clients[cc].ss, "200 OK", 7, 0);
							cout << "the list of connected clients: " << endl;
							send(clients[cc].ss, "the list of connected clients:", 30, 0);
							for (int i = 0; i < 4; i++) {
								if (list[i].connected == true) {
									countWho = 1;
									combinedMsg = list[i].name + "       " + list[i].ID;
									cout << combinedMsg << endl;
									strcpy_s(buffer, combinedMsg.c_str());
									send(clients[cc].ss, buffer, 50, 0);
								}
							}
							if (countWho == 0) {
								cout << "No one is logged in" << endl;
								send(clients[cc].ss, "No one is logged in", 20, 0);
							}
							send(clients[cc].ss, "", 1, 0);

						}
						
						// List function
						else if (firstWord == "LIST") {
							file.open("book.txt");
							string str;
							getline(file, str);
							if (!str.empty()) {
								file.close();
								file.open("book.txt");
								cout << "200 OK\n";
								send(clients[cc].ss, "200 OK", 7, 0);
								cout << "The list of records in the book:\n";
								send(clients[cc].ss, "The list of records in the book:", 32, 0);
								string one, two, three, four, combo, space = "     ";
								while (file >> one >> two >> three >> four) {
									combo = one + space + two + space + three + space + four;
									strcpy_s(buffer, combo.c_str());
									cout << combo << endl;
									send(clients[cc].ss, buffer, 50, 0);

								}
								send(clients[cc].ss, "", 1, 0);
							}
							else {
								cout << "Empty List\n";
								send(clients[cc].ss, "Empty List", 10, 0);
								send(clients[cc].ss, "", 1, 0);
							}
							file.close();
						}
						
						// Quit function
						else if (firstWord == "QUIT") {
							cout << "200 OK\n";
							send(clients[cc].ss, "200 OK", 7, 0);
							cout << "Client disconnected." << endl;
							clients[cc].connected = FALSE;
							clients_connected--;
						}
						// Buffer to push the client side forward a step in multiple sends
						else if (firstWord == "") {
						}
						// Invalid argument, loops back to beginning
						else {
							cout << "400 invalid command\n";
							send(clients[cc].ss, "400 invalid command", 20, 0);
						}
					}
				}
			}
		}
	}
}