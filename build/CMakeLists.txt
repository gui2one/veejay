CMake_minimum_required(VERSION 2.8)

# -fcompare-debug-second to remove simplejson ugly notes when building
set(CMAKE_CXX_FLAGS "-std=c++17 -fcompare-debug-second -Wall -Wno-unused-function -Wno-unused-but-set-variable -Wno-multistatement-macros")

project(VeeJeh)

include_directories(
	/usr/include
	../src/vendor/imgui
	../src/vendor
)

link_directories(
	/usr/local/lib
	/home/pi/projects/veejay/src/libs
)
add_library(PCHLib
	../src/pch.cpp
	
	../src/vendor/imgui/imgui.cpp
	../src/vendor/imgui/imgui_draw.cpp
	../src/vendor/imgui/imgui_widgets.cpp
	../src/vendor/imgui/imgui_impl_glfw.cpp
	../src/vendor/imgui/imgui_impl_opengl3.cpp	
	
	../src/vendor/FastNoise/FastNoise.cpp
)

add_library(CoreLib
	../src/core.cpp
	
	../src/core/texture.cpp
	../src/core/shader.cpp
	../src/core/particle_system.cpp
	
	../src/core/modules/module.cpp
	../src/core/modules/orbiter.cpp
	../src/core/modules/circles.cpp
	../src/core/modules/image.cpp
	../src/core/modules/particles.cpp
	
	
	../src/core/timer.cpp
	#~ ../src/core/log.cpp
	
	
	../src/core/renderer.cpp

)

add_executable(vj_editor
	../src/main.cpp
	../src/actions.cpp		
	../src/ipc/client.cpp	
	
	
	../src/vendor/SimpleJSON/json.cpp
	../src/jsonfilewriter.cpp		
	../src/jsonfilereader.cpp		
	
	../src/wave_file_reader.cpp
	../src/fft.cpp
	
)

target_link_libraries( vj_editor #PRIVATE spdlog
	
	PCHLib
	CoreLib
	-lglfw
	-lGL
	-lGLU
	-lGLEW
	-lpthread
	-L/usr/local/lib -lportaudio -lasound -lm -lpthread
	-lsndfile
	-lfftw3 -lm
	
)


add_executable(vj_player
	../src/player.cpp
	../src/ipc/server.cpp	
)

target_link_libraries( vj_player 
	PCHLib
	CoreLib
	-lfreetype
	-lglfw
	-lGL
	-lGLU
	-lGLEW
	-lpthread
	
)
