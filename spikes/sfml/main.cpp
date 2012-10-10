#include <SFML/Window.hpp>
#include <GL/glew.h>
#include "ShaderProgram.hpp"
#include "Inotify.hpp"

int main()
{
	sf::ContextSettings settings;
	settings.majorVersion = 3;
	settings.minorVersion = 2;
	sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
	window.setVerticalSyncEnabled(true);
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) throw new GeneralException("glewInit failed");

	ShaderProgram program("null.vs", "null.fs");

	bool running = true;
	while (running)
	{
		Inotify::instance.Poll();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		window.display();
	}
	return 0;
}

