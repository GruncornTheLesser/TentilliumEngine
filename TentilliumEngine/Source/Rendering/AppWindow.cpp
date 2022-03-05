#include "AppWindow.h"
#include "Resources/Resource.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <iostream>
#include <vector>


AppWindow* getAppWindow(void* wnd)
{
	return (AppWindow*)glfwGetWindowUserPointer(static_cast<GLFWwindow*>(wnd));
}

static void error_callback(int code, const char* text)
{
	std::cout << "error code [" << code << "] : " << text << std::endl;
}

static void key_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	auto appWnd = getAppWindow(wnd);
	
}

static void mouse_callback(GLFWwindow* wnd, int er, int idk, int wtf)
{
	auto appWnd = getAppWindow(wnd);
}

static void movecursor_callback(GLFWwindow* wnd, double posX, double posY)
{
	//getAppWindow(wnd)->onMoveCursor(posX, posY);
}

static void enter_callback(GLFWwindow* wnd, int entered)
{
	//getAppWindow(wnd)->onEnter(entered);
}

static void resize_callback(GLFWwindow* wnd, int width, int height)
{
	std::cout << "resizing " << getAppWindow(wnd)->getTitle() << " : " << width << ", " << height << '\n';
	glViewport(0, 0, width, height);
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
	
		const char* vendor = (const char*)glGetString(GL_VENDOR);
		const char* renderer = (const char*)glGetString(GL_RENDERER);
		const char* language = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

		std::cout << renderer << "\n" << language << std::endl;
	}

	glfwSetWindowUserPointer(static_cast<GLFWwindow*>(m_window), this);
	glfwSetKeyCallback(static_cast<GLFWwindow*>(m_window), key_callback);						// key event
	glfwSetMouseButtonCallback(static_cast<GLFWwindow*>(m_window), mouse_callback);				// mouse button event
	glfwSetCursorPosCallback(static_cast<GLFWwindow*>(m_window), movecursor_callback);			// cursor move event
	glfwSetCursorEnterCallback(static_cast<GLFWwindow*>(m_window), enter_callback);				// cursor enters window event
	glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(m_window), resize_callback);		// resize event
	
	//  per frame buffer (i think)
	glClearColor(0, 0, 0, 1); 
	glClearDepth(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

}

AppWindow::~AppWindow()
{
	std::cout << "destroying: " << getTitle() << std::endl;
	if (m_window)
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

void AppWindow::Init(std::vector<AppWindow*> windows)
{
	glfwSetTime(0);
	double temp, delta, time = 0;
	for (;;)
	{
		temp = glfwGetTime();
		delta = temp - time;
		time = temp;

		for (auto wnd = windows.begin(); wnd != windows.end(); )
		{
			if (glfwWindowShouldClose(static_cast<GLFWwindow*>((*wnd)->m_window))) 
			{
				glfwDestroyWindow(static_cast<GLFWwindow*>((*wnd)->m_window));
				(*wnd)->m_window = nullptr;
				wnd = windows.erase(wnd);

				if (!windows.empty()) 
					continue;
				
				glfwContext = nullptr;
				glfwTerminate();
				return;
			}

			glfwMakeContextCurrent(static_cast<GLFWwindow*>((*wnd)->m_window));
			glfwPollEvents(); // -> goes to callbacks eg onkey, onMouse etc
			(*wnd)->onDraw(delta);

			glfwSwapBuffers(static_cast<GLFWwindow*>((*wnd)->m_window));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			wnd++;
		}
	}
}