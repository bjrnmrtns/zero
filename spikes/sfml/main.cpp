#include <SFML/Window.hpp>
#include <GL/glew.h>
#include "ShaderProgram.hpp"
#include "Inotify.hpp"
#include "InputElementDescription.hpp"
#include "VertexBuffer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Window/Mouse.hpp>
#include "FPSCounter.hpp"
#include "UI.hpp"
#include <sstream>
	
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
};

static VertexBuffer& cube()
{
	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "in_color",    3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "", 0, 0, 0 } };
	glm::vec3 color(0.0f, 0.0f, 1.0f);
	static Vertex vertices[] = { { glm::vec3( 0,  1,  0), glm::vec3(-1,  0,  0), color },
				     { glm::vec3( 0,  0,  1), glm::vec3(-1,  0,  0), color },
				     { glm::vec3( 0,  0,  0), glm::vec3(-1,  0,  0), color }, 

				     { glm::vec3( 0,  0,  1), glm::vec3(-1,  0,  0), color },
				     { glm::vec3( 0,  1,  0), glm::vec3(-1,  0,  0), color },
				     { glm::vec3( 0,  1,  1), glm::vec3(-1,  0,  0), color },

				     { glm::vec3( 0,  1,  1), glm::vec3( 0,  0,  1), color },
				     { glm::vec3( 1,  0,  1), glm::vec3( 0,  0,  1), color },
				     { glm::vec3( 0,  0,  1), glm::vec3( 0,  0,  1), color },

				     { glm::vec3( 1,  0,  1), glm::vec3( 0,  0,  1), color },
				     { glm::vec3( 0,  1,  1), glm::vec3( 0,  0,  1), color },
				     { glm::vec3( 1,  1,  1), glm::vec3( 0,  0,  1), color },

				     { glm::vec3( 1,  0,  1), glm::vec3( 1,  0,  0), color },
				     { glm::vec3( 1,  1,  0), glm::vec3( 1,  0,  0), color },
				     { glm::vec3( 1,  0,  0), glm::vec3( 1,  0,  0), color },

				     { glm::vec3( 1,  1,  0), glm::vec3( 1,  0,  0), color },
				     { glm::vec3( 1,  0,  1), glm::vec3( 1,  0,  0), color },
				     { glm::vec3( 1,  1,  1), glm::vec3( 1,  0,  0), color },

				     { glm::vec3( 1,  1,  0), glm::vec3( 0,  0, -1), color },
				     { glm::vec3( 0,  0,  0), glm::vec3( 0,  0, -1), color },
				     { glm::vec3( 1,  0,  0), glm::vec3( 0,  0, -1), color },

				     { glm::vec3( 0,  0,  0), glm::vec3( 0,  0, -1), color },
				     { glm::vec3( 1,  1,  0), glm::vec3( 0,  0, -1), color },
				     { glm::vec3( 0,  1,  0), glm::vec3( 0,  0, -1), color },

				     { glm::vec3( 0,  1,  0), glm::vec3( 0,  1,  0), color },
				     { glm::vec3( 1,  1,  1), glm::vec3( 0,  1,  0), color },
				     { glm::vec3( 0,  1,  1), glm::vec3( 0,  1,  0), color },

				     { glm::vec3( 1,  1,  1), glm::vec3( 0,  1,  0), color },
				     { glm::vec3( 0,  1,  0), glm::vec3( 0,  1,  0), color },
				     { glm::vec3( 1,  1,  0), glm::vec3( 0,  1,  0), color },

				     { glm::vec3( 1,  0,  0), glm::vec3( 0, -1,  0), color },
				     { glm::vec3( 0,  0,  1), glm::vec3( 0, -1,  0), color },
				     { glm::vec3( 1,  0,  1), glm::vec3( 0, -1,  0), color },

				     { glm::vec3( 0,  0,  1), glm::vec3( 0, -1,  0), color },
				     { glm::vec3( 1,  0,  0), glm::vec3( 0, -1,  0), color },
				     { glm::vec3( 0,  0,  0), glm::vec3( 0, -1,  0), color }
		};
	static VertexBuffer model(description, vertices, sizeof(vertices)/sizeof(Vertex));
	return model;
}

#include <Rocket/Core.h>
int main()
{
	sf::ContextSettings settings;
	settings.majorVersion = 3;
	settings.minorVersion = 2;
	const size_t width = 800;
	const size_t height = 600;
	sf::Window window(sf::VideoMode(width, height), "OpenGL", sf::Style::Default, sf::ContextSettings(32));
	window.setVerticalSyncEnabled(true);
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) throw new GeneralException("glewInit failed");


	/////////////
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, width, height);
	glm::mat4 projection = glm::perspective(60.0f, width / (float)height, 1.0f, 1000.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
	glm::mat4 world = glm::mat4(1.0f);

	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT },
                                                      { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT },
                                                      { "in_color",    3, sizeof(glm::vec3), GL_FLOAT },
                                                      { "", 0, 0, 0 } };

	ShaderProgram program("null.vs", "null.fs", description);

	///UI
	UIRenderer renderer;
	UIFileInterface fileinterface("ui");
	UISystemInterface systeminterface;
	Rocket::Core::Context* Context;
	Rocket::Core::SetSystemInterface(&systeminterface);
	Rocket::Core::SetFileInterface(&fileinterface);
	Rocket::Core::SetRenderInterface(&renderer);

	if(!Rocket::Core::Initialise())
		throw new std::exception();

	Rocket::Core::FontDatabase::LoadFontFace("Delicious-Bold.otf");
	Rocket::Core::FontDatabase::LoadFontFace("Delicious-BoldItalic.otf");
	Rocket::Core::FontDatabase::LoadFontFace("Delicious-Italic.otf");
	Rocket::Core::FontDatabase::LoadFontFace("Delicious-Roman.otf");

	Context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(width, height));

//		Rocket::Debugger::Initialise(Context);

	Rocket::Core::ElementDocument *Document = Context->LoadDocument("zero.rml");
	Document->Show();
	Document->RemoveReference();
	///UI


	FPSCounter fpscounter;
	bool running = true;
	while (running)
	{
		std::ostringstream convert;
		convert << fpscounter.Update();
		Document->GetElementById("fps")->SetInnerRML(convert.str().c_str());
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
		}
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFrontFace(GL_CW);
		glDisable(GL_BLEND);
		program.Set("projection", &projection[0][0]);
		program.Set("view", &view[0][0]);
		program.Set("world", &world[0][0]);
		program.Use();
		cube().Draw();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		Context->Update();
		Context->Render();
		window.display();
	}
	return 0;
}

