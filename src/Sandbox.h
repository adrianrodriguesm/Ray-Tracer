#pragma once
#include "RayTracer.h"
using namespace rayTracer;
class Sandbox : public Layer
{
public:
	Sandbox();
	virtual ~Sandbox();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate() override;

	virtual void OnWindowClose() override;
	virtual void OnWindowResize(int w, int h) override;

	virtual void OnMouseKeyPress(int button, int state, int xx, int yy) override;
	virtual void OnMouseMove(int xx, int yy) override;
	virtual void OnMouseScroll(int wheel, int direction, int x, int y) override;

	virtual void OnKeyPress(unsigned char key, int xx, int yy) override;

private:
	void InitScene();
private:
	Camera* m_Camera = nullptr;
	Scene* m_Scene = nullptr;
	uint32_t RES_WIDTH = 512, RES_HEIGHT = 512;
	const uint32_t MAX_DEPTH = 3;

	// Camera Move
	// Current Camera Position
	float camX, camY, camZ;

	//Original Camera position;
	Vec3 Eye = {};

	// Mouse Tracking Variables
	int startX, startY, tracking = 0;

	// Camera Spherical Coordinates
	float alpha = 0.0f, beta = 0.0f;
	float r = 4.0f;
};