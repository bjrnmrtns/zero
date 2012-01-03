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
	void use() {};
};

class Renderable
{
public:
	void draw(const ShaderProgram& sp) {};
};

class RenderTarget
{
public:
	void use() {};
};

class Object
{
private:
	Renderable& renderable;
	vec3& position;
	quat& orientation;
public:
	Object(Renderable& renderable, vec3 position, quat orientation)
	: renderable(renderable)
	, position(position)
	, orientation(orientation)
	{
	};
	void draw(const ShaderProgram& sp)
	{
		// push postion and orientation as worldmatrix to gpu so it can do translation and orientation.
		renderable.draw(sp);
	}
};

int main()
{
	RenderTarget deferredtarget;
	Renderable renderable;
	vec3 pos;
	quat q;
	Object object(renderable, pos, q);
	ShaderProgram shaderprogram;

	deferredtarget.use();
	object.draw(shaderprogram);
}

