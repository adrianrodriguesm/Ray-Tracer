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
					float Kd, Ks, Shine, T, ior, roughness;
					Vec3 cd, cs;

					file >> cd >> Kd >> cs >> Ks >> Shine >> T >> ior;
					if(file.peek() == '\r' || file.peek() == '\n')
						material = new Material(cd, Kd, cs, Ks, Shine, T, ior);
					else {
						file >> roughness;
						material = new Material(cd, Kd, cs, Ks, Shine, T, ior, roughness);
					}

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
					Box* box;

					file >> minpoint >> maxpoint;
					box = new Box(minpoint, maxpoint);
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

				else if (cmd == "larea")  // Area Light
				{
					Vec3 pos;
					Vec3 color;
					Vec3 side1;
					Vec3 side2;

					file >> pos >> color >> side1 >> side2;

					scene->AddLight(new AreaLight(pos, color, side1, side2));

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
					std::string skyboxFilepath(token);
					skyboxFilepath = "assets/" + skyboxFilepath;
					scene->LoadSkybox(skyboxFilepath.c_str());
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

	Scene* SceneLoader::Create_random_scene() {
		Camera* camera;
		Material* material;
		Sphere* sphere;
		Scene* scene = new Scene();

		set_rand_seed(time(NULL) * time(NULL) * time(NULL));
		material = NULL;
		scene->SetSkyBoxFlg(false);  //init with no skybox

		scene->SetBackgroundColor(Vec3(0.5, 0.7, 1.0));
		//scene->LoadSkybox("skybox");
		//scene->SetSkyBoxFlg(true);

		camera = new Camera(Vec3(13.0, 2.0, 3.0), Vec3(0.0, 0.0, 0), Vec3(0.0, 1.0, 0.0), 45.0, 0.01, 10000.0, 512, 512, 0, 1.5f);
		scene->SetCamera(camera);

		scene->AddLight(new Light(Vec3(7, 10, -5), Vec3(1.0, 1.0, 1.0)));
		scene->AddLight(new Light(Vec3(-7, 10, -5), Vec3(1.0, 1.0, 1.0)));
		scene->AddLight(new Light(Vec3(0, 10, 7), Vec3(1.0, 1.0, 1.0)));

		material = new Material(Vec3(0.5, 0.5, 0.5), 1.0, Vec3(0.0, 0.0, 0.0), 0.0, 10, 0, 1);


		sphere = new Sphere(Vec3(0.0, -1000, 0.0), 1000.0);
		if (material) sphere->SetMaterial(material);
		scene->AddObject((Object*)sphere);

		for (int a = -5; a < 5; a++)
			for (int b = -5; b < 5; b++) {

				float choose_mat = Random::Float();

				Vec3 center = Vec3(a + 0.9 * Random::Float(), 0.2, b + 0.9 * Random::Float());

				if ((center - Vec3(4.0, 0.2, 0.0)).Magnitude() > 0.9) {
					if (choose_mat < 0.4) {  //diffuse
						material = new Material(Vec3(Random::Float(), Random::Float(), Random::Float()), 1.0, Vec3(0.0, 0.0, 0.0), 0.0, 10, 0, 1);
						sphere = new Sphere(center, 0.2);
						if (material) sphere->SetMaterial(material);
						scene->AddObject((Object*)sphere);
					}
					else if (choose_mat < 0.9) {   //metal
						material = new Material(Vec3(0.0, 0.0, 0.0), 0.0, Vec3(Random::Float(0.5, 1), Random::Float(0.5, 1), Random::Float(0.5, 1)), 1.0, 220, 0, 1);
						sphere = new Sphere(center, 0.2);
						if (material) sphere->SetMaterial(material);
						scene->AddObject((Object*)sphere);
					}
					else {   //glass 
						material = new Material(Vec3(0.0, 0.0, 0.0), 0.0, Vec3(1.0, 1.0, 1.0), 0.7, 20, 1, 1.5);
						sphere = new Sphere(center, 0.2);
						if (material) sphere->SetMaterial(material);
						scene->AddObject((Object*)sphere);
					}

				}

			}

		material = new Material(Vec3(0.0, 0.0, 0.0), 0.0, Vec3(1.0, 1.0, 1.0), 0.7, 20, 1, 1.5);
		sphere = new Sphere(Vec3(0.0, 1.0, 0.0), 1.0);
		if (material) sphere->SetMaterial(material);
		scene->AddObject((Object*)sphere);

		material = new Material(Vec3(0.4, 0.2, 0.1), 0.9, Vec3(1.0, 1.0, 1.0), 0.1, 10, 0, 1.0);
		sphere = new Sphere(Vec3(-4.0, 1.0, 0.0), 1.0);
		if (material) sphere->SetMaterial(material);
		scene->AddObject((Object*)sphere);

		material = new Material(Vec3(0.4, 0.2, 0.1), 0.0, Vec3(0.7, 0.6, 0.5), 1.0, 220, 0, 1.0);
		sphere = new Sphere(Vec3(4.0, 1.0, 0.0), 1.0);
		if (material) sphere->SetMaterial(material);
		scene->AddObject((Object*)sphere);

		return scene;
	}
}

