#ifndef PCH_H
#define PCH_H




#define GLM_ENABLE_EXPERIMENTAL
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtx/rotate_vector.hpp"
#include "vendor/glm/gtx/matrix_decompose.hpp"
#include "vendor/glm/gtx/string_cast.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"




#include <vector>
#include <memory>
#include <functional>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "opengl_debug.h"
#include <thread>
#include <chrono>
#include <random>



#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#endif
