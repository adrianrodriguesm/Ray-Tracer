#pragma once
#include "Core/Layer.h"

extern int main(int argc, char** argv);
namespace rayTracer
{
	enum class RenderMode
	{
		DrawMode, GenerateImage, Default = DrawMode
	};
	struct ApplicationSpecification
	{
		Layer* AppLayer = nullptr;
		std::string Caption = "Distribution Ray-Tracer";
		RenderMode Mode = RenderMode::DrawMode;
		int Width = 512, Height = 512;
		// TODO: Check if necessary
		int argc; char** argv;
	};
	class Application
	{
	private:
		// This is private because is Singleton
		Application(ApplicationSpecification spec);
		virtual ~Application();
		friend int ::main(int argc, char** argv);
	public:
		void Run();
		static Application& Get() { return *s_Instance; }
		int GetNativeWindow() const { return m_GlutWindow; }
		const ApplicationSpecification& GetSpecification() const { return m_Spec; }
		Layer& GetLayer() { return *m_Spec.AppLayer; }
		inline void SetResolution(int width, int height) { m_Spec.Width = width; m_Spec.Height = height; }

		
	private:
		void InitGraphicContext();
		void InitWindow();
		void SetCallBacks();

	private:
		static Application* s_Instance;
		ApplicationSpecification m_Spec;
		int m_GlutWindow;
	};
}