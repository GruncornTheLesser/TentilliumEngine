#pragma once
#include "Resources/Resource.h"
#include "Resources/Shader.h"
#include "Resources/Image.h"
#include "Resources/Material.h"



class AppWindow
{
private:
	void* m_window;

	// all resources that get managed in this context
	ResourceManager<Image, Shader, Material> m_resManager;

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
};




