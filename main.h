#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
//#include <GL/glew.h>
#include "glm.h" //TODO super messy...
#endif
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include "Window.h"
#include "InputManager.h"
#include "GameManager.h"
#include "util.h"

#endif
