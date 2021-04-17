#include "Renderer/SceneRenderer.h"
#include <math.h>
#include <Math/Maths.h>
#include <algorithm>    // std::random_shuffle
#include "Renderer/Grid.h"
#include "Core/Constant.h"
namespace rayTracer
{
	/////////////////////////////////////////////////////////////////////// OpenGL error callbacks
	static bool IsOpenGLError()
	{
		bool isError = false;
		GLenum errCode;
		const GLubyte* errString;
		while ((errCode = glGetError()) != GL_NO_ERROR) 
		{
			isError = true;
			errString = gluErrorString(errCode);
			std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
		}
		return isError;
	}

	static void CheckOpenGLError(std::string error)
	{
		if (IsOpenGLError()) 
		{
			std::cerr << error << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	/////////////////////////////////////////////////////////////////////// Shaders
	static const GLchar* VertexShader =
	{
		"#version 430 core\n"

		"in vec2 in_Position;\n"
		"in vec3 in_Color;\n"
		"uniform mat4 Matrix;\n"
		"out vec4 color;\n"

		"void main(void)\n"
		"{\n"
		"	vec4 position = vec4(in_Position, 0.0, 1.0);\n"
		"	color = vec4(in_Color, 1.0);\n"
		"	gl_Position = Matrix * position;\n"

		"}\n"
	};
	static const GLchar* FragmentShader =
	{
		"#version 430 core\n"

		"in vec4 color;\n"
		"out vec4 out_Color;\n"

		"void main(void)\n"
		"{\n"
		"	out_Color = color;\n"
		"}\n"
	};
	
	struct RendererData
	{
		// Scene Data
		struct SceneData
		{
			std::vector<Object*> Objects;
			Camera* Camera = nullptr;
			Scene* Scene = nullptr;
			uint32_t Width = 512, Height = 512;
			uint32_t MaxDepth = 3;
		} DataScene;
		// Rendering Mode
		RenderMode RenderMode = RenderMode::Default; // This is define by the application
		// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
		float* Colors = nullptr, * Vertices = nullptr;
		int VerticesSize = 0, ColorsSize = 0;
		// Array of Pixels to be stored in a file by using DevIL library
		uint8_t* ImageData = nullptr;
		// Shader Data
		GLuint VertexShaderId = 0, FragmentShaderId = 0, ProgramId = 0;
		GLint UniformId = 0;
		static constexpr uint32_t VERTEX_COORD_ATTRIB = 0;
		static constexpr uint32_t COLOR_ATTRIB = 1;
		// Vertex Data
		GLuint VaoId = 0, VboId[2];
		// Additional Parameters
		bool toneMappingActivated = true;
		bool gammaCorrectionActivated = true;
		// Antialiasing
		AntialiasingMode antialiasingMode = AntialiasingMode::JITTERING;
		std::vector<Vec2> lightSamplingOffsetGrid; // The grid of offsets for the shadow sampling. Used in the Light class
		// Acceleration Structures
		AccelerationStructure currentAccelerationStruct = AccelerationStructure::BVH;
		Grid* Grid;
		BVH* Bvh;
		// Timer
		bool ShowTime = true;
		std::vector<Vec3> HemishepreSamples;
	};
	static RendererData s_Data;


	void SceneRenderer::Init()
	{
		InitData();
		CreateShaders();
		CreateBuffers();
	}

	void SceneRenderer::Shutdown()
	{
		delete s_Data.Grid;
		delete s_Data.Bvh;
		DestroyData();
		DestroyShaders();
		DestroyBuffers();
	}

	void SceneRenderer::BeginSubmit(SceneRendererSpec spec)
	{
		s_Data.DataScene.Camera = spec.Camera;
		s_Data.DataScene.Scene = spec.Scene;
		s_Data.DataScene.Width = spec.Width;
		s_Data.DataScene.Height = spec.Height;
		s_Data.DataScene.MaxDepth = spec.MaxDepth;
		// Pixel buffer to be used in the Save Image function
		InitData();
	}

	void SceneRenderer::SubmitObject(Object* obj)
	{
		s_Data.DataScene.Objects.push_back(obj);
	}

	void SceneRenderer::EndSubmit()
	{
		s_Data.Grid = new Grid(s_Data.DataScene.Objects);
		std::cout << std::endl << "Building grid..." << std::endl;
		s_Data.Grid->BuildGrid();
		std::cout << "Grid built!" << std::endl;

		s_Data.Bvh = new BVH();
		std::cout << std::endl << "Building BVH..." << std::endl;
		s_Data.Bvh->Build(s_Data.DataScene.Objects);
		std::cout << "BVH built!" << std::endl;

		// Bind Vertex Array and Shader
		glBindVertexArray(s_Data.VaoId);
		glUseProgram(s_Data.ProgramId);

	}

	void SceneRenderer::OnResize(int width, int height)
	{
		s_Data.DataScene.Width = width;
		s_Data.DataScene.Height = width;
		DestroyData();
		InitData();

	}

	void SceneRenderer::ToggleGammaCorrection() 
	{
		s_Data.gammaCorrectionActivated = !s_Data.gammaCorrectionActivated;
		std::cout << "Gamma correction: " << (s_Data.gammaCorrectionActivated ? "On" : "Off") << std::endl;
		s_Data.ShowTime = true;
	}

	void SceneRenderer::ToggleToneMapping() 
	{
		s_Data.toneMappingActivated = !s_Data.toneMappingActivated;
		std::cout << "Tone mapping: " << (s_Data.toneMappingActivated ? "On" : "Off") << std::endl;
		s_Data.ShowTime = true;
	}

	void SceneRenderer::ToggleShadows()
	{
		int lightsOn = 0;
		int lightsOff = 0;
		for each(auto& light in s_Data.DataScene.Scene->GetLights())
		{
			bool onOff = !light->shadows;
			light->shadows = onOff;
			if (onOff)
				lightsOn++;
			else
				lightsOff++;
		}
		if(lightsOff > 0)
			std::cout << "Deactivated shadows for " << lightsOff << " lights." << std::endl;
		if(lightsOn > 0)
			std::cout << "Activated shadows for " << lightsOn << " lights." << std::endl;
		std::cout << std::endl;
		s_Data.ShowTime = true;
	}

	void SceneRenderer::ChangeTracingDepth(int change) 
	{
		s_Data.DataScene.MaxDepth = std::max(1, (int)s_Data.DataScene.MaxDepth + change);
		std::cout << "Max Depth: " << s_Data.DataScene.MaxDepth << std::endl;
		s_Data.ShowTime = true;
	}

	void SceneRenderer::SwitchAntialiasingMode(AntialiasingMode newMode)
	{
		s_Data.antialiasingMode = newMode;
		std::string modeName = newMode == AntialiasingMode::NONE ? "None" : newMode == AntialiasingMode::JITTERING ? "Jittering" : "Regular Sampling";
		std::cout << "Antialiasing Mode: " << modeName << std::endl;
		s_Data.ShowTime = true;
	}
	void SceneRenderer::SwitchAccelererationStructure(AccelerationStructure newStruct)
	{
		s_Data.currentAccelerationStruct = newStruct;
		std::string modeName = newStruct == AccelerationStructure::NONE ? "None" : newStruct == AccelerationStructure::GRID ? "Grid" : "BVH";
		std::cout << "Acceleration Structure: " << modeName << std::endl;
		s_Data.ShowTime = true;
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Render
	/////////////////////////////////////////////////////////////////////////////////////////
	Vec3 SceneRenderer::TraceRays(Ray& ray, uint32_t depth, float refractionIndex)
	{
		if (ray.Direction.SqrMagnitude() == 0)
			return Vec3();

		RayCastHit hit;
		switch (s_Data.currentAccelerationStruct)
		{
		case(AccelerationStructure::NONE): hit = GetClosestHit(ray); break;
		case(AccelerationStructure::GRID): hit = s_Data.Grid->Intercepts(ray); break;
		case(AccelerationStructure::BVH): hit = s_Data.Bvh->Intercepts(ray); break;
		default:
			{
				std::cout << "\nERROR: The attempted acceleration structure has not been implemented\n";
				exit(EXIT_FAILURE);
			}
		}
		
		if (!hit)
			return s_Data.DataScene.Scene->GetSkyBoxFlg() ? s_Data.DataScene.Scene->GetSkyboxColor(ray) : s_Data.DataScene.Scene->GetBackgroundColor(); //GetBackgroundColor();
			

		Material* material = hit.Object->GetMaterial();
		Vec3 color;
		Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
		if (hit.Object->IsInsideObject(hit.InterceptionPoint, ray))
			normal *= -1; // Inverse normal if inside object (For transparent objects)
		Vec3 viewDir = ray.Direction; // Unit vector
		Vec3 invViewDir = -1 * viewDir; // Unit vector
		Vec3 emissionPoint = hit.InterceptionPoint + normal * DISPLACEMENT_BIAS;


		auto& lights = s_Data.DataScene.Scene->GetLights();
		for (auto& light : lights)
		{
			Vec3 lightDir =  Vec3(light->position - hit.InterceptionPoint).Normalized();

			if (DotProduct(lightDir, normal) <= 0)
				continue;

			float intensity = light->GetIntensityAtPoint(emissionPoint, s_Data.DataScene.Objects, s_Data.antialiasingMode);
			
			if (intensity <= 0)
				continue;
			
			color += BlinnPhong(material, light, lightDir, viewDir, normal, intensity);
		}


		// If we reach the max deth end the recursive call
		if (depth >= s_Data.DataScene.MaxDepth)
			return color;
		
		// Refracted
		if (material->GetTransmittance() > 0)
		{
			float incidentAngle = acosf(DotProduct(normal,invViewDir));
			float sinIncidentAngle = sinf(incidentAngle);

			if (sinIncidentAngle > 0) {
				float otherRefractIndex = material->GetRefrIndex();

				// Calculate Reflection power (Shlicks Approximation)
				float R0 = powf(((refractionIndex - otherRefractIndex) / (refractionIndex + otherRefractIndex)), 2.0f);
				float R = R0 + (1 - R0) * (powf(1 - cosf(incidentAngle), 5));

				Ray reflectedRay = RayCastHit::CalculateReflectedRay(hit, viewDir);
				Ray refractedRay = RayCastHit::CalculateRefractedRay(normal, hit, viewDir, refractionIndex, otherRefractIndex);

				Vec3 reflectedColor = TraceRays(reflectedRay, depth + 1, refractionIndex);
				Vec3 refractedColor = TraceRays(refractedRay, depth + 1, otherRefractIndex);

				color += R * reflectedColor;
				color += (1 - R) * refractedColor;
			}
		}
		// Reflected Ray
		else if (material->GetReflection() > 0)
		{
			Ray reflected = RayCastHit::CalculateReflectedRay(hit, viewDir);
			Vec3 reflectedColor = TraceRays(reflected, depth + 1, refractionIndex);
			color += material->GetReflection() * reflectedColor;
		}
		return color;
	}
	RayCastHit SceneRenderer::GetClosestHit(Ray& ray)
	{
		float tmin = FLOAT_MAX;
		RayCastHit hit, temphit;
		for (auto& obj : s_Data.DataScene.Objects)
		{
			temphit = obj->Intercepts(ray);
			if (temphit && temphit.Tdist < tmin)
			{
				tmin = temphit.Tdist;
				hit = temphit;
			}
		}
		return hit;
	}
	/*
	RayCastHit SceneRenderer::IsPointInShadow(RayCastHit& hit, Vec3& lightDir, float lightDistance)
	{
		Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
		Ray shadowFeeler(hit.InterceptionPoint + normal * DISPLACEMENT_BIAS, lightDir);
		for (auto obj : s_Data.DataScene.Objects)
		{
			if (obj->GetMaterial()->GetTransmittance() > 0)
				continue; // Transparent objects do not block light (Should refract light, but....)

			RayCastHit shadowHit = obj->Intercepts(shadowFeeler);
			if (shadowHit && shadowHit.Tdist < lightDistance)
				return true;
		}
		return false;
	}*/

	Vec3 SceneRenderer::BlinnPhong(Material* mat, Light* light, Vec3& lightDir, Vec3& viewDir, Vec3& normal, float intensity)
	{
		float diffuseIntensity = std::fmax(DotProduct(lightDir, normal), 0.0f);
		if (diffuseIntensity > 0)
		{
			// Diffuse
			float KdLamb = mat->GetDiffuse() * diffuseIntensity;
			Vec3 diffuseColor = light->color * KdLamb * mat->GetDiffColor();

			// Specular
			Vec3 halfwayVector = viewDir + lightDir;
			Vec3 reflected = halfwayVector.Normalized();
			float specAngle = std::fmax(DotProduct(reflected, normal), 0.0f);
			float specular = pow(specAngle, mat->GetShine());
			float ksSpec = mat->GetSpecular() * specular;
			Vec3 specularColor = light->color * ksSpec * mat->GetSpecColor();

			// Total
			Vec3 color = diffuseColor + specularColor;
			color *= intensity;

			return color;
		}
		return Vec3(0.f);
	}
	Vec3 SceneRenderer::SampleHemisphere()
	{
		static uint32_t currentSampleIndex = 0;
		if (currentSampleIndex % 16 == 0)
			currentSampleIndex = (rand_int() % 16);
		
		int index =  currentSampleIndex++ % s_Data.HemishepreSamples.size();
		return s_Data.HemishepreSamples[index];
	}
	void SceneRenderer::MapSamplesToHemisphere(std::vector<Vec2>& samples, const float exp)
	{
		int size = samples.size();
		s_Data.HemishepreSamples.clear();
		s_Data.HemishepreSamples.reserve(samples.size());
		for (int j = 0; j < size; j++) 
		{
			// Sample to hemisphere using a cosine power distribution
			// https://blog.thomaspoulet.fr/uniform-sampling-on-unit-hemisphere/
			float phi = 2.0f * M_PI * samples[j].x;
			float cosPhi = cosf(phi);
			float sinPhi = sinf(phi);
			float cosTheta = powf((1.0f * samples[j].y), 1.0f / (exp + 1.0f));
			float sintheta = sqrtf(1.0f - cosTheta * cosTheta);
			s_Data.HemishepreSamples.push_back({ sintheta * cosPhi, sintheta * cosPhi, cosTheta });
		}
	}


	void SceneRenderer::Render()
	{
		int startTime = glutGet(GLUT_ELAPSED_TIME);
		auto& width = s_Data.DataScene.Width;
		auto& height = s_Data.DataScene.Height;

		int index_pos = 0;
		int index_col = 0;
		uint32_t counter = 0;

		constexpr uint32_t nbSamples = 4;

		
		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				Vec3 color;
				Vec2 pixel = Vec2(x, y);
				std::vector<Vec2> samplingOffsets;
				// Get Sampling points
				switch (s_Data.antialiasingMode)
				{
					case(AntialiasingMode::NONE):				samplingOffsets = SingularSampling::GetSamplingOffsets(nbSamples); break;
					case(AntialiasingMode::JITTERING):			samplingOffsets = JitteringSampling::GetSamplingOffsets(nbSamples); break;
					case(AntialiasingMode::REGULAR_SAMPLING):	samplingOffsets = RegularSampling::GetSamplingOffsets(nbSamples); break;
					default: 
					{
						std::cout << "\nERROR: The attempted antialasing mode has not been implemented\n";
						exit(EXIT_FAILURE); 
					}
				}
				// Set the lightoffset array to a scrambled version of the sampling offsets
				s_Data.lightSamplingOffsetGrid = samplingOffsets;
				std::shuffle(std::begin(s_Data.lightSamplingOffsetGrid), std::end(s_Data.lightSamplingOffsetGrid), std::default_random_engine{});
				MapSamplesToHemisphere(samplingOffsets);
				// Calculate Color
				for each (Vec2 sampleOffset in samplingOffsets)
				{
					if(s_Data.antialiasingMode != AntialiasingMode::NONE)
					{
						Ray ray = s_Data.DataScene.Camera->PrimaryLensRay(pixel + sampleOffset);
						color += TraceRays(ray, 1, 1.0);
					}
					else
					{
						Ray ray = s_Data.DataScene.Camera->PrimaryCenterRay(pixel + sampleOffset);
						color += TraceRays(ray, 1, 1.0);
					}
				}
				color = color / samplingOffsets.size();

				// Reinhard tonemapping
				static constexpr float exposure = 0.3f;
				if(s_Data.toneMappingActivated)
					color = utils::ApplyToneMapping(color, exposure);

				// Gamma correction
				if (s_Data.gammaCorrectionActivated)
					color = utils::ConvertColorFromLinearToGammaSpace(color);

				// Update Image Data
				s_Data.ImageData[counter++] = (uint8_t)color.r;
				s_Data.ImageData[counter++] = (uint8_t)color.g;
				s_Data.ImageData[counter++] = (uint8_t)color.b;

				if (s_Data.RenderMode == RenderMode::DrawMode)
				{
					s_Data.Vertices[index_pos++] = (float)x;
					s_Data.Vertices[index_pos++] = (float)y;
					s_Data.Colors[index_col++] = (float)color.r;
					s_Data.Colors[index_col++] = (float)color.g;
					s_Data.Colors[index_col++] = (float)color.b;
					if (x == width - 1 && fmodf((float)y, (height / 10)) == 0)
						DrawPoints();
				}
					
			}

		}
		if (s_Data.ShowTime)
		{
			std::cout << "Elapsed Time: " << glutGet(GLUT_ELAPSED_TIME) - startTime << "ms" << std::endl;
			s_Data.ShowTime = false;
		}

		// Render Data
		//Flush();
	}
	std::vector<Vec2> SceneRenderer::GetLightSamplingArray()
	{
		return s_Data.lightSamplingOffsetGrid;
	}
	bool SceneRenderer::GetGammaCorrection()
	{
		return s_Data.gammaCorrectionActivated;
	}
	bool SceneRenderer::GetToneMapping()
	{
		return s_Data.toneMappingActivated;
	}
	int SceneRenderer::GetTracingDepth()
	{
		return s_Data.DataScene.MaxDepth;
	}

	float SceneRenderer::GetAperture()
	{
		return s_Data.DataScene.Camera->GetAperture();
	}

	AntialiasingMode SceneRenderer::GetAntialiasingMode()
	{
		return s_Data.antialiasingMode;
	}
	AccelerationStructure SceneRenderer::GetAccelerationStruct()
	{
		return s_Data.currentAccelerationStruct;
	}
	Grid& SceneRenderer::GetGrid()
	{
		return *(s_Data.Grid);
	}
	BVH& SceneRenderer::GetBVH()
	{
		return *(s_Data.Bvh);
	}
	void SceneRenderer::Flush()
	{
		switch (s_Data.RenderMode)
		{
			case(RenderMode::DrawMode):			DrawPoints();	 return;
			case(RenderMode::GenerateImage):	GenerateImage(); return;
		}
	}

	void SceneRenderer::DrawPoints()
	{
		//glClear(GL_COLOR_BUFFER_BIT);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VboId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, s_Data.VerticesSize, s_Data.Vertices);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VboId[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, s_Data.ColorsSize, s_Data.Colors);

		glUniformMatrix4fv(s_Data.UniformId, 1, GL_FALSE, s_Data.DataScene.Camera->GetProjectionMatrix().Data());
		glDrawArrays(GL_POINTS, 0, s_Data.DataScene.Width * s_Data.DataScene.Height);
		//glFinish();
		/** /
		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		/**/
		//CheckOpenGLError("ERROR: Could not draw scene.");

		glutSwapBuffers();
	}

	void SceneRenderer::GenerateImage()
	{
		printf("Drawing finished!\n");
		auto msg = SaveImage("RT_Output.png");
		ASSERT(msg == IL_NO_ERROR, "Image file created\n");
		
	}

	uint32_t SceneRenderer::SaveImage(const std::string& filename)
	{
		ILuint ImageId;
		ilEnable(IL_FILE_OVERWRITE);
		ilGenImages(1, &ImageId);
		ilBindImage(ImageId);

		ilTexImage(s_Data.DataScene.Width, s_Data.DataScene.Height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, s_Data.ImageData /*Texture*/);
		ilSaveImage(filename.c_str());

		ilDisable(IL_FILE_OVERWRITE);
		ilDeleteImages(1, &ImageId);
		if (ilGetError() != IL_NO_ERROR) return ilGetError();

		return IL_NO_ERROR;
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Shaders
	////////////////////////////////////////////////////////////////////////////////////////
	void SceneRenderer::CreateShaders()
	{
		s_Data.VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(s_Data.VertexShaderId, 1, &VertexShader, 0);
		glCompileShader(s_Data.VertexShaderId);

		s_Data.FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(s_Data.FragmentShaderId, 1, &FragmentShader, 0);
		glCompileShader(s_Data.FragmentShaderId);

		s_Data.ProgramId = glCreateProgram();
		glAttachShader(s_Data.ProgramId, s_Data.VertexShaderId);
		glAttachShader(s_Data.ProgramId, s_Data.FragmentShaderId);

		glBindAttribLocation(s_Data.ProgramId, s_Data.VERTEX_COORD_ATTRIB, "in_Position");
		glBindAttribLocation(s_Data.ProgramId, s_Data.COLOR_ATTRIB, "in_Color");

		glLinkProgram(s_Data.ProgramId);
		s_Data.UniformId = glGetUniformLocation(s_Data.ProgramId, "Matrix");

		CheckOpenGLError("ERROR: Could not create shaders.");
	}
	void SceneRenderer::DestroyShaders()
	{
		glUseProgram(0);
		glDetachShader(s_Data.ProgramId, s_Data.VertexShaderId);
		glDetachShader(s_Data.ProgramId, s_Data.FragmentShaderId);

		glDeleteShader(s_Data.FragmentShaderId);
		glDeleteShader(s_Data.VertexShaderId);
		glDeleteProgram(s_Data.ProgramId);

		CheckOpenGLError("ERROR: Could not destroy shaders.");
	}
	void SceneRenderer::InitData()
	{
		// Image
		s_Data.ImageData = (uint8_t*)::operator new (3 * s_Data.DataScene.Width * s_Data.DataScene.Height * sizeof(uint8_t));
		// Vertices
		s_Data.VerticesSize = 2 * s_Data.DataScene.Width * s_Data.DataScene.Height * sizeof(float);
		s_Data.Vertices = (float*)::operator new (s_Data.VerticesSize);
		// Colors
		s_Data.ColorsSize = 3 * s_Data.DataScene.Width * s_Data.DataScene.Height * sizeof(float);
		s_Data.Colors = (float*)::operator new (s_Data.ColorsSize);
	}
	void SceneRenderer::DestroyData()
	{
		::operator delete(s_Data.ImageData, 3 * s_Data.DataScene.Width * s_Data.DataScene.Height * sizeof(uint8_t));
		::operator delete(s_Data.Vertices, s_Data.VerticesSize);
		::operator delete(s_Data.Colors, s_Data.ColorsSize);
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Buffers
	////////////////////////////////////////////////////////////////////////////////////////
	void SceneRenderer::CreateBuffers()
	{
		glGenVertexArrays(1, &s_Data.VaoId);
		glBindVertexArray(s_Data.VaoId);
		glGenBuffers(2, s_Data.VboId);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VboId[0]);

		glBufferData(GL_ARRAY_BUFFER, s_Data.VerticesSize, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(s_Data.VERTEX_COORD_ATTRIB);
		glVertexAttribPointer(s_Data.VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VboId[1]);
		glBufferData(GL_ARRAY_BUFFER, s_Data.ColorsSize, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(s_Data.COLOR_ATTRIB);
		glVertexAttribPointer(s_Data.COLOR_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

		// unbind the VAO
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB); 
		//	glDisableVertexAttribArray(COLOR_ATTRIB);
		CheckOpenGLError("ERROR: Could not create VAOs and VBOs.");
	}
	
	void SceneRenderer::DestroyBuffers()
	{
		glDisableVertexAttribArray(s_Data.VERTEX_COORD_ATTRIB);
		glDisableVertexAttribArray(s_Data.COLOR_ATTRIB);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glDeleteBuffers(1, s_Data.VboId);
		glDeleteVertexArrays(1, &s_Data.VaoId);
		CheckOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
	}
}