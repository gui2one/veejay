#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "connection.h"
#include <chrono>
#include <thread>

class Server
{
	public:
		Server();
		void init_socket();
		int listen_to_socket();
		void update();
			
		struct sockaddr_un name;
		int down_flag = 0;
		int ret;
		int connection_socket;
		int data_socket;
		int result;
		char buffer[BUFFER_SIZE];		

		PLAYER_CMD current_command = PLAYER_CMD_NULL;
		bool is_command_active = false;
	   	
	private:
		/* add your private declarations */
};

#endif /* SERVER_H */ 
