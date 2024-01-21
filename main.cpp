#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameEngine.h"
#include "vec2.h"

int main()
{
	GameEngine g("assets.txt");
	g.run();

	return 0;
};