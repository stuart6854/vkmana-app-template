#include <fmt/format.h>

#include <glm/glm.hpp>

#include <VkMana/Context.hpp>
#include <VkMana/ShaderCompiler.hpp>
#include <VkMana/WSI.hpp>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#define LOG_INFO(...) std::cerr << fmt::format(__VA_ARGS__) << std::endl
#define LOG_WARN(...) std::cerr << fmt::format(__VA_ARGS__) << std::endl
#define LOG_ERROR(...) std::cerr << fmt::format(__VA_ARGS__) << std::endl

const auto TriangleHLSLShader = R"(
struct VSOutput
{
	float4 FragPos : SV_POSITION;
	[[vk::location(0)]] float4 Color : COLOR0;
};
VSOutput VSMain(uint vtxId : SV_VERTEXID)
{
	const float3 positions[3] = {
		float3(0.5, 0.5, 0.0),
		float3(-0.5, 0.5, 0.0),
		float3(0.0, -0.5, 0.0),
	};
	const float4 colors[3] = {
		float4(1, 0, 0, 1),
		float4(0, 1, 0, 1),
		float4(0, 0, 1, 1),
	};
	VSOutput output;
	output.FragPos = float4(positions[vtxId], 1.0);
	output.Color = colors[vtxId];
	return output;
}
struct PSInput
{
	[[vk::location(0)]] float4 Color : COLOR;
};
struct PSOutput
{
	float4 FragColor : SV_TARGET0;
};
PSOutput PSMain(PSInput input)
{
	PSOutput output;
	output.FragColor = input.Color;
	return output;
}
)";

class Window : public VkMana::WSI
{
public:
	Window() = default;
	~Window() override { glfwDestroyWindow(m_window); }

	bool Init()
	{
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_window = glfwCreateWindow(1280, 720, "VkMana App Template", nullptr, nullptr);
		if (!m_window)
			return false;

		glfwSetWindowUserPointer(m_window, this);

		return true;
	}

	void NextFrame() { PollEvents(); }

	void PollEvents() override { glfwPollEvents(); }

	auto CreateSurface(vk::Instance instance) -> vk::SurfaceKHR override
	{
		VkSurfaceKHR surface = nullptr;
		if (glfwCreateWindowSurface(instance, m_window, nullptr, &surface) != VK_SUCCESS)
			return nullptr;
		return surface;
	}

	auto GetSurfaceWidth() -> uint32_t override
	{
		int32_t dim = 0;
		glfwGetFramebufferSize(m_window, &dim, nullptr);
		return dim;
	}
	auto GetSurfaceHeight() -> uint32_t override
	{
		int32_t dim = 0;
		glfwGetFramebufferSize(m_window, nullptr, &dim);
		return dim;
	}

	bool IsVSync() override { return true; }
	bool IsAlive() override { return !glfwWindowShouldClose(m_window); }

	void HideCursor() override {}
	void ShowCursor() override {}

	auto CreateCursor(uint32_t cursorType) -> void* override { return nullptr; }
	void SetCursor(void* cursor) override {}

private:
	GLFWwindow* m_window;
};

int main(int, char**)
{
	LOG_INFO("VkMana Application Template");

	Window window{};
	if (!window.Init())
	{
		LOG_ERROR("Failed to init GLFW window");
		return 1;
	}

	VkMana::Context ctx;
	if (!ctx.Init(&window))
	{
		LOG_ERROR("Failed to init VkMana context");
		return 1;
	}

	const VkMana::PipelineLayoutCreateInfo pipelineLayoutInfo{};
	auto pipelineLayout = ctx.CreatePipelineLayout(pipelineLayoutInfo);

	VkMana::ShaderCompileInfo compileInfo{
		.SrcLanguage = VkMana::SourceLanguage::HLSL,
		.SrcString = TriangleHLSLShader,
		.Stage = vk::ShaderStageFlagBits::eVertex,
		.EntryPoint = "VSMain",
		.Debug = 1,
	};
	const auto vertSpirvOpt = VkMana::CompileShader(compileInfo);
	if (!vertSpirvOpt)
	{
		LOG_ERROR("Failed to compiler VERTEX shader.");
		return 1;
	}

	compileInfo.Stage = vk::ShaderStageFlagBits::eFragment;
	compileInfo.EntryPoint = "PSMain";
	const auto fragSpirvOpt = VkMana::CompileShader(compileInfo);
	if (!fragSpirvOpt)
	{
		LOG_ERROR("Failed to compiler FRAGMENT shader.");
		return 1;
	}

	const VkMana::GraphicsPipelineCreateInfo pipelineInfo{
		.Vertex = { vertSpirvOpt.value(), "VSMain" },
		.Fragment = { fragSpirvOpt.value(), "PSMain" },
		.Topology = vk::PrimitiveTopology::eTriangleList,
		.ColorTargetFormats = { vk::Format::eB8G8R8A8Srgb },
		.Layout = pipelineLayout,
	};
	auto trianglePipeline = ctx.CreateGraphicsPipeline(pipelineInfo);

	while (window.IsAlive())
	{
		window.NextFrame();

		ctx.BeginFrame();

		auto mainCmd = ctx.RequestCmd();

		const auto windowWidth = window.GetSurfaceWidth();
		const auto windowHeight = window.GetSurfaceHeight();

		const auto rpInfo = ctx.GetSurfaceRenderPass(&window);
		mainCmd->BeginRenderPass(rpInfo);
		mainCmd->BindPipeline(trianglePipeline.Get());
		mainCmd->SetViewport(0, 0, float(windowWidth), float(windowHeight));
		mainCmd->SetScissor(0, 0, windowWidth, windowHeight);
		mainCmd->Draw(3, 0);
		mainCmd->EndRenderPass();

		ctx.Submit(mainCmd);

		ctx.EndFrame();
		ctx.Present();
	}

	return 0;
}
