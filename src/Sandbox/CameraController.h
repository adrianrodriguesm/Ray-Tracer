#pragma once
#include <Renderer/Camera.h>
#include <Math/Vector.h>

class CameraController
{
public:
	CameraController() = default;
	CameraController(rayTracer::Camera* camera);
	virtual ~CameraController() = default;

	void OnMouseKeyPress(int button, int state, int xx, int yy);
	void OnMouseMove(int xx, int yy);
	void OnMouseScroll(int direction);
	void OnResize(int width, int height);
	void ResetPosition();
	void DisplayCameraInfo();
	rayTracer::Camera& GetCamera() { return *m_Camera; }
	float IncreaseAperture(float value);
private:
	rayTracer::Camera* m_Camera = nullptr;
	//Original Camera position;
	rayTracer::Vec3 m_StartPos = {};

	// Mouse Tracking Variables
	rayTracer::Vec2 m_LastPos = {};
	int m_Tracking = 0;

	// Camera Spherical Coordinates
	float m_Alpha = 0.0f, m_Beta = 0.0f;
	float m_R = 4.0f;
};