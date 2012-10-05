#include <iostream>
#include <vector>
#include <memory>
#include <iostream>
#include <map>

class Resource
{
private:
	virtual void Reload() = 0;
public:
	virtual ~Resource() {}
};

class ShaderProgram : public Resource
{
public:
	static std::shared_ptr<ShaderProgram> Create()
	{
		std::shared_ptr<ShaderProgram> sp(new ShaderProgram());
		shaderprograms.insert(std::make_pair("test", sp));
		return sp;
	}
	void Reload()
	{
		std::cout << "Reloaded this resource" << std::endl;
	}
	void Set()
	{
	}
	void Activate()
	{
	}
private:
	uint32_t id;
	static std::map<std::string, std::shared_ptr<ShaderProgram>> shaderprograms;
};
std::map<std::string, std::shared_ptr<ShaderProgram>> ShaderProgram::shaderprograms;

class RenderTarget
{
public:
	static std::shared_ptr<RenderTarget> Create()
	{
		std::shared_ptr<RenderTarget> rt(new RenderTarget());
		rendertargets.insert(std::make_pair("deferred", rt));
		return rt;
	}
	void Activate()
	{
	}
	static std::map<std::string, std::shared_ptr<RenderTarget>> rendertargets;
};
std::map<std::string, std::shared_ptr<RenderTarget>> RenderTarget::rendertargets;

class Renderable
{
public:
	static std::shared_ptr<Renderable> Create()
	{
		std::shared_ptr<Renderable> rbl(new Renderable(1));
		renderables.insert(std::make_pair("henk", rbl));
		return rbl;
	}
	Renderable(size_t id)
	: id(id)
	{
	}
	virtual void Activate()
	{
	}
	virtual void Draw()
	{
		std::cout << "Renderable draw id: " << id << std::endl;
	}
	~Renderable()
	{
		std::cout << "Renderable destr id: " << id << std::endl;
	}
private:
	size_t id;
	static std::map<std::string, std::shared_ptr<Renderable>> renderables;
};
std::map<std::string, std::shared_ptr<Renderable>> Renderable::renderables;

class Camera
{
};

class RenderPass
{
public:
	RenderPass()
	: rendertarget(RenderTarget::Create())
	, shaderprogram(ShaderProgram::Create())
	, renderable(Renderable::Create())
	{
	}
	virtual void Activate()
	{
	}
	void Render()
	{
		renderable->Activate();
		shaderprogram->Activate();
		rendertarget->Activate();
		renderable->Draw();
	}
protected:
	std::shared_ptr<RenderTarget> rendertarget;
	std::shared_ptr<ShaderProgram> shaderprogram;
	std::shared_ptr<Renderable> renderable;
};

class DeferredPass : public RenderPass
{
public:
	DeferredPass()
	: camera(new Camera())
	{
	}
	void Activate()
	{
		shaderprogram->Set();
	}
private:
	std::shared_ptr<Camera> camera;
};


class Renderpipeline
{
public:
	Renderpipeline()
	{
		renderpasses.push_back(std::unique_ptr<RenderPass>(new RenderPass()));
		renderpasses.push_back(std::unique_ptr<RenderPass>(new RenderPass()));
	}
	void Render()
	{
		for(auto it = renderpasses.begin(); it != renderpasses.end(); it++)
		{
			(*it)->Render();
		}
	}
private:
	std::vector<std::unique_ptr<RenderPass>> renderpasses;
};

int main()
{
	std::unique_ptr<Renderpipeline> rp(new Renderpipeline());
	rp->Render();
	Renderpipeline &sec = *rp;	
	sec.Render();
	return 0;
}
