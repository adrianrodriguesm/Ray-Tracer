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
		virtual ~Object();
		Material* GetMaterial() { return m_Material; }
		void SetMaterial(Material* a_Mat) { m_Material = a_Mat; }
		virtual RayCastHit Intercepts(Ray& r) = 0;
		virtual Vec3 GetNormal(Vec3 point) = 0;
		virtual AABB GetBoundingBox() = 0;
		virtual bool IsInsideObject(const Vec3& point, const Ray& ray) = 0;

	protected:
		Material* m_Material = nullptr;

	};

	class Plane : public Object
	{
	public:
		Plane(Vec3& PNc, float Dc);
		Plane(Vec3& P0, Vec3& P1, Vec3& P2);
		virtual ~Plane() = default;
		virtual RayCastHit Intercepts(Ray& r) override;
		
		virtual AABB GetBoundingBox() { return m_BoundingBox; }
		virtual Vec3 GetNormal(Vec3 point) override { return m_Normal; }
		virtual bool IsInsideObject(const Vec3& point, const Ray& ray) { return false; }
	private:
		void CalculateAABB(Vec3& P0, Vec3& P1, Vec3& P2);
	private:
		Vec3 m_Normal;
		AABB m_BoundingBox;
		float m_Distance;
	};

	class Triangle : public Object
	{
	public:
		Triangle(Vec3& P0, Vec3& P1, Vec3& P2);
		virtual ~Triangle() = default;
		virtual RayCastHit Intercepts(Ray& r) override;
		virtual Vec3 GetNormal(Vec3 point) override { return m_Normal; }
		virtual AABB GetBoundingBox(void) override { return m_BoundingBox; }
		virtual bool IsInsideObject(const Vec3& point, const Ray& ray) { return false; }
	private:
		void CalculateAABB();
		Vec3 m_Points[3], m_Normal;
		AABB m_BoundingBox;
	};


	class Sphere : public Object
	{
	public:
		Sphere() : m_Center(0), SqRadius(1),
			m_Radius(1) 
		{
			CalculateAABB();
		};
		Sphere(Vec3& a_center, float a_radius) :
			m_Center(a_center), SqRadius(a_radius * a_radius),
			m_Radius(a_radius)
		{
			CalculateAABB();
		};
		virtual ~Sphere() = default;
		virtual RayCastHit Intercepts(Ray& r) override;
		virtual Vec3 GetNormal(Vec3 point) override;
		virtual AABB GetBoundingBox(void) override { return m_BoundingBox; }
		virtual bool IsInsideObject(const Vec3& point, const Ray& ray);

		float GetRadius() { return m_Radius; }
		Vec3 GetCenter() { return m_Center; }

	protected:
		void CalculateAABB();

		AABB m_BoundingBox;
		Vec3 m_Center;
		float m_Radius, SqRadius;
	};

	class Box : public Object 
	{
	public:
		Box(Vec3& minPoint, Vec3& maxPoint);
		virtual ~Box() = default;
		virtual AABB GetBoundingBox(void) override { return m_BoundingBox; }
		virtual RayCastHit Intercepts(Ray& r) override;
		virtual Vec3 GetNormal(Vec3 point) override;
		virtual bool IsInsideObject(const Vec3& point, const Ray& ray);//TODO: Implement

	private:
		AABB m_BoundingBox;
		Vec3 m_Center;
	};

	class BubbleSphere : public Sphere
	{
	public:
		BubbleSphere(Vec3& a_center, float a_radiusfloat, float _perlingFreq, float bubbleLowLimit, Material* childMaterial) ;
		
		// Ray Tracing
		RayCastHit Intercepts(Ray& ray);

		// Getters / Setters
		std::vector<Sphere*> GetChildSpheres();
		void SetSampleStep(float step);
		void SetBubbleThreshold(float threshold);
		void SetPerlinSamplingFreq(float frequency);

		// Print
		friend std::ostream& operator << (std::ostream& stream, const BubbleSphere& sphere);

	private:
		void GenerateChildSpheres();
		RayCastHit GetClosestHitInChildSpheres(Ray& ray, float tmin);

		float m_Step = 0.05f; // step used in generation of childspheres
		float m_BubbleThreshold = 0.7f;
		float m_PerlinSampingFreq;
		std::vector<Sphere*> m_ChildSpheres;
		Material* m_ChildMaterial;
	};
}