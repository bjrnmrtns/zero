#include <vector>
#include <memory>
#include <map>
#include <string>
#include <iostream>

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
	static std::shared_ptr<Buffer> alias(std::string alias, std::string original)
	{
		std::shared_ptr<Buffer> aliasedBuffer(Buffer::get(original));
		// if it is already in the std::map, the std::map is not updated
		// this is a property of a std::map which we use so we can safely
		// insert it here.
		buffers.insert(std::make_pair(alias, aliasedBuffer));
		return aliasedBuffer;
	}
};
std::map<std::string, std::shared_ptr<Buffer>> Buffer::buffers;

class RenderStep
{
public:
	static std::map<std::string, std::shared_ptr<RenderStep>> pipeline;
	std::map<std::string, std::shared_ptr<Buffer>> input;
	std::map<std::string, std::shared_ptr<Buffer>> output;
protected:
	const std::string name;
public:
	RenderStep(const std::string name)
	: name(name)
	{
	}
	void addInput(std::string name)
	{
		input.insert(std::make_pair(name, Buffer::get(name)));
	}
	void addOutput(std::string name, std::shared_ptr<RenderStep> step)
	{
		pipeline.insert(std::make_pair(name, step));
		output.insert(std::make_pair(name, Buffer::get(name)));
	}
	virtual void run()
	{
	}
};
std::map<std::string, std::shared_ptr<RenderStep>> RenderStep::pipeline;

class DeferredRenderStep : public RenderStep
{
public:
	DeferredRenderStep()
	: RenderStep("DeferredRenderStep")
	{
		std::shared_ptr<RenderStep> sharedThis(this);
		addOutput("color", sharedThis);
		addOutput("normal", sharedThis);
		addOutput("depth", sharedThis);
	}
	void run()
	{
	}
};

class AmbientRenderStep : public RenderStep
{
public:
	AmbientRenderStep()
	: RenderStep("FinalRenderStep")
	{
		std::shared_ptr<RenderStep> sharedThis(this);
		addOutput("occlusionmap", sharedThis);
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
	: RenderStep("FinalRenderStep")
	{
		std::shared_ptr<RenderStep> sharedThis(this);
		addOutput("framebuffer", sharedThis);
		addInput("color");
		addInput("normal");
		addInput("depth");
		addInput("occlusionmap");
	}
	void run()
	{
	}
};

int main()
{
	new DeferredRenderStep();
	new AmbientRenderStep();
	new FinalRenderStep();
	for(auto it = RenderStep::pipeline.begin(); it != RenderStep::pipeline.end(); ++it)
	{
		std::cout << it->first << std::endl;
	}
	return 0;
}

