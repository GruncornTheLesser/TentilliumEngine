#include "AppWindow.h"
#include "../Resources/Resource.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <iostream>
#include <vector>
#include "../../Components/FlagManager.h"

AppWindow* getWindow(void* glfwWindow)
{
	return (AppWindow*)glfwGetWindowUserPointer(static_cast<GLFWwindow*>(glfwWindow));

}

static void error_callback(int code, const char* text)
{
	std::cout << "error code [" << code << "] : " << text << std::endl;
}

static void key_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	auto appWnd = getWindow(wnd);
	appWnd->onKey((Key)key, action == GLFW_PRESS);
}

static void mouse_callback(GLFWwindow* wnd, int button, int action, int mod)
{
	auto appWnd = getWindow(wnd);
	appWnd->onMouse((Button)button, action == GLFW_PRESS);
}

static void mousewheel_callback(GLFWwindow* wnd, double xoffset, double yoffset)
{
	auto appWnd = getWindow(wnd);
	appWnd->onMouse(Button::SCROLL, (int)yoffset);
}

static void movecursor_callback(GLFWwindow* wnd, double posX, double posY)
{
	auto appWnd = getWindow(wnd);
	appWnd->onMouseMove(posX, posY, Mouse::getPosX() - posX, Mouse::getPosY() - posY);
}

static void enter_callback(GLFWwindow* wnd, int entered)
{
	//getWindow(wnd)->onEnter(entered);
}

static void resize_callback(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
	getWindow(wnd)->onResize(width, height);
}

GLFWwindow* glfwContext;

AppWindow::AppWindow(int width, int height, std::string title)
	: title(title), m_width(width), m_height(height), m_window(nullptr)
{

	if (!glfwContext)
	{
		if (!glfwInit())
			throw std::runtime_error("[Engine Error] : GLFW failed to initialize");
		
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		
		glfwSetErrorCallback(error_callback);
	}

	m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, glfwContext);

	// makes context current AND window render target
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window));

	if (!glfwContext)
	{
		glfwContext = static_cast<GLFWwindow*>(m_window);
		if (glewInit() != GLEW_OK)
			throw std::runtime_error("[Engine Error] : GLEW failed to initialize");	

		const char* renderer = (const char*)glGetString(GL_RENDERER);
		const char* vendor = (const char*)glGetString(GL_VENDOR);
		const char* version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

		//std::cout << renderer << std::endl;
		//std::cout << vendor << std::endl;
		std::cout << "OpenGL - " << version << std::endl;
	}

	glfwSetWindowUserPointer(static_cast<GLFWwindow*>(m_window), this);
	glfwSetKeyCallback(static_cast<GLFWwindow*>(m_window), key_callback);
	glfwSetMouseButtonCallback(static_cast<GLFWwindow*>(m_window), mouse_callback);
	glfwSetScrollCallback(static_cast<GLFWwindow*>(m_window), mousewheel_callback);
	glfwSetCursorPosCallback(static_cast<GLFWwindow*>(m_window), movecursor_callback);
	glfwSetCursorEnterCallback(static_cast<GLFWwindow*>(m_window), enter_callback);
	glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(m_window), resize_callback);

	

	//glClearColor(0, 0, 0, 1); 
	//glClearDepth(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

}

AppWindow::~AppWindow()
{
	if (m_window) // if window not already destroyed -> destroy it
		glfwDestroyWindow(static_cast<GLFWwindow*>(m_window));
		
}

void AppWindow::close()
{
	glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_window), true);
}

std::string AppWindow::getTitle()
{
	return title;
}

void AppWindow::setTitle(std::string title)
{
	this->title = title;
	glfwSetWindowTitle(static_cast<GLFWwindow*>(m_window), title.c_str());
}

void AppWindow::refresh()
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void AppWindow::onKey(Key key, bool pressed) 
{
	Keyboard::setKey(key, pressed);
}

void AppWindow::onMouse(Button button, int pressed) 
{
	Mouse::setButton(button, pressed);
}

void AppWindow::onMouseMove(int posX, int posY, int deltaX, int deltaY)
{
	Mouse::setPosition(posX, posY);
}

void AppWindow::onResize(int width, int height) 
{
	m_width = width;
	m_height = height;
}



void AppWindow::Main(std::vector<AppWindow*> windows)
{
	// check input
	for (auto it = windows.begin(); it != windows.end(); )
	{
		if (!(*it)->m_window) // if window invalid
		{
			std::cerr << "trying to use invalid window" << std::endl;
			it = windows.erase(it);
		}
		else
		{
			it++;
		}
	}
	
	if (windows.empty()) // if no valid windows found
		return;

	// time setup
	glfwSetTime(0);
	float delta = 0, time = 0;
	for (;;)
	{
		for (auto it = windows.begin(); it != windows.end(); ) // iterate over all windows
		{
			if (glfwWindowShouldClose(static_cast<GLFWwindow*>((*it)->m_window))) // if window has been closed
			{
				glfwDestroyWindow(static_cast<GLFWwindow*>((*it)->m_window));	// destroy window so its no longer visible
				(*it)->m_window = nullptr;										// invalidate ptr
				it = windows.erase(it);											// erase from windows

				if (!windows.empty())											// if windows is not empty, continue loop
					continue;
				
				glfwContext = nullptr;											// invalidate context
				glfwTerminate();												// terminate glfw
				return;															// return
			}

			glfwMakeContextCurrent(static_cast<GLFWwindow*>((*it)->m_window));	// render to this window
			glfwPollEvents(); // -> goes to callbacks eg onkey, onMouse etc		// execute events
			(*it)->onDraw(delta);												// draw to window

			it++;
		}

		// calculate delta time from last frame
		float temp = glfwGetTime();
		delta = temp - time;
		time = temp;

		FlagManager::clear();
	}
}

