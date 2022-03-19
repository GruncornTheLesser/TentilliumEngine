#pragma once
#include <string>
#include <vector>

class AppWindow
{
	friend class callback_func;
private:
	struct Keyboard {
		static Key IntToKey(int key);
		static int KeyToInt(Key key);
	} m_keyboard;

	struct Mouse { 
		int cur_pos_x, cur_pos_y;
		int next_pos_x, next_pos_y;
	} m_mouse;


public:
	__declspec(property (get = getTitle, put = setTitle)) const char* title;
private:
	void* m_window;
	int m_width, m_height;
	const char* m_title;

public:
	AppWindow(int width, int height, const char* title);
	
	~AppWindow();

	bool isPressed(Key key);

private:
	const char* getTitle();

	void setTitle(const char* title);

public:
	void close();	

	void refresh();

	virtual void onDraw(float delta) = 0;

	virtual void onKey(Key key, Action pressed, Mod mod);

	virtual void onMouse(Button button, Action action, Mod mod);

	virtual void onMouseMove(int posX, int posY);

	virtual void onResize(int width, int height);


public:
	static void Main(std::vector<AppWindow*> windows);
};




