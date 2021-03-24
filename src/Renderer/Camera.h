#pragma once

#include <cmath>
#include <stdio.h>
#include "Math/Matrix.h"
#include "Math/Ray.h"
#include "Core/Constant.h"
namespace rayTracer
{
	class Camera
	{

	private:
		Vec3 eye, at, up;
		float fovy, vnear, vfar, plane_dist, focal_ratio, m_Aperture, m_ApertureRatio;
		float m_Width, m_Height;
		int m_WidthRes, m_HeightRes;
		Vec3 m_XAxis, m_YAxis, m_ZAxis;
		Mat4 m_ProjectionMatrix;

	public:
		Vec3 GetEye() { return eye; }
		Vec3 GetCenter() { return at; }
		int GetResX() { return m_WidthRes; }
		int GetResY() { return m_HeightRes; }
		float GetFov() { return fovy; }
		float GetPlaneDist() { return plane_dist; }
		float GetFar() { return vfar; }
		float GetAperture() { return m_Aperture; }

		Camera(Vec3 from, Vec3 At, Vec3 Up, float angle, float hither, float yon, int ResX, int ResY, float Aperture_ratio, float Focal_ratio)
			: eye(from), at(At), up(Up), fovy(angle), vnear(hither), vfar(yon), m_WidthRes(ResX), m_HeightRes(ResY), focal_ratio(Focal_ratio), m_ApertureRatio(Aperture_ratio)
		{
			// set the camera frame Axis
			m_ZAxis = (eye - at);
			plane_dist = Magnitude(m_ZAxis);
			m_ZAxis = m_ZAxis / plane_dist;

			m_XAxis = CrossProduct(up, m_ZAxis);
			m_XAxis = m_XAxis.Normalized();

			m_YAxis = CrossProduct(m_ZAxis, m_XAxis);

			//Dimensions of the vis window
			m_Height = 2 * plane_dist * tan((M_PI * angle / 180) / 2.0f);
			m_Width = ((float)m_WidthRes / m_HeightRes) * m_Height;

			m_Aperture = Aperture_ratio * (m_Width / m_WidthRes); //Lens aperture = aperture_ratio * pixel_size

			printf("\nwidth=%f height=%f fov=%f, viewplane distance=%f, pixel size=%.3f\n", m_Width, m_Height, fovy, plane_dist, m_Width / m_WidthRes);
			if (Aperture_ratio != 0) printf("\nDepth-Of-Field effect enabled with a lens aperture = %.1f\n", Aperture_ratio);
		}

		virtual ~Camera() {};

		// Projection Matrix
		inline const Mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		void SetProjectionMatrix(const Mat4& projection)
		{
			m_ProjectionMatrix = projection;
		}

		void SetEye(Vec3 from) 
		{
			eye = from;
			// set the camera frame uvn
			m_ZAxis = (eye - at);
			plane_dist = Magnitude(m_ZAxis);
			m_ZAxis = m_ZAxis / plane_dist;
			m_XAxis = CrossProduct(up, m_ZAxis);
			m_XAxis = m_XAxis.Normalized();
			m_YAxis = CrossProduct(m_ZAxis, m_XAxis);
		}

		void OnResize(int width, int height)
		{
			m_WidthRes = width;
			m_HeightRes = height;
			//Dimensions of the vis window
			m_Height = 2 * plane_dist * tan((M_PI * fovy / 180) / 2.0f);
			m_Width = ((float)m_WidthRes / m_HeightRes) * m_Height;

			m_Aperture = m_ApertureRatio * (m_Width / m_WidthRes); //Lens aperture = aperture_ratio * pixel_size
		}

		Ray PrimaryRay(const Vec2& pixel_sample) //  Rays cast from the Eye to a pixel sample which is in Viewport coordinates
		{
			Ray ray;
			ray.Origin = eye;
			ray.Direction = m_ZAxis * -plane_dist + m_Height * (pixel_sample.y / m_HeightRes - 0.5f) * m_YAxis + m_Width * (pixel_sample.x / m_WidthRes - 0.5f) * m_XAxis;
			ray.Direction = ray.Direction.Normalized();
			return ray;
		}

		Ray PrimaryRay(const Vec3& lens_sample, const Vec3& pixel_sample) // DOF: Rays cast from  a thin lens sample to a pixel sample
		{

			Vec3 ray_dir;
			Vec3 eye_offset;

			return Ray(eye_offset, ray_dir);
		}

		void DumpSelf()
		{
			printf("\nCamera:\n");
			std::cout << "Eye = " << eye << "\n";
			std::cout << "Center = " << at << "\n";
			std::cout << "Up = " << up << "\n";
			std::cout << "Width x Height = " << m_WidthRes << " x " << m_HeightRes << "\n";
		}
	};
}
