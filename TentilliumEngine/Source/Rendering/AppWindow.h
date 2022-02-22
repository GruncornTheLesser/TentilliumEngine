#pragma once
#include <string>

class AppWindow
{
private:
	void* m_window;
	unsigned int m_width, m_height;
	std::string title;


public:
	const char* m_title;
public:
	AppWindow(int width, int height, const char* title);
	~AppWindow();

	void close();
	void setTitle(std::string title);

	void refresh();
	virtual void onDraw(float delta) = 0;
	//virtual void onKeyDown() = 0;
	//virtual void onResize()
	
	static void Init();
};




