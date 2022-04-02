#include "Window.h"


// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "GLFW Starter Project";

// Objects to Render
Cube * Window::cube;
PointCloud * Window::cubePoints;
Model * Window::model;
Mesh * Window::mesh;
Object* currObj;

// Camera Matrices 
// Projection matrix:
glm::mat4 Window::projection; 

// View Matrix:
glm::vec3 Window::eyePos(0, 0, 20);			// Camera position.
glm::vec3 Window::lookAtPoint(0, 0, 0);		// The point we are looking at.
glm::vec3 Window::upVector(0, 1, 0);		// The up direction of the camera.
glm::mat4 Window::view = glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);

// Shader Program ID
GLuint Window::shaderProgram; 
GLuint Window::modelShaderProgram; 

glm::vec3 lastPos(0.0, 0.0, 0.0);

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	modelShaderProgram = LoadShaders("modelShader.vert", "modelShader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Check the shader program.
	if (!modelShaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	// model = new Model("models/bunny.obj");
	// mesh = new Mesh();
	// model = new Model("models/Chef Bear/NpcChefBear.fbx");
	model = new Model("models/backpack/backpack.obj");
	// Create a cube of size 5.
	cube = new Cube(5.0f);

	// Create a point cloud consisting of cube vertices.
	cubePoints = new PointCloud("foo", 100);

	// Set cube to be the first to display
	currObj = cube;

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete cube;
	delete cubePoints;
	// delete model;

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
	glDeleteProgram(modelShaderProgram);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	Window::projection = glm::perspective(glm::radians(60.0), 
								double(width) / (double)height, 1.0, 1000.0);
}

void Window::idleCallback()
{
	// Perform any necessary updates here 
	//currObj->update();
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the objects
	// currObj->draw(view, projection, shaderProgram);
	model->draw(view, projection, modelShaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		// switch between the cube and the cube pointCloud
		case GLFW_KEY_1:
			currObj = cube;
			break;
		case GLFW_KEY_2:
			currObj = cubePoints;
			break;

		default:
			break;
		}
	}
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) 
{
	glm::vec3 mouseLoc = convert_points(xpos, ypos);


	if (lastPos.x == 0.0f && lastPos.y == 0.0f && lastPos.z == 0.0f) {
		lastPos = mouseLoc;
	}

	glm::vec3 direction = mouseLoc - lastPos;

	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)) {
		float movem = glm::length(direction);

		float angle = 0.0f;
		glm::vec3 rot(0.0f, 0.0f, 0.0f);
		// Prevent too little movement
		if (movem > 0.001) {
			rot = glm::cross((lastPos), (mouseLoc));
			angle = movem * 100;

			model->rotate(angle, rot);
		}
	}
	lastPos = mouseLoc;
}

glm::vec3 Window::convert_points(double xpos, double ypos) {
	glm::vec3 mouseLoc;

	mouseLoc.x = ((2 * xpos) - width) / width;
	mouseLoc.y = (height - (2 * ypos)) / height;
	mouseLoc.z = 0.0;

	double dis = glm::length(mouseLoc);

	// Do not want values above 1.0
	if (dis > 1.0) {
		dis = 1.0;
	}

	mouseLoc.z = (double)(sqrtf(1.001 - (dis * dis)));
	mouseLoc = glm::normalize(mouseLoc);
	return mouseLoc;
}
