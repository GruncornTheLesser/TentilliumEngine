#include "AppWindow.h"
#include<Windows.h>
#include "../Resources/Resource.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework
#include <iostream>
#include <vector>
#include "../../Components/FlagManager.h"

class callback_func {

private:
	static AppWindow* getWindow(void* glfwWindow);

public:
	static void error(int code, const char* text);

	static void key(GLFWwindow* wnd, int key, int scancode, int action, int mod);

	static void mouse(GLFWwindow* wnd, int button, int action, int mod);

	static void mousewheel(GLFWwindow* wnd, double xoffset, double yoffset);

	static void movecursor(GLFWwindow* wnd, double posX, double posY);

	static void enter(GLFWwindow* wnd, int entered);

	static void resize(GLFWwindow* wnd, int width, int height);
};

AppWindow* callback_func::getWindow(void* glfwWindow)
{
	return (AppWindow*)glfwGetWindowUserPointer(static_cast<GLFWwindow*>(glfwWindow));
}

void callback_func::error(int code, const char* message)
{
	std::cout << "error code [" << code << "] : " << message << std::endl;
}

void callback_func::key(GLFWwindow* wnd, int key, int scancode, int action, int mod)
{
    auto appWnd = getWindow(wnd);
    appWnd->onKey(AppWindow::Keyboard::IntToKey(key), Action(action), Mod(mod));
}

void callback_func::mouse(GLFWwindow* wnd, int button, int action, int mod)
{
	auto appWnd = getWindow(wnd);
	appWnd->onMouse(AppWindow::Mouse::IntToButton(button), Action(action), Mod(mod));
}

void callback_func::mousewheel(GLFWwindow* wnd, double xoffset, double yoffset)
{
	auto appWnd = getWindow(wnd);
}

void callback_func::movecursor(GLFWwindow* wnd, double pos_x, double pos_y)
{
	auto appWnd = getWindow(wnd);
	appWnd->m_mouse.next_pos_x = pos_x;
	appWnd->m_mouse.next_pos_y = pos_y;
	appWnd->onMouseMove(pos_x, pos_y);
}

void callback_func::enter(GLFWwindow* wnd, int entered)
{
	//getWindow(wnd)->onEnter(entered);
}

void callback_func::resize(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
    auto* appWnd = getWindow(wnd);
    appWnd->m_width = width;
    appWnd->m_height = height;
    appWnd->onResize(width, height);
}



GLFWwindow* glfwContext;

AppWindow::AppWindow(int width, int height, const char* title)
	: m_title(title), m_width(width), m_height(height), m_window(nullptr)
{
#ifdef _DEBUG
    ShowWindow(GetConsoleWindow(), SW_SHOW);
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

	if (!glfwContext)
	{
		if (!glfwInit())
			throw std::runtime_error("[Init Error] : GLFW failed to initialize.");
		
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);
		glfwSetErrorCallback(callback_func::error);
	}

	m_window = glfwCreateWindow(width, height, title, nullptr, glfwContext);

    if (!m_window)
        throw std::runtime_error("[Window Error] : failed to create GLFW window.");
   
	// makes context current AND window render target
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window));

	if (!glfwContext)
	{
		glfwContext = static_cast<GLFWwindow*>(m_window);
		if (glewInit() != GLEW_OK)
			throw std::runtime_error("[Init Error] : GLEW failed to initialize");	

        std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
        std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
        std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

        if (!GLEW_VERSION_4_6)
            throw std::runtime_error("[Init Error] : OpenGL 4.6 API is not available.");
	}

	glfwSetWindowUserPointer(static_cast<GLFWwindow*>(m_window), this);
	glfwSetKeyCallback(static_cast<GLFWwindow*>(m_window), callback_func::key);
	glfwSetMouseButtonCallback(static_cast<GLFWwindow*>(m_window), callback_func::mouse);
	glfwSetScrollCallback(static_cast<GLFWwindow*>(m_window), callback_func::mousewheel);
	glfwSetCursorPosCallback(static_cast<GLFWwindow*>(m_window), callback_func::movecursor);
	glfwSetCursorEnterCallback(static_cast<GLFWwindow*>(m_window), callback_func::enter);
	glfwSetFramebufferSizeCallback(static_cast<GLFWwindow*>(m_window), callback_func::resize);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
}

AppWindow::~AppWindow()
{
	if (m_window) // if window not already destroyed -> destroy it
		glfwDestroyWindow(static_cast<GLFWwindow*>(m_window));
		
}

bool AppWindow::isPressed(Key key)
{
    return glfwGetKey(static_cast<GLFWwindow*>(m_window), Keyboard::KeyToInt(key));
}

bool AppWindow::isPressed(Button key)
{
    return glfwGetMouseButton(static_cast<GLFWwindow*>(m_window), Mouse::ButtonToInt(key));
}

void AppWindow::close()
{
	glfwSetWindowShouldClose(static_cast<GLFWwindow*>(m_window), true);
}

const char* AppWindow::get_title()
{
	return m_title;
}

void AppWindow::set_title(const char* title)
{
	this->m_title = title;
	glfwSetWindowTitle(static_cast<GLFWwindow*>(m_window), title);
}

int AppWindow::get_width()
{
    return m_width;
}

int AppWindow::get_height()
{
    return m_height;
}

Key AppWindow::Keyboard::IntToKey(int key)
{
    switch (key) {
    case GLFW_KEY_UNKNOWN:      return Key::UNKNOWN;
    case GLFW_KEY_SPACE:        return Key::SPACE;
    case GLFW_KEY_APOSTROPHE:   return Key::APOSTROPHE;
    case GLFW_KEY_COMMA:        return Key::COMMA;
    case GLFW_KEY_MINUS:        return Key::MINUS;
    case GLFW_KEY_PERIOD:       return Key::PERIOD;
    case GLFW_KEY_SLASH:        return Key::SLASH;
    case GLFW_KEY_0:            return Key::NUM_0;
    case GLFW_KEY_1:            return Key::NUM_1;
    case GLFW_KEY_2:            return Key::NUM_2;
    case GLFW_KEY_3:            return Key::NUM_3;
    case GLFW_KEY_4:            return Key::NUM_4;
    case GLFW_KEY_5:            return Key::NUM_5;
    case GLFW_KEY_6:            return Key::NUM_6;
    case GLFW_KEY_7:            return Key::NUM_7;
    case GLFW_KEY_8:            return Key::NUM_8;
    case GLFW_KEY_9:            return Key::NUM_9;
    case GLFW_KEY_SEMICOLON:    return Key::SEMICOLON;
    case GLFW_KEY_EQUAL:        return Key::EQUAL;
    case GLFW_KEY_A:            return Key::A;
    case GLFW_KEY_B:            return Key::B;
    case GLFW_KEY_C:            return Key::C;
    case GLFW_KEY_D:            return Key::D;
    case GLFW_KEY_E:            return Key::E;
    case GLFW_KEY_F:            return Key::F;
    case GLFW_KEY_G:            return Key::G;
    case GLFW_KEY_H:            return Key::H;
    case GLFW_KEY_I:            return Key::I;
    case GLFW_KEY_J:            return Key::J;
    case GLFW_KEY_K:            return Key::K;
    case GLFW_KEY_L:            return Key::L;
    case GLFW_KEY_M:            return Key::M;
    case GLFW_KEY_N:            return Key::N;
    case GLFW_KEY_O:            return Key::O;
    case GLFW_KEY_P:            return Key::P;
    case GLFW_KEY_Q:            return Key::Q;
    case GLFW_KEY_R:            return Key::R;
    case GLFW_KEY_S:            return Key::S;
    case GLFW_KEY_T:            return Key::T;
    case GLFW_KEY_U:            return Key::U;
    case GLFW_KEY_V:            return Key::V;
    case GLFW_KEY_W:            return Key::W;
    case GLFW_KEY_X:            return Key::X;
    case GLFW_KEY_Y:            return Key::Y;
    case GLFW_KEY_Z:            return Key::Z;
    case GLFW_KEY_LEFT_BRACKET: return Key::LEFT_BRACKET;
    case GLFW_KEY_BACKSLASH:    return Key::BACKSLASH;
    case GLFW_KEY_RIGHT_BRACKET:return Key::RIGHT_BRACKET;
    case GLFW_KEY_GRAVE_ACCENT: return Key::GRAVE_ACCENT;
    case GLFW_KEY_WORLD_1:      return Key::WORLD_1;
    case GLFW_KEY_WORLD_2:      return Key::WORLD_2;
    case GLFW_KEY_ESCAPE:       return Key::ESCAPE;
    case GLFW_KEY_ENTER:        return Key::ENTER;
    case GLFW_KEY_TAB:          return Key::TAB;
    case GLFW_KEY_BACKSPACE:    return Key::BACKSPACE;
    case GLFW_KEY_INSERT:       return Key::INSERT;
    case GLFW_KEY_DELETE:       return Key::DEL;
    case GLFW_KEY_RIGHT:        return Key::RIGHT;
    case GLFW_KEY_LEFT:         return Key::LEFT;
    case GLFW_KEY_DOWN:         return Key::DOWN;
    case GLFW_KEY_UP:           return Key::UP;
    case GLFW_KEY_PAGE_UP:      return Key::PAGE_UP;
    case GLFW_KEY_PAGE_DOWN:    return Key::PAGE_DOWN;
    case GLFW_KEY_HOME:         return Key::HOME;
    case GLFW_KEY_END:          return Key::END;
    case GLFW_KEY_CAPS_LOCK:    return Key::CAPS_LOCK;
    case GLFW_KEY_SCROLL_LOCK:  return Key::SCROLL_LOCK;
    case GLFW_KEY_NUM_LOCK:     return Key::NUM_LOCK;
    case GLFW_KEY_PRINT_SCREEN: return Key::PRINT_SCREEN;
    case GLFW_KEY_PAUSE:        return Key::PAUSE;
    case GLFW_KEY_F1:           return Key::F1;
    case GLFW_KEY_F2:           return Key::F2;
    case GLFW_KEY_F3:           return Key::F3;
    case GLFW_KEY_F4:           return Key::F4;
    case GLFW_KEY_F5:           return Key::F5;
    case GLFW_KEY_F6:           return Key::F6;
    case GLFW_KEY_F7:           return Key::F7;
    case GLFW_KEY_F8:           return Key::F8;
    case GLFW_KEY_F9:           return Key::F9;
    case GLFW_KEY_F10:          return Key::F10;
    case GLFW_KEY_F11:          return Key::F11;
    case GLFW_KEY_F12:          return Key::F12;
    case GLFW_KEY_F13:          return Key::F13;
    case GLFW_KEY_F14:          return Key::F14;
    case GLFW_KEY_F15:          return Key::F15;
    case GLFW_KEY_F16:          return Key::F16;
    case GLFW_KEY_F17:          return Key::F17;
    case GLFW_KEY_F18:          return Key::F18;
    case GLFW_KEY_F19:          return Key::F19;
    case GLFW_KEY_F20:          return Key::F20;
    case GLFW_KEY_F21:          return Key::F21;
    case GLFW_KEY_F22:          return Key::F22;
    case GLFW_KEY_F23:          return Key::F23;
    case GLFW_KEY_F24:          return Key::F24;
    case GLFW_KEY_F25:          return Key::F25;
    case GLFW_KEY_KP_0:         return Key::KP_0;
    case GLFW_KEY_KP_1:         return Key::KP_1;
    case GLFW_KEY_KP_2:         return Key::KP_2;
    case GLFW_KEY_KP_3:         return Key::KP_3;
    case GLFW_KEY_KP_4:         return Key::KP_4;
    case GLFW_KEY_KP_5:         return Key::KP_5;
    case GLFW_KEY_KP_6:         return Key::KP_6;
    case GLFW_KEY_KP_7:         return Key::KP_7;
    case GLFW_KEY_KP_8:         return Key::KP_8;
    case GLFW_KEY_KP_9:         return Key::KP_9;
    case GLFW_KEY_KP_DECIMAL:   return Key::KP_DECIMAL;
    case GLFW_KEY_KP_DIVIDE:    return Key::KP_DIVIDE;
    case GLFW_KEY_KP_MULTIPLY:  return Key::KP_MULTIPLY;
    case GLFW_KEY_KP_SUBTRACT:  return Key::KP_SUBTRACT;
    case GLFW_KEY_KP_ADD:       return Key::KP_ADD;
    case GLFW_KEY_KP_ENTER:     return Key::KP_ENTER;
    case GLFW_KEY_KP_EQUAL:     return Key::KP_EQUAL;
    case GLFW_KEY_LEFT_SHIFT:   return Key::LEFT_SHIFT;
    case GLFW_KEY_LEFT_CONTROL: return Key::LEFT_CONTROL;
    case GLFW_KEY_LEFT_ALT:     return Key::LEFT_ALT;
    case GLFW_KEY_LEFT_SUPER:   return Key::LEFT_SUPER;
    case GLFW_KEY_RIGHT_SHIFT:  return Key::RIGHT_SHIFT;
    case GLFW_KEY_RIGHT_CONTROL:return Key::RIGHT_CONTROL;
    case GLFW_KEY_RIGHT_ALT:    return Key::RIGHT_ALT;
    case GLFW_KEY_RIGHT_SUPER:  return Key::RIGHT_SUPER;
    case GLFW_KEY_MENU:         return Key::MENU;
    default:                    return Key::UNKNOWN;
    }
}

int AppWindow::Keyboard::KeyToInt(Key key)
{
    switch (key) {
    case Key::UNKNOWN:      return GLFW_KEY_UNKNOWN;
    case Key::SPACE:        return GLFW_KEY_SPACE;
    case Key::APOSTROPHE:   return GLFW_KEY_APOSTROPHE;
    case Key::COMMA:        return GLFW_KEY_COMMA;
    case Key::MINUS:        return GLFW_KEY_MINUS;
    case Key::PERIOD:       return GLFW_KEY_PERIOD;
    case Key::SLASH:        return GLFW_KEY_SLASH;
    case Key::NUM_0:        return GLFW_KEY_0;
    case Key::NUM_1:        return GLFW_KEY_1;
    case Key::NUM_2:        return GLFW_KEY_2;
    case Key::NUM_3:        return GLFW_KEY_3;
    case Key::NUM_4:        return GLFW_KEY_4;
    case Key::NUM_5:        return GLFW_KEY_5;
    case Key::NUM_6:        return GLFW_KEY_6;
    case Key::NUM_7:        return GLFW_KEY_7;
    case Key::NUM_8:        return GLFW_KEY_8;
    case Key::NUM_9:        return GLFW_KEY_9;
    case Key::SEMICOLON:    return GLFW_KEY_SEMICOLON;
    case Key::EQUAL:        return GLFW_KEY_EQUAL;
    case Key::A:            return GLFW_KEY_A;
    case Key::B:            return GLFW_KEY_B;
    case Key::C:            return GLFW_KEY_C;
    case Key::D:            return GLFW_KEY_D;
    case Key::E:            return GLFW_KEY_E;
    case Key::F:            return GLFW_KEY_F;
    case Key::G:            return GLFW_KEY_G;
    case Key::H:            return GLFW_KEY_H;
    case Key::I:            return GLFW_KEY_I;
    case Key::J:            return GLFW_KEY_J;
    case Key::K:            return GLFW_KEY_K;
    case Key::L:            return GLFW_KEY_L;
    case Key::M:            return GLFW_KEY_M;
    case Key::N:            return GLFW_KEY_N;
    case Key::O:            return GLFW_KEY_O;
    case Key::P:            return GLFW_KEY_P;
    case Key::Q:            return GLFW_KEY_Q;
    case Key::R:            return GLFW_KEY_R;
    case Key::S:            return GLFW_KEY_S;
    case Key::T:            return GLFW_KEY_T;
    case Key::U:            return GLFW_KEY_U;
    case Key::V:            return GLFW_KEY_V;
    case Key::W:            return GLFW_KEY_W;
    case Key::X:            return GLFW_KEY_X;
    case Key::Y:            return GLFW_KEY_Y;
    case Key::Z:            return GLFW_KEY_Z;
    case Key::LEFT_BRACKET: return GLFW_KEY_LEFT_BRACKET;
    case Key::BACKSLASH:    return GLFW_KEY_BACKSLASH;
    case Key::RIGHT_BRACKET:return GLFW_KEY_RIGHT_BRACKET;
    case Key::GRAVE_ACCENT: return GLFW_KEY_GRAVE_ACCENT;
    case Key::WORLD_1:      return GLFW_KEY_WORLD_1;
    case Key::WORLD_2:      return GLFW_KEY_WORLD_2;
    case Key::ESCAPE:       return GLFW_KEY_ESCAPE;
    case Key::ENTER:        return GLFW_KEY_ENTER;
    case Key::TAB:          return GLFW_KEY_TAB;
    case Key::BACKSPACE:    return GLFW_KEY_BACKSPACE;
    case Key::INSERT:       return GLFW_KEY_INSERT;
    case Key::DEL:          return GLFW_KEY_DELETE;
    case Key::RIGHT:        return GLFW_KEY_RIGHT;
    case Key::LEFT:         return GLFW_KEY_LEFT;
    case Key::DOWN:         return GLFW_KEY_DOWN;
    case Key::UP:           return GLFW_KEY_UP;
    case Key::PAGE_UP:      return GLFW_KEY_PAGE_UP;
    case Key::PAGE_DOWN:    return GLFW_KEY_PAGE_DOWN;
    case Key::HOME:         return GLFW_KEY_HOME;
    case Key::END:          return GLFW_KEY_END;
    case Key::CAPS_LOCK:    return GLFW_KEY_CAPS_LOCK;
    case Key::SCROLL_LOCK:  return GLFW_KEY_SCROLL_LOCK;
    case Key::NUM_LOCK:     return GLFW_KEY_NUM_LOCK;
    case Key::PRINT_SCREEN: return GLFW_KEY_PRINT_SCREEN;
    case Key::PAUSE:        return GLFW_KEY_PAUSE;
    case Key::F1:           return GLFW_KEY_F1;
    case Key::F2:           return GLFW_KEY_F2;
    case Key::F3:           return GLFW_KEY_F3;
    case Key::F4:           return GLFW_KEY_F4;
    case Key::F5:           return GLFW_KEY_F5;
    case Key::F6:           return GLFW_KEY_F6;
    case Key::F7:           return GLFW_KEY_F7;
    case Key::F8:           return GLFW_KEY_F8;
    case Key::F9:           return GLFW_KEY_F9;
    case Key::F10:          return GLFW_KEY_F10;
    case Key::F11:          return GLFW_KEY_F11;
    case Key::F12:          return GLFW_KEY_F12;
    case Key::F13:          return GLFW_KEY_F13;
    case Key::F14:          return GLFW_KEY_F14;
    case Key::F15:          return GLFW_KEY_F15;
    case Key::F16:          return GLFW_KEY_F16;
    case Key::F17:          return GLFW_KEY_F17;
    case Key::F18:          return GLFW_KEY_F18;
    case Key::F19:          return GLFW_KEY_F19;
    case Key::F20:          return GLFW_KEY_F20;
    case Key::F21:          return GLFW_KEY_F21;
    case Key::F22:          return GLFW_KEY_F22;
    case Key::F23:          return GLFW_KEY_F23;
    case Key::F24:          return GLFW_KEY_F24;
    case Key::F25:          return GLFW_KEY_F25;
    case Key::KP_0:         return GLFW_KEY_KP_0;
    case Key::KP_1:         return GLFW_KEY_KP_1;
    case Key::KP_2:         return GLFW_KEY_KP_2;
    case Key::KP_3:         return GLFW_KEY_KP_3;
    case Key::KP_4:         return GLFW_KEY_KP_4;
    case Key::KP_5:         return GLFW_KEY_KP_5;
    case Key::KP_6:         return GLFW_KEY_KP_6;
    case Key::KP_7:         return GLFW_KEY_KP_7;
    case Key::KP_8:         return GLFW_KEY_KP_8;
    case Key::KP_9:         return GLFW_KEY_KP_9;
    case Key::KP_DECIMAL:   return GLFW_KEY_KP_DECIMAL;
    case Key::KP_DIVIDE:    return GLFW_KEY_KP_DIVIDE;
    case Key::KP_MULTIPLY:  return GLFW_KEY_KP_MULTIPLY;
    case Key::KP_SUBTRACT:  return GLFW_KEY_KP_SUBTRACT;
    case Key::KP_ADD:       return GLFW_KEY_KP_ADD;
    case Key::KP_ENTER:     return GLFW_KEY_KP_ENTER;
    case Key::KP_EQUAL:     return GLFW_KEY_KP_EQUAL;
    case Key::LEFT_SHIFT:   return GLFW_KEY_LEFT_SHIFT;
    case Key::LEFT_CONTROL: return GLFW_KEY_LEFT_CONTROL;
    case Key::LEFT_ALT:     return GLFW_KEY_LEFT_ALT;
    case Key::LEFT_SUPER:   return GLFW_KEY_LEFT_SUPER;
    case Key::RIGHT_SHIFT:  return GLFW_KEY_RIGHT_SHIFT;
    case Key::RIGHT_CONTROL:return GLFW_KEY_RIGHT_CONTROL;
    case Key::RIGHT_ALT:    return GLFW_KEY_RIGHT_ALT;
    case Key::RIGHT_SUPER:  return GLFW_KEY_RIGHT_SUPER;
    case Key::MENU:         return GLFW_KEY_MENU;
    default:                return GLFW_KEY_UNKNOWN;
    }
}

int AppWindow::Mouse::ButtonToInt(Button button)
{
    switch (button) {
    case Button::UNKNOWN:   return -1;
    case Button::LEFT:      return GLFW_MOUSE_BUTTON_LEFT;
    case Button::RIGHT:     return GLFW_MOUSE_BUTTON_RIGHT;
    case Button::MIDDLE:    return GLFW_MOUSE_BUTTON_MIDDLE;
    case Button::B5:        return GLFW_MOUSE_BUTTON_5;
    case Button::B6:        return GLFW_MOUSE_BUTTON_6;
    case Button::B7:        return GLFW_MOUSE_BUTTON_7;
    case Button::B8:        return GLFW_MOUSE_BUTTON_8;
    }
}

Button AppWindow::Mouse::IntToButton(int button)
{
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:    return Button::LEFT;
    case GLFW_MOUSE_BUTTON_RIGHT:   return Button::RIGHT;
    case GLFW_MOUSE_BUTTON_MIDDLE:  return Button::MIDDLE;
    case GLFW_MOUSE_BUTTON_5:       return Button::B5;
    case GLFW_MOUSE_BUTTON_6:       return Button::B6;
    case GLFW_MOUSE_BUTTON_7:       return Button::B7;
    case GLFW_MOUSE_BUTTON_8:       return Button::B8;
    default:                        return Button::UNKNOWN;
    }
}





void AppWindow::refresh()
{
    m_mouse.cur_pos_x = m_mouse.next_pos_x;
    m_mouse.cur_pos_y = m_mouse.next_pos_y;

	glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void AppWindow::main(std::vector<AppWindow*> windows)
{
	// time setup
	glfwSetTime(0);
	float delta = 0, time = 0;
	for (;;)
	{
        auto wnd = windows.begin();
		while (wnd != windows.end()) // iterate over all windows
		{
			if (glfwWindowShouldClose(static_cast<GLFWwindow*>((*wnd)->m_window))) // if window has been closed
			{
				glfwDestroyWindow(static_cast<GLFWwindow*>((*wnd)->m_window));	// destroy window so its no longer visible
				(*wnd)->m_window = nullptr;										// invalidate ptr
				wnd = windows.erase(wnd);										// erase from windows

                if (windows.empty()) {			// if windows is not empty, continue loop
                    glfwTerminate();			// terminate glfw
                    glfwContext = nullptr;		// invalidate context
                    return;						// return
                }
			}
            else 
            {
			    glfwMakeContextCurrent(static_cast<GLFWwindow*>((*wnd)->m_window));	// render to this window
			    glfwPollEvents(); // -> goes to callbacks eg onkey, onMouse etc		// execute events
                (*wnd)->onProcess(delta);										    // process window
			    (*wnd)->onDraw();												    // draw window
			
			    wnd++;
            }
		}

		// calculate delta time from last frame
		float temp = glfwGetTime();
		delta = temp - time;
		time = temp;

		FlagManager::clear();
	}
}