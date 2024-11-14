#include "states/menu.hpp"
#include "physics.hpp"
#include "PCG/pcg_basic.h"
#include <time.h>
using namespace sf;

int main()
{
	ContextSettings GL_Config;
	ResourceManager Res_Man;
	StateManager State_Man;
	RenderWindow window;
	Clock DT_Timer;
	Event event;
	Image icon;

	pcg32_srandom((uint64_t)time(NULL), ((uint64_t)time(NULL) * 31) | 1);

	GL_Config.antialiasingLevel = 16;
	icon.loadFromFile("gcc/icon.png");

	// Window Mode
	window.create({1400, 950}, "Rogue Matraque", Style::Titlebar | Style::Close, GL_Config);
	window.setPosition(Vector2i(window.getPosition().x, 30));

	// FullScreen Mode
	// window.create(VideoMode::getDesktopMode(), "Rogue Matraque", Style::Fullscreen | Style::Close, GL_Config);

	SetWinDims((Vector2f)window.getSize());
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	window.setKeyRepeatEnabled(false);
	window.setFramerateLimit(400);

	State_Man.run(StateManager::build<MenuState>(State_Man, Res_Man, window, true));

	float DeltaTime = 0.f;
	while (State_Man.running())
	{
		DeltaTime = DT_Timer.restart().asSeconds();
		DeltaTime = (DeltaTime > 0.1f) ? 0.1f : DeltaTime;

		State_Man.nextState();

		while (window.pollEvent(event))
		{
			State_Man.handleEvents(event);
		}

		State_Man.update(DeltaTime);

		State_Man.draw();
	}
	return 0;
}