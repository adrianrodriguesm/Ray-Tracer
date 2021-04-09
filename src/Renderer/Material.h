#pragma once
#include "Renderer/Color.h"

namespace rayTracer
{
	class Material
	{
	public:

		Material() :
			m_diffColor(Vec3(0.2f, 0.2f, 0.2f)), m_Diff(0.2f), m_specColor(Vec3(1.0f, 1.0f, 1.0f)), m_Spec(0.8f), m_Shine(20), m_Refl(1.0f), m_Transmittance(0.0f), m_RIndex(1.0f), m_Roughness(0.f) {};

		Material(Vec3& c, float Kd, Vec3& cs, float Ks, float Shine, float T, float ior, float roughness) 
			: m_diffColor(c), m_Diff(Kd), m_specColor(cs), m_Spec(Ks), m_Shine(Shine), m_Refl(Ks), m_Transmittance(T), m_RIndex(ior), m_Roughness(roughness)
		{
			//m_diffColor = c; m_Diff = Kd; m_specColor = cs; m_Spec = Ks; m_Shine = Shine; m_Refl = Ks; m_T = T; m_RIndex = ior;
		}

		virtual ~Material() = default;

		void SetDiffColor(Vec3& a_Color) { m_diffColor = a_Color; }
		Vec3 GetDiffColor() { return m_diffColor; }
		void SetSpecColor(Vec3& a_Color) { m_specColor = a_Color; }
		Vec3 GetSpecColor() { return m_specColor; }
		void SetDiffuse(float a_Diff) { m_Diff = a_Diff; }
		void SetSpecular(float a_Spec) { m_Spec = a_Spec; }
		void SetShine(float a_Shine) { m_Shine = a_Shine; }
		void SetReflection(float a_Refl) { m_Refl = a_Refl; }
		void SetTransmittance(float a_T) { m_Transmittance = a_T; }
		float GetSpecular() { return m_Spec; }
		float GetDiffuse() { return m_Diff; }
		float GetShine() { return m_Shine; }
		float GetReflection() { return m_Refl; }
		float GetTransmittance() { return m_Transmittance; }
		void SetRefrIndex(float a_ior) { m_RIndex = a_ior; }
		float GetRefrIndex() { return m_RIndex; }
		float GetRoughness() { return m_Roughness; }
	private:
		Vec3 m_diffColor, m_specColor;
		float m_Refl, m_Transmittance;
		float m_Diff, m_Shine, m_Spec;
		float m_RIndex;
		float m_Roughness;
	};

}