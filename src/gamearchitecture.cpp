#include <cstdlib>

struct vec3
{
	float x, y, z;
};

struct quat
{
	float x, y, z, w;
};

class ShaderProgram
{
public:
	void use() {}
	void set(const vec3& position, const quat& orientation) const {}
};

class Renderable
{
public:
	virtual void draw(const ShaderProgram& sp) = 0;
};

class VertexBuffer : public Renderable
{
public:
	VertexBuffer()
	{
		// Do some opengl specific stuff for creating a vertex buffer
		// on GPU, probably using some arguments passed in constructor
	}
	void draw(const ShaderProgram& sp)
	{
		// Do opengl specific stuff to use this vertexbuffer id
	}
};

class RenderTarget
{
public:
	void use() {}
};

class Object : public Renderable
{
private:
	Renderable& renderable;
	vec3& position;
	quat& orientation;
public:
	Object(Renderable& renderable, vec3& position, quat& orientation)
	: renderable(renderable)
	, position(position)
	, orientation(orientation)
	{
	}
	void update()
	{
		// do something with position and orientation
		position.x += 1;
	}
	void draw(const ShaderProgram& sp)
	{
		sp.set(position, orientation);
		renderable.draw(sp);
	}
};

class Plane : public Renderable
{
private:
	Object object;
	VertexBuffer vb;
	vec3 pos;
	quat q;
public:
	Plane(size_t size)
	// construct object with renderable which holds a Plane
	: object(vb, pos, q)
	{
	}
	void draw(const ShaderProgram& sp)
	{
		object.draw(sp);
	}
};

int main()
{
	RenderTarget deferredtarget;
	VertexBuffer vb;
	vec3 pos;
	quat q;
	Object object(vb, pos, q);
	Plane p(3);
	ShaderProgram shaderprogram;

	deferredtarget.use();
	object.draw(shaderprogram);
	p.draw(shaderprogram);
}

