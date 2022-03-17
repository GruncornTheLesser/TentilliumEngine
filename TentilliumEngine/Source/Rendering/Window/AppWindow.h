#pragma once
#include <string>
#include <vector>
#include "Keyboard.h"
#include "Mouse.h"

class AppWindow
{
private:
	void* m_window;
	unsigned int m_width, m_height;
	std::string title;

public:
	AppWindow(int width, int height, std::string title);
	~AppWindow();

	void close();

	std::string getTitle();
	void setTitle(std::string title);

	void refresh();

	virtual void onDraw(float delta) = 0;

	virtual void onKey(Key key, bool pressed);

	virtual void onMouse(Button button, int pressed);

	virtual void onMouseMove(int posX, int posY, int deltaX, int deltaY);

	virtual void onResize(int width, int height);


public:
	static void Main(std::vector<AppWindow*> windows);
};




