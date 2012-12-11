#include <SFML/Window.hpp>
#include "Camera.hpp"
#include <GL/glew.h>
#include "ShaderProgram.hpp"
#include "Inotify.hpp"
#include "InputElementDescription.hpp"
#include "VertexBuffer.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "FPSCounter.hpp"
#include "UI.hpp"
#include <sstream>
#include "GameState.hpp"
#include "models.hpp"
	
int main()
{
	sf::ContextSettings settings;
	settings.majorVersion = 3;
	settings.minorVersion = 2;
	const size_t width = 800;
	const size_t height = 600;
	sf::Window window(sf::VideoMode(width, height), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
	window.setVerticalSyncEnabled(false);
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) throw new GeneralException("glewInit failed");

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, width, height);
	glm::mat4 projection = glm::perspective(60.0f, width / (float)height, 1.0f, 1000.0f);
	glm::mat4 world = glm::mat4(1.0f);

	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT },
                                                      { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT },
                                                      { "in_color",    3, sizeof(glm::vec3), GL_FLOAT },
                                                      { "", 0, 0, 0 } };

	ShaderProgram program("null.vs", "null.fs", description);
	RocketUI rocketui(width, height);
	FPSCounter fpscounter;
	bool running = true;
	std::string currentline;
	VertexBuffer& worldblocks = blocks();
	FreeLookCamera camera(0.0f, 0.0f, glm::vec3(-1.0f, 1.0f, 4.0f), width, height);
	while (running)
	{
		camera.Update();
		std::ostringstream convert;
		convert << fpscounter.Update();
		rocketui.setFPS(convert.str());
		Inotify::Poll();
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
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					running = false;
				}
				else if(event.key.code == sf::Keyboard::Quote)
				{
					running = false;
				}
			}
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFrontFace(GL_CW);
		glDisable(GL_BLEND);
		program.Set("projection", &projection[0][0]);
		program.Set("view", &camera.GetViewMatrix()[0][0]);
		program.Set("world", &world[0][0]);
		program.Use();
		cube().Draw();
//		worldblocks.Draw();
		// Using the mouse sf::Mouse::getPosition();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		rocketui.UpdateAndRender();
		window.display();
	}
	return 0;
}

