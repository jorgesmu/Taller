#include "clientsocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>

#include "wcomm.h"
#include "clientsocket.h"

#pragma comment(lib, "Ws2_32.lib")
	
using namespace std;

int chat() {
	ClientSocket sock;
	
	if(!sock.init()) 
		return 1;
	
	if(!sock.connect("127.0.0.1", 8080))
		return 1;
	
	// Creamos el thread de listen
	HANDLE hth1 = (HANDLE)_beginthreadex(NULL, 0, ClientSocket::listenEntry, (void*)&sock, 0, NULL);
	// Hacemos el input en el main thread
	std::string line;
	while(std::getline(std::cin, line)) {
		if(!sock.isOpen() || !sock.send(line)) 
			break;
	}

	sock.close();
	return 0;
}

int main() {
	//chat();

	
	WComm w;
	char rec[32] = "";
	// Connect To Server
	w.connectServer("127.0.0.1",27016);
	printf("Connected to server...\n");

	// Sending File
	w.sendData("FileSend");	w.recvData(rec,32);
	w.fileSend("Files/test.JPG");
	printf("File Sent.............\n");

	// Send Close Connection Signal
	w.sendData("EndConnection");w.recvData(rec,32);
	printf("Connection ended......\n");
	getchar();
	
	/*
	ClientSocket s;
	string rec;
	// Connect To Server
	s.connect("127.0.0.1",27016);
	printf("Connected to server...\n");

	// Sending File
	s.send("FileSend");
	s.receive(rec);
	s.sendFile("Files/test.JPG");
	printf("File Sent.............\n");

	// Send Close Connection Signal
	s.send("EndConnection");
	s.receive(rec);
	printf("Connection ended......\n")
	getchar();
	*/
	return 0;
}