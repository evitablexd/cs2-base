#pragma once

namespace variables
{
	inline bool bunnyhop = false;
}

class menu
{
private:
	bool open = true;

public:

	bool& is_opened() { return this->open; }

	void render();
};

inline menu g_menu;