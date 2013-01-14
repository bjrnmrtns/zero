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
#include "TimeStepper.hpp"
#include "Input.hpp"

class Player : public Input
{
public:
	Player(glm::vec3 pos)
	: pos(pos)
	{
	}
	void Update(float timepassed)
	{
		static const float playerspeed = 10.0f; // km/h
		float meters_traveled = (playerspeed * timepassed) / (float)3.6;
		old = pos;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) pos.z += meters_traveled;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) pos.z -= meters_traveled;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) pos.x += meters_traveled;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) pos.x -= meters_traveled;
	}
	glm::vec3 pos;
	glm::vec3 old;
};
	
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

	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "in_color",    3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "", 0, 0, 0, false } };

	ShaderProgram program("null.vs", "null.fs", description);
	RocketUI rocketui(width, height);
	FPSCounter fpscounter;
	bool running = true;
	std::string currentline;
	VertexBuffer& worldblocks = blocks();
	FreeLookCamera camera(0.0f, 0.0f, glm::vec3(-1.0f, 1.0f, 4.0f), width, height);
	NoInput noinput;
	std::vector<Input*> inputs;
	Player player(glm::vec3(0.25f, 20.0f, 0.25f));
	inputs.push_back(&camera);
	inputs.push_back(&player);
	inputs.push_back(&noinput);
	auto curinput = inputs.begin();
	static const float physics_frame_rate = 60;
	TimeStepper timestepper(physics_frame_rate);
	while (running)
	{
		size_t nrofsteps = timestepper.steps();
		for(size_t i = 0; i < nrofsteps; i++)
		{
			(*curinput)->Update(1 / (float)physics_frame_rate);
		}
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
				else if(event.key.code == sf::Keyboard::C)
				{
					if(++curinput == inputs.end()) curinput = inputs.begin();
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
		worldblocks.Draw();

		static const float eps = 0.000001f;
		glm::vec3 diff = player.pos - player.old;
		if(diff.y > eps)
		{
			if(theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0
			|| theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0)
			{
				player.pos.y = player.old.y;
			}
		}
		if(diff.y < -eps)
		{
			if(theworld[(int)(player.pos.x)][(int)(player.pos.y)][(int)(player.pos.z)] > 0
			|| theworld[(int)(player.pos.x)][(int)(player.pos.y)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y)][(int)(player.pos.z)] > 0)
			{
				player.pos.y = player.old.y;
			}
		}
		if(diff.x > eps)
		{
			if(theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y)][(int)(player.pos.z)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0)
			{
				player.pos.x = player.old.x;
			}
		}
		if(diff.z > eps)
		{
			if(theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0)
			{
				player.pos.z = player.old.z;
			}
		}
		if(diff.x < -eps)
		{
			if(theworld[(int)(player.pos.x)][(int)(player.pos.y)][(int)(player.pos.z)] > 0
			|| theworld[(int)(player.pos.x)][(int)(player.pos.y)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z + 0.8)] > 0
			|| theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0)
			{
				player.pos.x = player.old.x;
			}
		}
		if(diff.z < -eps)
		{
			if(theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0
			|| theworld[(int)(player.pos.x)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0
			|| theworld[(int)(player.pos.x + 0.8)][(int)(player.pos.y + 0.8)][(int)(player.pos.z)] > 0)
			{
				player.pos.z = player.old.z;
			}
		}


		glm::mat4 playerworld = glm::translate(glm::mat4(1.0f), player.pos);
		program.Set("world", &playerworld[0][0]);
		cube().Draw();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		rocketui.UpdateAndRender();
		window.display();
	}
	return 0;
}

