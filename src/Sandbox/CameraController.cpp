#include "pch.h"
#include "CameraController.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
#include <GL/freeglut_std.h>

CameraController::CameraController(rayTracer::Camera* camera)
	: m_Camera(camera), m_StartPos(m_Camera->GetEye()), 
	  m_R((m_Camera->GetCenter() - m_Camera->GetEye()).Magnitude())
{
}

void CameraController::OnMouseKeyPress(int button, int state, int xx, int yy)
{
	// Start tracking the mouse
	if (state == GLUT_DOWN)
	{
		m_LastPos = { (float)xx , (float)yy };
		if (button == GLUT_LEFT_BUTTON)
			m_Tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			m_Tracking = 2;
	}

	// Stop tracking the mouse
	else if (state == GLUT_UP)
		m_Tracking = 0;
	
}

void CameraController::OnMouseMove(int xx, int yy)
{
	if (m_Tracking == 0)
		return;
	rayTracer::Vec3 finalPos = m_Camera->GetEye();
	int deltaX, deltaY;

	deltaX = -xx + (int)m_LastPos.x;
	deltaY = yy - (int)m_LastPos.y;
	m_LastPos = { (float)xx , (float)yy };

	// Left mouse button: move camera
	if (m_Tracking == 1)
	{
		m_Alpha += deltaX;
		m_Beta += deltaY;

		if (m_Beta > 85.0f)
			m_Beta = 85.0f;
		else if (m_Beta < -85.0f)
			m_Beta = -85.0f;

		auto& center = m_Camera->GetCenter();

		rayTracer::Qtrn pitch = rayTracer::Qtrn(m_Beta, { -1,0,0 });
		rayTracer::Qtrn yaw = rayTracer::Qtrn(m_Alpha, { 0,1,0 });
		rayTracer::Qtrn rot = pitch * yaw;
		rayTracer::Qtrn rotatedPoint = rot * rayTracer::Qtrn(0, m_StartPos.x, m_StartPos.y, m_StartPos.z) * conjugate(rot);
		// Translate back
		finalPos = { rotatedPoint.x + center.x, rotatedPoint.y + center.y, rotatedPoint.z + center.z}; 
	}
	// Right mouse button: zoom
	else if (m_Tracking == 2)
	{
		m_R = m_R + (deltaY * 0.01f);
		if (m_R < 0.1f)
			m_R = 0.1f;

		finalPos += Normalize(finalPos - m_Camera->GetCenter()) * m_R;
	}

	m_Camera->SetEye(finalPos);
}

void CameraController::OnMouseScroll(int direction)
{
	m_R += direction * 0.1f;
	if (m_R < 0.1f)
		m_R = 0.1f;
}

void CameraController::OnResize(int width, int height)
{
	rayTracer::Mat4 ortho = rayTracer::MatFactory::CreateOrthographicProjectionMatrix(0, (float)width, 0, (float)height, 
																					-1.0, 1.0);
	m_Camera->SetProjectionMatrix(ortho);
	m_Camera->OnResize(width, height);
}

void CameraController::ResetPosition()
{
	m_Camera->SetEye(m_StartPos);
	m_R = rayTracer::Magnitude(m_StartPos - m_Camera->GetCenter());
	m_Beta = 0; 
	m_Alpha = 0;
}

void CameraController::DisplayCameraInfo()
{
	printf("Camera Spherical Coordinates (%f, %f, %f)\n", m_R, m_Beta, m_Alpha);
	m_Camera->DumpSelf();
}

float CameraController::IncreaseAperture(float value)
{
	return m_Camera->SetAperture(m_Camera->GetAperture() + value);
}
