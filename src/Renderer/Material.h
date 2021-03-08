#pragma once
#include "Renderer/Color.h"

namespace rayTracer
{
	class Material
	{
	public:

		Material() :
			m_diffColor(Color(0.2f, 0.2f, 0.2f)), m_Diff(0.2f), m_specColor(Color(1.0f, 1.0f, 1.0f)), m_Spec(0.8f), m_Shine(20), m_Refl(1.0f), m_T(0.0f), m_RIndex(1.0f) {};

		Material(Color& c, float Kd, Color& cs, float Ks, float Shine, float T, float ior) {
			m_diffColor = c; m_Diff = Kd; m_specColor = cs; m_Spec = Ks; m_Shine = Shine; m_Refl = Ks; m_T = T; m_RIndex = ior;
		}

		void SetDiffColor(Color& a_Color) { m_diffColor = a_Color; }
		Color GetDiffColor() { return m_diffColor; }
		void SetSpecColor(Color& a_Color) { m_specColor = a_Color; }
		Color GetSpecColor() { return m_specColor; }
		void SetDiffuse(float a_Diff) { m_Diff = a_Diff; }
		void SetSpecular(float a_Spec) { m_Spec = a_Spec; }
		void SetShine(float a_Shine) { m_Shine = a_Shine; }
		void SetReflection(float a_Refl) { m_Refl = a_Refl; }
		void SetTransmittance(float a_T) { m_T = a_T; }
		float GetSpecular() { return m_Spec; }
		float GetDiffuse() { return m_Diff; }
		float GetShine() { return m_Shine; }
		float GetReflection() { return m_Refl; }
		float GetTransmittance() { return m_T; }
		void SetRefrIndex(float a_ior) { m_RIndex = a_ior; }
		float GetRefrIndex() { return m_RIndex; }
	private:
		Color m_diffColor, m_specColor;
		float m_Refl, m_T;
		float m_Diff, m_Shine, m_Spec;
		float m_RIndex;
	};

}