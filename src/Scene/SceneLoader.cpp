#include "SceneLoader.h"
#include "Core/Application.h"
namespace rayTracer
{
	static void next_token(std::ifstream& file, char* token, const char* name)
	{
		file >> token;
		if (strcmp(token, name))
			std::cerr << "'" << name << "' expected.\n";
	}
	Scene* SceneLoader::LoadP3D(const std::string filepath)
	{
		const	int	lineSize = 1024;
		std::string	cmd;
		char		token[256];
		std::ifstream	file(filepath, std::ios::in);
		Material* material = nullptr;
		Scene* scene = new Scene();
		if (file >> cmd)
		{
			while (true)
			{

				if (cmd == "f")   //Material
				{
					float Kd, Ks, Shine, T, ior;
					Vec3 cd, cs;

					file >> cd >> Kd >> cs >> Ks >> Shine >> T >> ior;

					material = new Material(cd, Kd, cs, Ks, Shine, T, ior);
				}

				else if (cmd == "s")    //Sphere
				{
					Vec3 center;
					float radius;
					Sphere* sphere;

					file >> center >> radius;
					sphere = new Sphere(center, radius);
					if (material) sphere->SetMaterial(material);
					scene->AddObject((Object*)sphere);
				}

				else if (cmd == "box")    //axis aligned box
				{
					Vec3 minpoint, maxpoint;
					aaBox* box;

					file >> minpoint >> maxpoint;
					box = new aaBox(minpoint, maxpoint);
					if (material) box->SetMaterial(material);
					scene->AddObject((Object*)box);
				}
				else if (cmd == "p")  // Polygon: just accepts triangles for now
				{
					Vec3 P0, P1, P2;
					Triangle* triangle;
					unsigned total_vertices;

					file >> total_vertices;
					if (total_vertices == 3)
					{
						file >> P0 >> P1 >> P2;
						triangle = new Triangle(P0, P1, P2);
						if (material) triangle->SetMaterial(material);
						scene->AddObject((Object*)triangle);
					}
					else
					{
						std::cerr << "Unsupported number of vertices.\n";
						break;
					}
				}

				else if (cmd == "mesh") {
					unsigned total_vertices, total_faces;
					unsigned P0, P1, P2;
					Triangle* triangle;
					Vec3* verticesArray, vertex;

					file >> total_vertices >> total_faces;
					verticesArray = (Vec3*)malloc(total_vertices * sizeof(Vec3));
					for (uint32_t i = 0; i < total_vertices; i++) {
						file >> vertex;
						verticesArray[i] = vertex;
					}
					for (uint32_t i = 0; i < total_faces; i++) {
						file >> P0 >> P1 >> P2;
						triangle = new Triangle(verticesArray[P0 - 1], verticesArray[P1 - 1], verticesArray[P2 - 1]); //vertex index start at 1
						if (material) triangle->SetMaterial(material);
						scene->AddObject((Object*)triangle);
					}

				}

				else if (cmd == "pl")  // General Plane
				{
					Vec3 P0, P1, P2;
					Plane* plane;

					file >> P0 >> P1 >> P2;
					plane = new Plane(P0, P1, P2);
					if (material) plane->SetMaterial(material);
					scene->AddObject((Object*)plane);
				}

				else if (cmd == "l")  // Need to check light color since by default is white
				{
					Vec3 pos;
					Vec3 color;

					file >> pos >> color;

					scene->AddLight(new Light(pos, color));

				}
				else if (cmd == "v")
				{
					Vec3 up, from, at;
					float fov, hither;
					int xres, yres;
					Camera* camera;
					float focal_ratio; //ratio beteween the focal distance and the viewplane distance
					float aperture_ratio; // number of times to be multiplied by the size of a pixel

					next_token(file, token, "from");
					file >> from;

					next_token(file, token, "at");
					file >> at;

					next_token(file, token, "up");
					file >> up;

					next_token(file, token, "angle");
					file >> fov;

					next_token(file, token, "hither");
					file >> hither;

					next_token(file, token, "resolution");
					file >> xres >> yres;

					next_token(file, token, "aperture");
					file >> aperture_ratio;

					next_token(file, token, "focal");
					file >> focal_ratio;
					// Create Camera
					AplicationSpecification appSpec = Application::Get().GetSpecification();
					camera = new Camera(from, at, up, fov, hither, 100.0f * hither, appSpec.Width, appSpec.Height, aperture_ratio, focal_ratio);
					scene->SetCamera(camera);
				}

				else if (cmd == "bclr")   //Background color
				{
					Vec3 bgcolor;
					file >> bgcolor;
					scene->SetBackgroundColor(bgcolor);
				}

				else if (cmd == "env")
				{
					file >> token;

					scene->LoadSkybox(token);
					scene->SetSkyBoxFlg(true);
				}
				else if (cmd[0] == '#')
				{
					file.ignore(lineSize, '\n');
				}
				else
				{
					std::cerr << "unknown command '" << cmd << "'.\n";
					break;
				}
				if (!(file >> cmd))
					break;
			}
		}

		file.close();
		return scene;
	}
}

