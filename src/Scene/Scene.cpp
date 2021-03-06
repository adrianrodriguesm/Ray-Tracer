#include "pch.h"
#include "Math/maths.h"
#include "Scene.h"

namespace rayTracer
{
	Scene::Scene()
	{}

	Scene::~Scene()
	{
		for ( int i = 0; i < m_Objects.size(); i++ )
			delete m_Objects[i];
		
		m_Objects.clear();

		for (int i = 0; i < m_Lights.size(); i++)
			delete m_Lights[i];

		m_Lights.clear();
		
	}

	int Scene::GetNumObjects()
	{
		return (int)m_Objects.size();
	}


	void Scene::AddObject(Object* o)
	{
		m_Objects.push_back(o);
	}


	Object* Scene::GetObject(unsigned int index)
	{
		if (index >= 0 && index < m_Objects.size())
			return m_Objects[index];
		return nullptr;
	}


	int Scene::GetNumLights()
	{
		return (int)m_Lights.size();
	}


	void Scene::AddLight(Light* l)
	{
		m_Lights.push_back(l);
	}

	Light* Scene::GetLight(unsigned int index)
	{
		if (index >= 0 && index < m_Lights.size())
			return m_Lights[index];
		return nullptr;
	}

	void Scene::LoadSkybox(const char* sky_dir)
	{
		char* filenames[6];
		char buffer[100];
		const char* maps[] = { "/right.jpg", "/left.jpg", "/top.jpg", "/bottom.jpg", "/front.jpg", "/back.jpg" };

		for (int i = 0; i < 6; i++) {
			strcpy_s(buffer, sizeof(buffer), sky_dir);
			strcat_s(buffer, sizeof(buffer), maps[i]);
			filenames[i] = (char*)malloc(sizeof(buffer));
			strcpy_s(filenames[i], sizeof(buffer), buffer);
		}

		ILuint ImageName;

		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

		for (int i = 0; i < 6; i++) {
			ilGenImages(1, &ImageName);
			ilBindImage(ImageName);

			if (ilLoadImage(filenames[i]))  //Image loaded with lower left origin
				printf("Skybox face %d: Image sucessfully loaded.\n", i);
			else
			{
				std::cerr << "ERROR: Could not load skybox image at path '" << filenames[i] << "'.\n";
				exit(0);
			}

			ILint bpp = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);

			ILenum format = IL_RGB;
			printf("bpp=%d\n", bpp);
			if (bpp == 24)
				format = IL_RGB;
			else if (bpp == 32)
				format = IL_RGBA;

			ilConvertImage(format, IL_UNSIGNED_BYTE);

			int size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
			m_SkyboxImage[i].img = (ILubyte*)malloc(size);
			ILubyte* bytes = ilGetData();
			memcpy(m_SkyboxImage[i].img, bytes, size);
			m_SkyboxImage[i].resX = ilGetInteger(IL_IMAGE_WIDTH);
			m_SkyboxImage[i].resY = ilGetInteger(IL_IMAGE_HEIGHT);
			format == IL_RGB ? m_SkyboxImage[i].BPP = 3 : m_SkyboxImage[i].BPP = 4;
			ilDeleteImages(1, &ImageName);
		}
		ilDisable(IL_ORIGIN_SET);
	}

	//Skybox images constant symbolics
	static enum CubeMap { RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK };
	Vec3 Scene::GetSkyboxColor(Ray& r)
	{

		if (r.Direction.SqrMagnitude() == 0)
			return this->GetBackgroundColor();

		float t_intersec;
		Vec3 cubemap_coords; //To index the skybox

		float ma;
		CubeMap img_side;
		float sc, tc, s, t;
		unsigned int xp, yp, width, height, bytesperpixel;

		//skybox indexed by the ray direction
		cubemap_coords = r.Direction;


		if (fabs(cubemap_coords.x) > fabs(cubemap_coords.y)) {
			ma = fabs(cubemap_coords.x);
			cubemap_coords.x >= 0 ? img_side = LEFT : img_side = RIGHT;    //left cubemap at X = +1 and right at X = -1
		}
		else {
			ma = fabs(cubemap_coords.y);
			cubemap_coords.y >= 0 ? img_side = TOP : img_side = BOTTOM; //top cubemap at Y = +1 and bottom at Y = -1
		}

		if (fabs(cubemap_coords.z) > ma) {
			ma = fabs(cubemap_coords.z);
			cubemap_coords.z >= 0 ? img_side = FRONT : img_side = BACK;   //front cubemap at Z = +1 and back at Z = -1
		}

		switch (img_side) {

		case 0:  //right
			sc = -cubemap_coords.z;
			tc = cubemap_coords.y;
			break;

		case 1:  //left
			sc = cubemap_coords.z;
			tc = cubemap_coords.y;
			break;

		case 2:  //top
			sc = -cubemap_coords.x;
			tc = -cubemap_coords.z;
			break;

		case 3: //bottom
			sc = -cubemap_coords.x;
			tc = cubemap_coords.z;
			break;

		case 4:  //front
			sc = -cubemap_coords.x;
			tc = cubemap_coords.y;
			break;

		case 5: //back
			sc = cubemap_coords.x;
			tc = cubemap_coords.y;
			break;
		}

		float invMa = 1 / ma;
		s = (sc * invMa + 1) / 2.f;
		t = (tc * invMa + 1) / 2.f;

		width = m_SkyboxImage[img_side].resX;
		height = m_SkyboxImage[img_side].resY;
		bytesperpixel = m_SkyboxImage[img_side].BPP;

		xp = int((width - 1) * s);
		xp < 0 ? 0 : (xp > (width - 1) ? width - 1 : xp);
		yp = int((height - 1) * t);
		yp < 0 ? 0 : (yp > (height - 1) ? height - 1 : yp);

		float red = u8tofloat(m_SkyboxImage[img_side].img[(yp * width + xp) * bytesperpixel]);
		float green = u8tofloat(m_SkyboxImage[img_side].img[(yp * width + xp) * bytesperpixel + 1]);
		float blue = u8tofloat(m_SkyboxImage[img_side].img[(yp * width + xp) * bytesperpixel + 2]);

		return(Vec3(red, green, blue));
	}

	////////////////////////////////////////////////////////////////////////////////
	// P3F file parsing methods.
	//
	/** /
	void next_token(std::ifstream& file, char* token, const char* name)
	{
		file >> token;
		if (strcmp(token, name))
			std::cerr << "'" << name << "' expected.\n";
	}

	bool Scene::load_p3f(const char* name)
	{
		const	int	lineSize = 1024;
		std::string	cmd;
		char		token[256];
		std::ifstream	file(name, std::ios::in);
		Material* material;

		material = NULL;

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
					this->AddObject((Object*)sphere);
				}

				else if (cmd == "box")    //axis aligned box
				{
					Vec3 minpoint, maxpoint;
					aaBox* box;

					file >> minpoint >> maxpoint;
					box = new aaBox(minpoint, maxpoint);
					if (material) box->SetMaterial(material);
					this->AddObject((Object*)box);
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
						this->AddObject((Object*)triangle);
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
						this->AddObject((Object*)triangle);
					}

				}

				else if (cmd == "pl")  // General Plane
				{
					Vec3 P0, P1, P2;
					Plane* plane;

					file >> P0 >> P1 >> P2;
					plane = new Plane(P0, P1, P2);
					if (material) plane->SetMaterial(material);
					this->AddObject((Object*)plane);
				}

				else if (cmd == "l")  // Need to check light color since by default is white
				{
					Vec3 pos;
					Vec3 color;

					file >> pos >> color;

					this->AddLight(new Light(pos, color));

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
					camera = new Camera(from, at, up, fov, hither, 100.0f * hither, xres, yres, aperture_ratio, focal_ratio);
					this->SetCamera(camera);
				}

				else if (cmd == "bclr")   //Background color
				{
					Vec3 bgcolor;
					file >> bgcolor;
					this->SetBackgroundColor(bgcolor);
				}

				else if (cmd == "env")
				{
					file >> token;

					this->LoadSkybox(token);
					this->SetSkyBoxFlg(true);
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
		return true;
	};
	/**/
}


