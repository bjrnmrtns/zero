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


static const size_t size = 192;
static int theworld[size][size][size];
bool noise(int x, int y, int z)
{
	return (float)y/(float)size <=  0.5 * sin((float)z/(float)size * 3.14) +  0.5 * sin((float)x/(float)size * 3.14);
}
static VertexBuffer& blocks()
{
	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "in_color",    3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "", 0, 0, 0, false } };
	glm::vec3 colors[] { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	size_t N = 0;
	for(size_t z = 0; z < size; z++)
	{
		for(size_t y = 0; y < size; y++)
		{
			for(size_t x = 0; x < size; x++)
			{
				if(noise(x, y, z))
				{
					theworld[x][y][z] = 1;
					N++;
				}
			}
		}
	}
	Vertex *vertices = new Vertex[3 * 12 * N];
	std::cout << "Number of triangles: " << 12 * N << std::endl;
	size_t i = 0;
	std::srand(time(0));
	for(size_t z = 0; z < size; z++)
	{
		for(size_t y = 0; y < size; y++)
		{
			for(size_t x = 0; x < size; x++)
			{
				glm::vec3 color = colors[std::rand() % (sizeof(colors)/sizeof(glm::vec3))];
				if(theworld[x][y][z] == 1)
				{
					if (x == 0 || !theworld[x-1][y][z])
					{
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3(-1,  0,  0), color }; 

					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3(-1,  0,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3(-1,  0,  0), color };
					}
					if (z == size-1 || !theworld[x][y][z+1])
					{
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };

					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					}
					if (x == size-1 || !theworld[x+1][y][z])
					{
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 1,  0,  0), color };

					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 1,  0,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 1,  0,  0), color };
					}
					if (z == 0 || !theworld[x][y][z-1])
					{
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1), color };

					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  0, -1), color };
					}
					if (y == size-1 || !theworld[x][y+1][z])
					{
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0), color };

					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  0 + z), glm::vec3( 0,  1,  0), color };
					}
					if (y == 0 || !theworld[x][y-1][z])
					{
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
	}
	std::cout << "Number of triangles effectively: " << i / 3 << std::endl;
	static VertexBuffer world(description, vertices, i);
	delete [] vertices;
	return world;
}

static VertexBuffer& cube()
{
	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "in_color",    3, sizeof(glm::vec3), GL_FLOAT, false },
                                                                  { "", 0, 0, 0, false } };
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
