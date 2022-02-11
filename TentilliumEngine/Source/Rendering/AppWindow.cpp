#include "AppWindow.h"
#include "Resources/Resource.h"
#include <iostream>
#include <thread>
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework

class GLEWContext;

struct aWindow
{
	GLFWwindow* m_window;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_id;
};



bool GLInitialized = false;
AppWindow* currentWindow;
std::list<AppWindow*> windows;

static void error_callback(int code, const char* text)
{
	std::cout << "error code [" << code << "] : " << text << std::endl;
}

static void key_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	currentWindow->onKey(key, action);
}

static void mouse_callback(GLFWwindow* wnd, int er, int idk, int wtf)
{
	currentWindow->onMouse(er, idk);
}

static void movecursor_callback(GLFWwindow* wnd, double posX, double posY)
{
	currentWindow->onMoveCursor(posX, posY);
}

static void enter_callback(GLFWwindow* wnd, int entered)
{
		currentWindow->onEnter(entered);
}

static void resize_callback(GLFWwindow* wnd, int width, int height)
{
	std::cout << "resizing window : " << width << ", " << height << std::endl;
	glViewport(0, 0, width, height);
	
	for (int i = -16; i < 16; i++)
		std::cout << ((float*)wnd)[i] << ", ";
	std::cout << std::endl;
}

AppWindow::AppWindow(int width, int height, const char* title, bool vsync)
	: m_title(title), m_width(width), m_height(height), m_vsync(vsync)
{
	if (!GLInitialized)
	{
		glfwSetErrorCallback(error_callback);

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(640, 480, m_title, nullptr, nullptr);

		if (!m_window) // check window valid
			throw std::runtime_error("[Engine Error] : Window failed to initialize");

		// make window current context
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window));
		
		// requires a window to have been created
		if (glewInit() != GLEW_OK)
			throw std::runtime_error("[Engine Error] : GLEW failed to initialize");

		glfwMakeContextCurrent(NULL); // to make it the same as when window
	}
	else
	{
		m_window = glfwCreateWindow(640, 480, m_title, nullptr, nullptr);

		if (!m_window) // check window valid
			throw std::runtime_error("[Engine Error] : Window failed to initialize");
	}

	glfwSetKeyCallback(static_cast<GLFWwindow*>(m_window), key_callback);					// key event
	glfwSetMouseButtonCallback(static_cast<GLFWwindow*>(m_window), mouse_callback);			// mouse button event
	glfwSetCursorPosCallback(static_cast<GLFWwindow*>(m_window), movecursor_callback);		// cursor move event
	glfwSetCursorEnterCallback(static_cast<GLFWwindow*>(m_window), enter_callback);			// cursor enters window event
	glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(m_window), resize_callback);	// resize event

	glfwSwapInterval(m_vsync);
}

AppWindow::~AppWindow()
{
	glfwDestroyWindow(static_cast<GLFWwindow*>(m_window));
}

void AppWindow::makeCurrent()
{
	m_resManager.makeCurrent();

	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window));
	currentWindow = this;
}

void AppWindow::refresh()
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
	glfwPollEvents();
}

bool AppWindow::Closed() const
{
	return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_window));
}

void AppWindow::onResize(int width, int height)
{
}

void AppWindow::onKey(int key, int action)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_window), GLFW_TRUE);
}

void AppWindow::onMouse(int button, int action)
{
}

void AppWindow::onMoveCursor(double posX, double posY)
{
}

void AppWindow::onEnter(int entering)
{
}


