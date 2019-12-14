#include "pch.h"

#include "ipc/server.h"
#include "core.h"
#include <thread>

Server server;
Renderer renderer;

static void execute_command(PLAYER_CMD cmd)
{
	if( cmd == PLAYER_CMD_PLAY)
	{
		printf("PLAY command executed\n");
	}else if( cmd == PLAYER_CMD_STOP)
	{
		printf("STOP command executed\n");
	}
}

void threaded_function()
{
	server.init_socket();
	server.listen_to_socket();

		while(1)
		{
			server.update();
			if( server.is_command_active == true){
				execute_command(server.current_command);
				server.is_command_active = false;
				
			}
		}
}

GLFWwindow * window;
int w_width = 500;
int w_height = 500;


int main(int argc, char** argv)
{

	printf("Player App \n");	
	
	std::thread t(threaded_function);
	
	if( !glfwInit()){
		printf("glfw init error\n");
		return -1;
	}
	
	glewInit();
	
	window = glfwCreateWindow(w_width, w_height,"VJ Player", NULL, NULL);	
	
	glfwMakeContextCurrent(window);
	
	while(!glfwWindowShouldClose(window))
	{
		//~ printf("player running !!!!\n");
		//~ std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		
		
		GLCall(glClearColor(0.0,0.0,0.5,1.0));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	exit(0);
	return 0;
}
