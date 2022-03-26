#pragma once
#include <string>
#include <vector>
#include "inputs.h"

class AppWindow
{
	friend class callback_func;
public:
	struct Keyboard {
		static Key IntToKey(int key);
		static int KeyToInt(Key key);
	} m_keyboard;

	struct Mouse {
		int cur_pos_x, cur_pos_y, next_pos_x, next_pos_y;

		static Button IntToButton(int button);
		static int ButtonToInt(Button button);

		int getDeltaX() { return next_pos_x - cur_pos_x; }
		int getDeltaY() { return next_pos_y - cur_pos_y; }
	} m_mouse;

public:
	__declspec(property (get = getTitle, put = setTitle)) const char* title;
	__declspec(property (get = getWidth)) int width;
	__declspec(property (get = getHeight)) int height;

private:
	void* m_window;
	int m_width, m_height;
	const char* m_title;

public:
	AppWindow(int width, int height, const char* title);
	
	~AppWindow();

	bool isPressed(Key key);

	bool isPressed(Button key);

	int getWidth();

	int getHeight();

private:
	const char* getTitle();

	void setTitle(const char* title);

public:
	void close();	

	void refresh();

	virtual void onProcess(float delta) = 0;

	virtual void onDraw() = 0;

	virtual void onKey(Key key, Action pressed, Mod mod) = 0;

	virtual void onMouse(Button button, Action action, Mod mod) = 0;

	virtual void onMouseMove(int posX, int posY) = 0;

	virtual void onResize(int width, int height) = 0;


public:
	static void Main(std::vector<AppWindow*> windows);
};




