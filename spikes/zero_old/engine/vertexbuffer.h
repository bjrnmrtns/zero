#ifndef VERTEXBUFFER_
#define VERTEXBUFFER_

#include "renderable.h"
#include "shaderprogram.h"
#include <GL/glew.h>

class VertexBuffer : public Renderable
{
public:
	struct AttributeReference
	{
		int offset;
		int count;
	};

	template <typename T>
	VertexBuffer(AttributeReference *refs, const T *vertexData, size_t count)
	: stride(sizeof(T))
	, count(count)
	, refs(refs)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		for(int i = 0; refs[i].count; ++i)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, refs[i].count, GL_FLOAT, GL_FALSE, stride, (void *)refs[i].offset);
		}
		glBufferData(GL_ARRAY_BUFFER, stride * count, vertexData, GL_STATIC_DRAW);
	}

	void Draw(ShaderProgram& shaderProgram, unsigned int type = GL_TRIANGLES);
	void Draw(ShaderProgram& shaderProgram, size_t offset, size_t size, unsigned int type = GL_TRIANGLES);
	~VertexBuffer();
private:
	unsigned int id;
	unsigned int vao;
	unsigned int stride;
	size_t count;
	AttributeReference *refs;
};

#endif

