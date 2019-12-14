#include "server.h"


Server::Server()
{
	
}

void Server::init_socket()
{
   /*
	* In case the program exited inadvertently on the last run,
	* remove the socket.
	*/

   unlink(SOCKET_NAME);

   /* Create local socket. */

   connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   if (connection_socket == -1) {
	   perror("socket");
	   exit(EXIT_FAILURE);
   }

   /*
	* For portability clear the whole structure, since some
	* implementations have additional (nonstandard) fields in
	* the structure.
	*/

   memset(&name, 0, sizeof(struct sockaddr_un));

   /* Bind socket to socket name. */

   name.sun_family = AF_UNIX;
   strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);	
   

   /* Bind socket to socket name. */

   name.sun_family = AF_UNIX;
   strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

   ret = bind(connection_socket, (const struct sockaddr *) &name,
			  sizeof(struct sockaddr_un));
   if (ret == -1) {
	   perror("bind");
	   exit(EXIT_FAILURE);
   } 

}

int Server::listen_to_socket()
{
     
   ret = listen(connection_socket, 20);

   data_socket = accept(connection_socket, NULL, NULL);
   if (data_socket == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
   }
   return ret;
}

void Server::update(){

   /* This is the main loop for handling connections. */
   /* Wait for incoming connection. */
   result = 0;

	  /* Wait for next data packet. */

   int res = read(data_socket, buffer, BUFFER_SIZE);

   if(res > 0 && buffer[0] != 0){
		 
      /* Ensure buffer is 0-terminated. */

      buffer[BUFFER_SIZE - 1] = 0;

      /* Handle commands. */
      if (!strncmp(buffer, "PLAY", BUFFER_SIZE)) 
      {
	    printf("received PLAY command\n");
	    current_command = PLAYER_CMD_PLAY;
	    is_command_active = true;
	    
      }else if (!strncmp(buffer, "STOP", BUFFER_SIZE)) 
      {
	    printf("received STOP command\n");
	    current_command = PLAYER_CMD_STOP;
	    is_command_active = true;
	    
      }

      if (!strncmp(buffer, "DOWN", BUFFER_SIZE)) 
      {
	    down_flag = 1;
      }

      if (!strncmp(buffer, "END", BUFFER_SIZE))
      {

      }

      /* Add received summand. */

      result += atoi(buffer);
      printf("%s\n", buffer);
      //~ sprintf(buffer, "%d", 0);
   }

   /* Send result. */

   //~ sprintf(buffer, "%d", result);
   ret = write(data_socket, buffer, BUFFER_SIZE);
   if (ret == -1) {
      printf("error sending result \n");
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      listen_to_socket();
   }

   // clear buffer ?


   /* Close socket. */
   //~ close(data_socket);


   /* Quit on DOWN command. */

   if (down_flag) {
    
   }	
}


