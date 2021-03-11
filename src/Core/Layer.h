#pragma once

namespace rayTracer
{
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		virtual void OnUploadScene() = 0;

		virtual void OnWindowClose() = 0;
		virtual void OnWindowResize(int w, int h) = 0;

		virtual void OnMouseKeyPress(int button, int state, int xx, int yy) = 0;
		virtual void OnMouseMove(int xx, int yy) = 0;
		virtual void OnMouseScroll(int wheel, int direction, int x, int y) = 0;

		virtual void OnKeyPress(unsigned char key, int xx, int yy) = 0;
		

	};
}