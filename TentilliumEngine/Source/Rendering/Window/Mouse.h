#include "AppWindow.h"
#include <bitset>

enum class Button
{
	BUTTON_1 = 0,
	BUTTON_2 = 1,
	BUTTON_3 = 2,
	BUTTON_4 = 3,
	BUTTON_5 = 4,
	BUTTON_6 = 5,
	BUTTON_7 = 6,
	BUTTON_8 = 7,
	SCROLL = 8,
	BUTTON_LEFT = 0,
	BUTTON_RIGHT = 1,
	BUTTON_MIDDLE = 2,
};

class Mouse
{
	friend class AppWindow;
private:
	static inline std::bitset<8> m_pressed = std::bitset<8>();

	static inline int m_x, m_y;

	static void setButton(Button button, bool pressed) 
	{
		int b = (int)button; // doesnt set anything for Button::SCROLL
		if (b < 8) m_pressed.set((int)button, pressed);
	}

	static void setPosition(int x, int y) 
	{
		m_x = x;
		m_y = y;
	}

public:
	static bool isPressed(Button button) 
	{
		return m_pressed.test((int)button);
	}

	static int getPosX() { return m_x; }
	static int getPosY() { return m_y; }
};