#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <vector>
#include "../stb_image.h"

using namespace std;

struct GUIImage {
public: 
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;
};
class GUI {
public:
	static vector<GUIImage> images_list;
	static void initializeGUI(GLFWwindow* window);
	static bool renderUI(bool show_GUI);
	static void cleanUp();
	static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	static void initializeImage(char* dir);
};


