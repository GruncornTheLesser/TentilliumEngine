#include "AppWindow.h"
#include "Resources/Resource.h"
#include <iostream>
#include <thread>
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <vector>
#include <thread>

std::vector<AppWindow*> windows;
std::thread* mainLoopPtr;

static void error_callback(int code, const char* text)
{
	std::cout << "error code [" << code << "] : " << text << std::endl;
}
/*
static void key_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	
}

static void mouse_callback(GLFWwindow* wnd, int er, int idk, int wtf)
{
	
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
}
*/




AppWindow::AppWindow(int width, int height, const char* title)
	: m_title(title), m_width(width), m_height(height)
{
	if (windows.size() == 0)
	{
		glfwSetErrorCallback(error_callback);

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(width, height, m_title, nullptr, nullptr);
		
		if (!m_window)
			throw std::runtime_error("[Engine Error] : Window failed to initialize");

		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window));

		if (glewInit() != GLEW_OK)
			throw std::runtime_error("[Engine Error] : GLEW failed to initialize");
		
		glfwSwapInterval(true);
	}
	else
	{
		m_window = glfwCreateWindow(width, height, m_title, nullptr, static_cast<GLFWwindow*>(windows.front()->m_window));

		if (!m_window) // check window valid
			throw std::runtime_error("[Engine Error] : Window failed to initialize");

		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window));
	}
	
	windows.push_back(this);

	//glfwSetKeyCallback(static_cast<GLFWwindow*>(m_window), key_callback);					// key event
	//glfwSetMouseButtonCallback(static_cast<GLFWwindow*>(m_window), mouse_callback);			// mouse button event
	//glfwSetCursorPosCallback(static_cast<GLFWwindow*>(m_window), movecursor_callback);		// cursor move event
	//glfwSetCursorEnterCallback(static_cast<GLFWwindow*>(m_window), enter_callback);			// cursor enters window event
	//glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(m_window), resize_callback);	// resize event
}

AppWindow::~AppWindow()
{
	glfwDestroyWindow(static_cast<GLFWwindow*>(m_window));
}

void AppWindow::refresh()
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
}

bool AppWindow::Closed() const
{
	return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_window));
}

void AppWindow::Init()
{
	for (;;)
	{
		for (auto wnd_itr = windows.begin(); wnd_itr != windows.end();)
		{
			AppWindow* pItem = *wnd_itr;

			if ((*wnd_itr)->Closed())
			{
				delete *wnd_itr;
				wnd_itr = windows.erase(wnd_itr);

				if (windows.size() == 0)
					return;
			}
			else
			{
				glfwMakeContextCurrent(static_cast<GLFWwindow*>((*wnd_itr)->m_window));
				glfwPollEvents(); // -> goes to callbacks eg onkey, onmouse etc
				(*wnd_itr)->draw();
				(*wnd_itr)->refresh();
				++wnd_itr;
			}
		}
	}

}