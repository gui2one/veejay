#ifndef CLIENT_H
#define CLIENT_H
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "connection.h"

class Client
{
	public:
		Client();
		void init_socket();
		int connect_to_socket();
		void send_message(const char* message, int& res);

	
	   struct sockaddr_un addr;
	   int i;
	   int ret;
	   int data_socket;
	   char buffer[BUFFER_SIZE];	
	   
	private:	   
		/* add your private declarations */
};

#endif /* CLIENT_H */ 
