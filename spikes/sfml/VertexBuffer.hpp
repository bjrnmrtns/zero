#ifndef VERTEXBUFFER__
#define VERTEXBUFFER__

#include <GL/gl.h>
#include "InputElementDescription.hpp"

class VertexBuffer
{
public:
	template <typename T>
	VertexBuffer(const InputElementDescription description[], const T vertexData[], size_t count, unsigned int mode = GL_TRIANGLES)
	: stride(sizeof(T))
	, count(count)
	, mode(mode)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		size_t offset = 0;
		for(int i = 0; description[i].elementsize; ++i)
		{
			glEnableVertexAttribArray(i);
			if(description[i].normalize)
			{
				glVertexAttribPointer(i, description[i].numberofelements, description[i].type, GL_TRUE, stride, (void *)offset);
			}
			else
			{
				glVertexAttribPointer(i, description[i].numberofelements, description[i].type, GL_FALSE, stride, (void *)offset);
			}
			offset += description[i].elementsize;
		}
		glBufferData(GL_ARRAY_BUFFER, stride * count, vertexData, GL_DYNAMIC_DRAW);
	}
	template <typename T>
	void Update(const T vertexData[], size_t count)
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * count, vertexData, GL_DYNAMIC_DRAW);
	}
	void Draw() const
	{
		glBindVertexArray(vao);
		glDrawArrays(mode, 0, count);
	}
	void DrawIndexed(int& indices, int num_indices) const
	{
		glBindVertexArray(vao);
		glDrawElements(mode, num_indices, GL_UNSIGNED_INT, &indices);
	}
	~VertexBuffer()
	{
		glDeleteBuffers(1, &id);
		glDeleteVertexArrays(1, &vao);
	}
private:
	unsigned int id;
	unsigned int vao;
	unsigned int stride;
	size_t count;
	unsigned int mode;
};

#endif
