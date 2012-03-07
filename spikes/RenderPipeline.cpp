#include <vector>
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>

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
private:
	static std::map<std::string, RenderStep*> pipeline;
	static std::vector<RenderStep*> flatten;
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
	void addOutput(std::string name)
	{
		assert(pipeline.find(name) == pipeline.end());
		pipeline.insert(std::make_pair(name, this));
		output.insert(std::make_pair(name, Buffer::get(name)));
	}
	static void CreatePipeline(RenderStep* renderStep)
	{
		CreateGraph(renderStep);
		flatten.erase(std::unique(flatten.begin(), flatten.end()), flatten.end());
	};
	static void CreateGraph(RenderStep* currentStep)
	{
		for(auto it = currentStep->input.begin(); it != currentStep->input.end(); ++it)
		{
			CreateGraph(pipeline[it->first]);
		}
		flatten.push_back(currentStep);
	}
	static void PrintFlatten()
	{
		for(auto it = flatten.begin(); it != flatten.end(); ++it)
		{
			std::cout << (*it)->name << std::endl;
		}
	}
	virtual void run()
	{
	}
};
std::map<std::string, RenderStep*> RenderStep::pipeline;
std::vector<RenderStep*> RenderStep::flatten;

class DeferredRenderStep : public RenderStep
{
public:
	DeferredRenderStep()
	: RenderStep("DeferredRenderStep")
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
	: RenderStep("AmbientRenderStep")
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
	: RenderStep("FinalRenderStep")
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

int main()
{
	DeferredRenderStep ds;
	AmbientRenderStep as;
	FinalRenderStep fs;
	RenderStep::CreatePipeline(&fs);
	RenderStep::PrintFlatten();
	return 0;
}

