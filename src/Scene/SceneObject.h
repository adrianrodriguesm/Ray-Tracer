#pragma once
#include "Math/Vector.h"
#include "Math/BoundingBox.h"
#include "Renderer/Material.h"
#include "Math/Ray.h"
namespace rayTracer
{
	class Object
	{
	public:

		Material* GetMaterial() { return m_Material; }
		void SetMaterial(Material* a_Mat) { m_Material = a_Mat; }
		virtual RayCastHit Intercepts(Ray& r) = 0;
		virtual Vec3 GetNormal(Vec3 point) = 0;
		virtual AABB GetBoundingBox() { return AABB(); }

	protected:
		Material* m_Material = nullptr;

	};

	class Plane : public Object
	{
	public:
		// TODO : CHECK IF NECESSARY
		//Plane(Vector& PNc, float Dc);
		Plane(Vec3& P0, Vec3& P1, Vec3& P2);

		virtual RayCastHit Intercepts(Ray& r) override;
		//RayCastHit Intercepts(Ray& r, float& dist);
		virtual Vec3 GetNormal(Vec3 point) override { return m_Normal; }
	private:
		Vec3 m_Vertices[3];
		Vec3 m_Normal;
	};

	class Triangle : public Object
	{
	public:
		Triangle(Vec3& P0, Vec3& P1, Vec3& P2);
		virtual RayCastHit Intercepts(Ray& r) override;
		virtual Vec3 GetNormal(Vec3 point) override { return m_Normal; }
		virtual AABB GetBoundingBox(void) override { return m_BoundingBox; }

	protected:
		void CalculateAABB();
		Vec3 m_Points[3], m_Normal;
		AABB m_BoundingBox;
	};


	class Sphere : public Object
	{
	public:
		Sphere(Vec3& a_center, float a_radius) :
			m_Center(a_center), SqRadius(a_radius * a_radius),
			m_Radius(a_radius)
		{
			CalculateAABB();
		};

		virtual RayCastHit Intercepts(Ray& r) override;
		virtual Vec3 GetNormal(Vec3 point) override;
		virtual AABB GetBoundingBox(void) override { return m_BoundingBox; }

	private:
		void CalculateAABB();

		AABB m_BoundingBox;
		Vec3 m_Center;
		float m_Radius, SqRadius;
	};

	class aaBox : public Object   //Axis aligned box: another geometric object
	{
	public:
		aaBox(Vec3& minPoint, Vec3& maxPoint);
		virtual AABB GetBoundingBox(void) override { return m_BoundingBox; }
		virtual RayCastHit Intercepts(Ray& r) override;
		virtual Vec3 GetNormal(Vec3 point) override { return m_Normal; }

	private:
		AABB m_BoundingBox;
		Vec3 m_Normal;
	};
}