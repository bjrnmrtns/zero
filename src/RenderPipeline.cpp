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
	: RenderStep("FinalRenderStep")
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

