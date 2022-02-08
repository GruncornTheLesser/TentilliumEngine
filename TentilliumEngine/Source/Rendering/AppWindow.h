#pragma once
#include "Resource.h"
#include "Shader.h"
#include "Image.h"

class AppWindow
{
private:
	// all resources that get managed in this context
	ResourceManager<Image, Shader> m_resManager;

	void* m_window;
	int m_width, m_height;
	const char* m_title;
	bool m_vsync;


public:
	AppWindow(int width, int height, const char* title, bool Vsync = true);
	~AppWindow();
	void makeCurrent();
	void refresh();
	bool Closed() const;

	virtual void onResize(int width, int height);
	virtual void onKey(int key, int action);
	virtual void onMouse(int button, int action);
	virtual void onMoveCursor(double posX, double posY);
	virtual void onEnter(int entering);

private:
	void resize(AppWindow* wnd, int width, int height);
	void pressKey(AppWindow* wnd, int key, int scancode, int action, int mods);
	void pressMouse(AppWindow* wnd, int idk0, int idk1, int idk2); // gotta work out what these parameters are
	void moveCursor(AppWindow* wnd, int posX, int posY);
	void enterWindow(AppWindow* wnd, int enter);
};