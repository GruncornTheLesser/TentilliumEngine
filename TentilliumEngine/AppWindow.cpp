#include "AppWindow.h"
#include "OpenGL.h"
#include <iostream>
#include <thread>


static bool GLInitialized = false;

static void key_callback(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(wnd, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
}

AppWindow::AppWindow(int width, int height, const char* title, bool vsync) : m_Title(title), m_Width(width), m_Height(height), m_Vsync(vsync)
{
	if (!GLInitialized)
	{
		if (!glfwInit())
			throw std::runtime_error("[Engine Error] - GLFW failed to initialize");


		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_Window = glfwCreateWindow(640, 480, m_Title, nullptr, nullptr);

		if (!m_Window) // check window valid
		{
			glfwTerminate();
			throw std::runtime_error("[Engine Error] : Window failed to initialize");
		}

		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_Window));

		if (glewInit() != GLEW_OK)
		{
			glfwTerminate();
			throw std::runtime_error("[Engine Error] : GLEW failed to initialize");
		}
	}
	else
	{
		m_Window = glfwCreateWindow(640, 480, m_Title, nullptr, nullptr);

		if (!m_Window) // check window valid
		{
			glfwTerminate();
			throw std::runtime_error("[Engine Error] : Window failed to initialize");
		}

		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_Window));
	}

	glfwSetKeyCallback(static_cast<GLFWwindow*>(m_Window), key_callback);
	glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(m_Window), framebuffer_size_callback);

	if (m_Vsync)
		glfwSwapInterval(1);
	
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_Window));

	glEnable(GL_DEPTH);
	glEnable(GL_CULL_FACE);
}

AppWindow::~AppWindow()
{
	glfwTerminate();
	glfwDestroyWindow(static_cast<GLFWwindow*>(m_Window));
}

void AppWindow::MakeCurrent()
{
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_Window));
}

void AppWindow::Refresh()
{
	glfwSwapBuffers(static_cast<GLFWwindow*>(m_Window));
	glfwPollEvents();
}

bool AppWindow::Closed() const
{
	return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_Window));
}
