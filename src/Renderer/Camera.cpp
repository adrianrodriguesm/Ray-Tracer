#include "Camera.h"
#include "Core/Constant.h"
#include "Math/Random.h"
namespace rayTracer
{
	Camera::Camera(Vec3 from, Vec3 At, Vec3 Up, float angle, float hither, float yon, int ResX, int ResY, float Aperture_ratio, float Focal_ratio)
		: m_Eye(from), m_LookAt(At), m_Up(Up), m_FOV(angle), m_Near(hither), m_Far(yon), m_WidthRes(ResX), m_HeightRes(ResY), m_FocalRatio(Focal_ratio), m_ApertureRatio(Aperture_ratio)
	{
		// set the camera frame Axis
		m_ZAxis = (m_Eye - m_LookAt);
		m_PlaneDistance = Magnitude(m_ZAxis);
		m_ZAxis = m_ZAxis / m_PlaneDistance;

		m_XAxis = CrossProduct(m_Up, m_ZAxis);
		m_XAxis = m_XAxis.Normalized();

		m_YAxis = CrossProduct(m_ZAxis, m_XAxis);

		//Dimensions of the vis window
		m_Height = 2 * m_PlaneDistance * tan((M_PI * angle / 180) / 2.0f);
		m_Width = ((float)m_WidthRes / m_HeightRes) * m_Height;

		m_Aperture = Aperture_ratio * (m_Width / m_WidthRes); //Lens aperture = aperture_ratio * pixel_size

		printf("\nwidth=%f height=%f fov=%f, viewplane distance=%f, pixel size=%.3f\n", m_Width, m_Height, m_FOV, m_PlaneDistance, m_Width / m_WidthRes);
		if (Aperture_ratio != 0) printf("\nDepth-Of-Field effect enabled with a lens aperture = %.1f\n", Aperture_ratio);
	}
	void Camera::SetEye(Vec3 from)
    {
		m_Eye = from;
		// set the camera frame uvn
		m_ZAxis = (m_Eye - m_LookAt);
		m_PlaneDistance = Magnitude(m_ZAxis);
		m_ZAxis = m_ZAxis / m_PlaneDistance;
		m_XAxis = CrossProduct(m_Up, m_ZAxis);
		m_XAxis = m_XAxis.Normalized();
		m_YAxis = CrossProduct(m_ZAxis, m_XAxis);
		
    }
	void Camera::OnResize(int width, int height)
	{
		m_WidthRes = width;
		m_HeightRes = height;
		//Dimensions of the vis window
		m_Height = 2 * m_PlaneDistance * tan((M_PI * m_FOV / 180) / 2.0f);
		m_Width = ((float)m_WidthRes / m_HeightRes) * m_Height;

		m_Aperture = m_ApertureRatio * (m_Width / m_WidthRes); //Lens aperture = aperture_ratio * pixel_size
	}
	Ray Camera::PrimaryRay(const Vec2& pixelSample)
	{
		Ray ray;
		ray.Origin = m_Eye;
		ray.Direction = m_ZAxis * -m_PlaneDistance + m_Height * (pixelSample.y / m_HeightRes - 0.5f) * m_YAxis + m_Width * (pixelSample.x / m_WidthRes - 0.5f) * m_XAxis;
		ray.Direction = ray.Direction.Normalized();
		return ray;
	}
	Ray Camera::PrimaryCenterRay(const Vec2& pixelSample)
	{
		Ray ray;
		ray.Origin = m_Eye;
		ray.Direction = m_ZAxis * -m_PlaneDistance + m_Height * (pixelSample.y / m_HeightRes - 0.5f) * m_YAxis + m_Width * (pixelSample.x / m_WidthRes - 0.5f) * m_XAxis;
		ray.Direction = ray.Direction.Normalized();
		return ray;
	}
	Ray Camera::PrimaryLensRay(const Vec2& pixelSample)
	{
		Vec2 lensSample = CalculateSamplePoint();
		Vec3 rayOrigin = m_Eye + lensSample.x * m_XAxis + lensSample.y * m_YAxis;
		// Dir eye space
		Vec3 dirFromCenter;
		dirFromCenter.x = m_Width * (pixelSample.x / m_WidthRes - 0.5f) * m_FocalRatio;
		dirFromCenter.y = m_Height * (pixelSample.y / m_HeightRes - 0.5f) * m_FocalRatio;
		dirFromCenter.z = -m_PlaneDistance;
		Vec3 rayDir = (dirFromCenter.x - lensSample.x) * m_XAxis + (dirFromCenter.y - lensSample.y) * m_YAxis + dirFromCenter.z * m_ZAxis;
		rayDir.Normalized();
		return Ray(rayOrigin, rayDir);
	}
	void Camera::DumpSelf()
	{
		printf("\nCamera:\n");
		std::cout << "Eye = " << m_Eye << "\n";
		std::cout << "Center = " << m_LookAt << "\n";
		std::cout << "Up = " << m_Up << "\n";
		std::cout << "Width x Height = " << m_WidthRes << " x " << m_HeightRes << "\n";
	}
	Vec2 Camera::CalculateSamplePoint()
	{
		Vec2 samplePoint;
		// Calculate the sample point base on a unit disk
		do
		{
			samplePoint = Vec2(Random::Float(), Random::Float()) * 2 - Vec2( 1.0f, 1.0f);
		}
		while (samplePoint.SqrMagnitude() >= 1.0);
		samplePoint = { samplePoint.x * m_Aperture, samplePoint.y * m_Aperture };
		return samplePoint;
	}
}