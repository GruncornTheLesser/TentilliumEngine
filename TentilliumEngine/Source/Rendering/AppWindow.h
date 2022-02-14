#pragma once




class AppWindow
{
private:
	void* m_window;

	int m_width, m_height;
	const char* m_title;

public:
	AppWindow(int width, int height, const char* title);
	~AppWindow();
	virtual void draw() = 0;
	void refresh();
	bool Closed() const;

	static void Init();
	/*
	virtual void onResize(int width, int height);
	virtual void onKey(int key, int action);
	virtual void onMouse(int button, int action);
	virtual void onMoveCursor(double posX, double posY);
	virtual void onEnter(int entering);
	*/
};




