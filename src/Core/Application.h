#pragma once
#include "Core/Layer.h"
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
namespace rayTracer
{
	enum class RenderMode
	{
		DrawMode, GenerateImage
	};
	struct AplicationSpecification
	{
		Layer* AppLayer = nullptr;
		std::string Caption = "Whitted Ray-Tracer";
		RenderMode Mode = RenderMode::DrawMode;
		int Width = 512, Height = 512;
		// Check if necessary
		int argc; char** argv;
	};
	class Application
	{
	public:
		Application(AplicationSpecification spec);
		virtual ~Application();

		void Run();
		static Application& Get() { return *s_Instance; }
		int GetNativeWindow() const { return m_GlutWindow; }
		const AplicationSpecification& GetSpecification() const { return m_Spec; }
		Layer& GetLayer() { return *m_Spec.AppLayer; }

		
	private:
		void InitGraphicContext();
		void InitWindow();
		void SetCallBacks();

	private:
		static Application* s_Instance;
		AplicationSpecification m_Spec;
		int m_GlutWindow;
	};
}