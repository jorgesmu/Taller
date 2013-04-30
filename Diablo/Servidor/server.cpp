#include "serversocket.h"

#include <Windows.h>
#include <process.h>

#include <iostream>
#include <string>

#include "wcomm.h"
#include "serversocket.h"

using namespace std;

int chat() {
	
	ServerSocket sock;
	// Init
	if(!sock.init())
		return 1;

	// Bind
	if(!sock.listen(8080))
		return 1;

	// Aceptar conexiones indefinidamente
	while(sock.accept()) {
		//
	}

	// Close
	sock.close();
	return 0;
}

int main() {
	//chat();
/*
	WComm w;
	// Start Server Daemon
	w.startServer(27016);
	printf("Server Started........\n");

    while (TRUE) {
        
		// Wait until a client connects
		w.waitForClient();
		printf("Client Connected......\n");

		// Work with client
		while(TRUE)
		{
			char rec[50] = "";
			w.recvData(rec,32);w.sendData("OK");

			if(strcmp(rec,"FileSend")==0)
			{
				char fname[32] ="";
				w.fileReceive(fname);
				printf("File Received.........\n");
			}

			
			if(strcmp(rec,"EndConnection")==0)break;
			printf("Connection Ended......\n");

		}
		
		// Disconnect client
		w.closeConnection();
	}
	*/
	ServerSocket s;
	// Start Server Daemon
	s.init();
	s.listen(27016);
	printf("Server Started........\n");

    while (TRUE) {
        
		// Wait until a client connects
		s.accept();
		printf("Client Connected......\n");

		// Work with client
		while(TRUE)
		{
			string rec;
			s.receive("1",rec);
			s.send("1","OK");

			if(strcmp(rec.c_str(),"FileSend")==0)
			{
				char fname[32] ="";
				s.fileReceive(fname);
				printf("File Received.........\n");
			}

			
			if(strcmp(rec.c_str(),"EndConnection")==0)break;
			printf("Connection Ended......\n");

		}
		
		// Disconnect client
		s.close();
	}
	return 0;
}