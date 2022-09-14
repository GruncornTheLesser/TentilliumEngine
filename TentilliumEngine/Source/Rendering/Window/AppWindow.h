#pragma once
#include <glm.hpp>	// GL maths
#include <string>
#include <vector>
#include "inputs.h"

class AppWindow
{
	friend class callback_func;
public:
	class Keyboard {
		friend class AppWindow;
		friend class callback_func;
	private:
		static Key IntToKey(int key);
		static int KeyToInt(Key key);
	} m_keyboard;

	class Mouse {
		friend class AppWindow;
		friend class callback_func;
	public:
		glm::ivec2 getDelta() { return next_pos - cur_pos; }
	private:
		glm::ivec2 cur_pos, next_pos;

		static Button IntToButton(int button);

		static int ButtonToInt(Button button);

	} m_mouse;

public:
	__declspec(property (get = getTitle, put = setTitle)) const char* title;
	__declspec(property (get = getSize)) glm::ivec2 size;
	__declspec(property (get = get_width)) int width;
	__declspec(property (get = get_height)) int height;

private:
	void* m_window;
	int m_width, m_height;
	const char* m_title;

public:
	AppWindow(int width, int height, const char* title);
	
	~AppWindow();

	glm::ivec2 getSize();

	const char* get_title();

	void set_title(const char* title);

	void close();	

	void refresh();

	bool isPressed(Button key);

	bool isPressed(Key key);

	virtual void onProcess(float delta) = 0;

	virtual void onDraw() = 0;

	virtual void onKey(Key key, Action pressed, Mod mod) = 0;

	virtual void onMouse(Button button, Action action, Mod mod) = 0;

	virtual void onMouseMove(glm::ivec2 position) = 0;

	virtual void onResize(glm::ivec2 size) = 0;

public:
	static void main(std::vector<AppWindow*> windows);
};




