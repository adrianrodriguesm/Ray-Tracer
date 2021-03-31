#pragma once
#include "Math/Matrix.h"
#include "Math/Ray.h"
namespace rayTracer
{
	class Camera
	{
	public:
		Camera(Vec3 from, Vec3 At, Vec3 Up, float angle, float hither, float yon, int ResX, int ResY, float Aperture_ratio, float Focal_ratio);
		Camera() = default;
		virtual ~Camera() = default;

		void SetProjectionMatrix(const Mat4& projection) { m_ProjectionMatrix = projection; }
		void SetEye(Vec3 from);
		// Projection Matrix
		inline const Mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }	
		inline Vec3 GetEye() { return m_Eye; }
		inline Vec3 GetCenter() { return m_LookAt; }
		inline int GetResX() { return m_WidthRes; }
		inline int GetResY() { return m_HeightRes; }
		inline float GetFov() { return m_FOV; }
		inline float GetPlaneDistance() { return m_PlaneDistance; }
		inline float GetNear() { return m_Near; }
		inline float GetFar() { return m_Far; }
		inline float GetAperture() { return m_Aperture; }

		void OnResize(int width, int height);

		//Ray PrimaryRay(const Vec2& pixelSample);
		Ray PrimaryCenterRay(const Vec2& pixelSample);
		Ray PrimaryLensRay(const Vec2& pixelSample);

		void DumpSelf();
	private:
		Vec2 CalculateSamplePoint();
	private:
		Vec3 m_Eye, m_LookAt, m_Up;
		float m_FOV, m_Near, m_Far, m_PlaneDistance, m_FocalRatio, m_Aperture, m_ApertureRatio;
		float m_Width, m_Height;
		int m_WidthRes, m_HeightRes;
		Vec3 m_XAxis, m_YAxis, m_ZAxis;
		Mat4 m_ProjectionMatrix;
	};
}
