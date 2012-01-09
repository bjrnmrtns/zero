#include <cstdlib>
#include <vector>
#include <memory>

struct vec3
{
	float x, y, z;
};

struct quat
{
	float x, y, z, w;
};

struct mat4x4
{
	float m[16];
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
	virtual void draw(ShaderProgram& sp) = 0;
};

class RenderView
{
private:
	mat4x4 projection;
public:
	// Let some keyboard handler access position and orientation to make
	// make a moving camera
	vec3 position;
	quat orientation;
	void set(ShaderProgram& sp)
	{
		// set position orientation and projection onto shaderprogram.
	}
};

class RenderTarget
{
public:
	void use() {}
};

class Model : public Renderable
{
public:
	static Model& Square()
	{
		static Model model;
		return model;
	}
	void draw(ShaderProgram& sp)
	{
		// Probably draw VertexBuffer here.
	}
};

// A Object is a renderable entity in a world.
class Object : public Renderable
{
private:
	Renderable& renderable;
	// following two accessed via physics engine callback
	vec3 position;
	quat orientation;
public:
	Object(Renderable& renderable, vec3 position, quat orientation)
	: renderable(renderable)
	, position(position)
	, orientation(orientation)
	{
	}
	Object& operator=(const Object& object)
	{
		renderable = object.renderable;
		position = position;
		orientation = orientation;
		return *this;
	}
	void draw(ShaderProgram& sp)
	{
		sp.set(position, orientation);
		renderable.draw(sp);
	}
};

class Scene : public Renderable
{
private:
	std::vector<std::unique_ptr<Object>> objects;
public:
	void draw(ShaderProgram& sp)
	{
		for(int i = 0; objects.size(); ++i)
		{
			objects[i]->draw(sp);
		}
	}
	void add(std::unique_ptr<Object> object)
	{
		objects.push_back(std::move(object));
	}
};

class RenderPass
{
private:
	RenderTarget deferredtarget;
	Scene& scene;
	RenderView renderview;
	ShaderProgram sp;
public:
	RenderPass(Scene& scene)
	: scene(scene)
	{
	}
	void run()
	{
		deferredtarget.use();
		renderview.set(sp);
		scene.draw(sp);
	}
};

int main()
{
	Scene scene;
	scene.add(std::unique_ptr<Object>(new Object(Model::Square(), vec3(), quat())));
	RenderPass pass(scene);
	pass.run();
}

