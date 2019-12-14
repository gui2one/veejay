#include "client.h"


Client::Client()
{
	init_socket();
}
void Client::init_socket()
{
   /* Create local socket. */

   data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
   if (data_socket == -1) {
	   perror("socket");
	   exit(EXIT_FAILURE);
   }

   /*
	* For portability clear the whole structure, since some
	* implementations have additional (nonstandard) fields in
	* the structure.
	*/

   memset(&addr, 0, sizeof(struct sockaddr_un));

   /* Connect socket to socket address */

   addr.sun_family = AF_UNIX;
   strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);	
}

int Client::connect_to_socket()
{
	   ret = connect (data_socket, (const struct sockaddr *) &addr,
				  sizeof(struct sockaddr_un));
				  
	  return ret;
}

void Client::send_message(const char * message, int& res)
{
	//~ init_socket();
	connect_to_socket();
	ret = write(data_socket, message, strlen(message) + 1);	
	res = ret;
	//~ close(data_socket);

}


