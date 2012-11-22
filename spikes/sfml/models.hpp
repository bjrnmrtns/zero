#ifndef MODELSH__
#define MODELSH__

#include <GL/glew.h>
#include "VertexBuffer.hpp"
#include <glm/glm.hpp>
#include "InputElementDescription.hpp"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
};

static int theworld[128][128][128];
static VertexBuffer& blocks()
{
	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "in_color",    3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "", 0, 0, 0 } };
	glm::vec3 colors[] { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	size_t N = 0;
	for(size_t z = 0; z < 128; z++)
	{
		for(size_t y = 0; y < 128; y++)
		{
			for(size_t x = 0; x < 128; x++)
			{
				if((y == 0) || ((y == 1) && x == 3))
				{
					theworld[x][y][z] = 1;
					N++;
				}
			}
		}
	}
	const size_t size = 128;
	Vertex *vertices = new Vertex[3 * 12 * N];
	size_t i = 0;
	std::srand(time(0));
	for(size_t z = 0; z < size; z++)
	{
		for(size_t y = 0; y < size; y++)
		{
			for(size_t x = 0; x < size; x++)
			{
				glm::vec3 color = colors[std::rand() % (sizeof(colors)/sizeof(glm::vec3))];
				if(theworld[x][y][z])
				{
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3(-1,  0,  0), color }; 

					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3(-1,  0,  0), color };

					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };

					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };

					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 1,  0,  0), color };

					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 1,  0,  0), color };

					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1), color };

					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1), color };

					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0), color };

					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0), color };

					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 0, -1,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0, -1,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0, -1,  0), color };

					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0, -1,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 0, -1,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0, -1,  0), color };
				}
			}
		}
	}
	static VertexBuffer world(description, vertices, sizeof(vertices)/sizeof(Vertex));
	delete vertices;
	return world;
}

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

#endif
