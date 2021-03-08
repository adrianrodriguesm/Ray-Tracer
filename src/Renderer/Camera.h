#pragma once

#include <cmath>
#include <stdio.h>

#include "Math/vector.h"
#include "Math/ray.h"
#include "Core/Constant.h"
namespace rayTracer
{
	class Camera
	{

	private:
		Vec3 eye, at, up;
		float fovy, vnear, vfar, plane_dist, focal_ratio, aperture;
		float w, h;
		int res_x, res_y;
		Vec3 u, v, n;

	public:
		Vec3 GetEye() { return eye; }
		int GetResX() { return res_x; }
		int GetResY() { return res_y; }
		float GetFov() { return fovy; }
		float GetPlaneDist() { return plane_dist; }
		float GetFar() { return vfar; }
		float GetAperture() { return aperture; }

		Camera(Vec3 from, Vec3 At, Vec3 Up, float angle, float hither, float yon, int ResX, int ResY, float Aperture_ratio, float Focal_ratio)
			: eye(from), at(At), up(Up), fovy(angle), vnear(hither), vfar(yon), res_x(ResX), res_y(ResY), focal_ratio(Focal_ratio)
		{
			// set the camera frame uvn
			n = (eye - at);
			plane_dist = Magnitude(n);
			n = n / plane_dist;

			u = CrossProduct(up,n);
			u = u / Magnitude(u);

			v = CrossProduct(n, u);

			//Dimensions of the vis window
			h = 2 * plane_dist * tan((M_PI * angle / 180) / 2.0f);
			w = ((float)res_x / res_y) * h;

			aperture = Aperture_ratio * (w / res_x); //Lens aperture = aperture_ratio * pixel_size

			printf("\nwidth=%f height=%f fov=%f, viewplane distance=%f, pixel size=%.3f\n", w, h, fovy, plane_dist, w / res_x);
			if (Aperture_ratio != 0) printf("\nDepth-Of-Field effect enabled with a lens aperture = %.1f\n", Aperture_ratio);
		}

		void SetEye(Vec3 from) 
		{
			eye = from;
			// set the camera frame uvn
			n = (eye - at);
			plane_dist = Magnitude(n);
			n = n / plane_dist;
			u = CrossProduct(up, n);
			u = u / Magnitude(u);
			v = CrossProduct(n, u);
		}

		Ray PrimaryRay(const Vec3& pixel_sample) //  Rays cast from the Eye to a pixel sample which is in Viewport coordinates
		{
			Ray ray;
			ray.Origin = eye;
			ray.Direction = n * -plane_dist + h * (pixel_sample.y / res_y - 0.5f) * v + w * (pixel_sample.x / res_x - 0.5f) * u;
			ray.Direction = ray.Direction.Normalized();
			return ray;
		}

		Ray PrimaryRay(const Vec3& lens_sample, const Vec3& pixel_sample) // DOF: Rays cast from  a thin lens sample to a pixel sample
		{

			Vec3 ray_dir;
			Vec3 eye_offset;

			return Ray(eye_offset, ray_dir);
		}
	};
}
