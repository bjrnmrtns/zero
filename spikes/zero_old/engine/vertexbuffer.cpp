#include "vertexbuffer.h"

void VertexBuffer::Draw(ShaderProgram& shaderProgram, unsigned int type)
{
	Draw(shaderProgram, 0, count, type);
}

void VertexBuffer::Draw(ShaderProgram& shaderProgram, size_t offset, size_t size, unsigned int type)
{
	shaderProgram.Use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glDrawArrays(type, offset, size);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &id);
	glDeleteVertexArrays(1, &vao);
}

