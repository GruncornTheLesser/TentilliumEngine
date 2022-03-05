#pragma once
#include <string>
#include <vector>
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
	//virtual void onProcess(float delta) = 0;
	//virtual void onKeyDown() = 0;
	//virtual void onResize() = 0;
	
public:
	static void Init(std::vector<AppWindow*> windows);

};




