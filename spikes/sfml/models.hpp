#ifndef MODELSH__
#define MODELSH__

#include <GL/glew.h>
#include "VertexBuffer.hpp"
#include <glm/glm.hpp>
#include "InputElementDescription.hpp"
#include <glm/gtx/noise.hpp>

enum blocktypes
{
	Air,
	Sand,
	Grass,
	Water,
	Size
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
};

static float noise2d(float x, float y, int octaves, float persistence) {
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;

	for(int i = 0; i < octaves; i++)
	{
		sum += strength * glm::simplex(glm::vec2(x, y) * scale);
		scale *= 2.0;
		strength *= persistence;
	}
	return sum;
}

static float noise3d(float x, float y, float z, int octaves, float persistence)
{
	float sum = 0;
	float strength = 1.0;
	float scale = 1.0;

	for(int i = 0; i < octaves; i++) {
		sum += strength * fabs(glm::simplex(glm::vec3(x, y, z) * scale));
		scale *= 2.0;
		strength *= persistence;
	}

	return sum;
}


static const size_t size_x = 256;
static const size_t size_y = 32;
static const size_t size_z = 256;
static const int water_height = 3;
static uint8_t theworld[size_x][size_y][size_z];
uint8_t noise(int x, int y, int z)
{
	if(y <= water_height) return Water;
	float n = noise2d(x / ((float)size_x / 2), z / ((float)size_z / 2), 5, 0.3) * 4;
	int h = n * 3;
	if(y < h + 9)
	{
		float r = noise3d(x / ((float)size_x / 2), y / ((float)size_y / 2), z / ((float)size_z / 2), 2, 1);
		if(n + r * 4 < 3) return Sand;
		return Grass;
	}
	else
	{
		return Air;
	}
}
static VertexBuffer& blocks()
{
	const InputElementDescription description[] { { "in_position", 3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "in_normal",   3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "in_color",    3, sizeof(glm::vec3), GL_FLOAT, false },
                                                      { "", 0, 0, 0, false } };
	glm::vec3 colors[Size];
	colors[Sand] = glm::vec3(1.0f, 1.0f, 0.0f);
	colors[Grass] = glm::vec3(0.0f, 1.0f, 0.0f);
	colors[Water] = glm::vec3(0.0f, 0.0f, 1.0f);
	size_t N = 0;
	for(size_t z = 0; z < size_z; z++)
	{
		for(size_t y = 0; y < size_y; y++)
		{
			for(size_t x = 0; x < size_x; x++)
			{
				theworld[x][y][z] = noise(x, y, z);
				N++;
			}
		}
	}
	Vertex *vertices = new Vertex[3 * 12 * N];
	std::cout << "Number of triangles: " << 12 * N << std::endl;
	size_t i = 0;
	std::srand(time(0));
	for(size_t z = 0; z < size_z; z++)
	{
		for(size_t y = 0; y < size_y; y++)
		{
			for(size_t x = 0; x < size_x; x++)
			{
				//glm::vec3 color = colors[std::rand() % (sizeof(colors)/sizeof(glm::vec3))];
				glm::vec3 color = colors[theworld[x][y][z]];
				if(theworld[x][y][z] > 0)
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
					if (z == size_z-1 || !theworld[x][y][z+1])
					{
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 0 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };

					vertices[i++] = { glm::vec3( 1 + x,  0 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 0 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					vertices[i++] = { glm::vec3( 1 + x,  1 + y,  1 + z), glm::vec3( 0,  0,  1), color };
					}
					if (x == size_x-1 || !theworld[x+1][y][z])
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
					if (y == size_y-1 || !theworld[x][y+1][z])
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
