#include <vector>
#include <memory>
#include <map>
#include <string>

class Buffer
{
private:
	static std::map<std::string, std::shared_ptr<Buffer>> buffers;
public:
	static std::shared_ptr<Buffer> get(std::string name)
	{
		auto it = buffers.find(name);
		if(it == buffers.end())
		{
			std::shared_ptr<Buffer> buffer(new Buffer());
			buffers.insert(std::make_pair(name, buffer));
			return buffer;
		}
		else
		{ 
			return it->second;
		}
	}
};

std::map<std::string, std::shared_ptr<Buffer>> Buffer::buffers;

class RenderStep
{
private:
	std::map<std::string, std::shared_ptr<Buffer>> input;
	std::map<std::string, std::shared_ptr<Buffer>> output;
public:
	void addInput(std::string name)
	{
		input.insert(std::make_pair(name, Buffer::get(name)));
	}
	void addOutput(std::string name)
	{
		output.insert(std::make_pair(name, Buffer::get(name)));
	}
	virtual void run()
	{
	}
};

class DeferredRenderStep : public RenderStep
{
public:
	DeferredRenderStep()
	{
		addOutput("color");
		addOutput("normal");
		addOutput("depth");
	}
	void run()
	{
	}
};

class AmbientRenderStep : public RenderStep
{
public:
	AmbientRenderStep()
	{
		addOutput("occlusionmap");
		addInput("color");
		addInput("normal");
		addInput("depth");
	}
	void run()
	{
	}
};

class FinalRenderStep : public RenderStep
{
public:
	FinalRenderStep()
	{
		addOutput("framebuffer");
		addInput("color");
		addInput("normal");
		addInput("depth");
		addInput("occlusionmap");
	}
	void run()
	{
	}
};

class RenderPipeline
{
private:
	std::vector<std::unique_ptr<RenderStep>> steps;
public:
	void Register(std::unique_ptr<RenderStep> step)
	{
		steps.push_back(std::move(step));
	}
};

int main()
{
	RenderPipeline rp;
	rp.Register(std::unique_ptr<DeferredRenderStep>(new DeferredRenderStep()));
	rp.Register(std::unique_ptr<AmbientRenderStep>(new AmbientRenderStep()));
	rp.Register(std::unique_ptr<FinalRenderStep>(new FinalRenderStep()));
	return 0;
}

