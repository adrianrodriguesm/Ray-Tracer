#include "Renderer/SceneRenderer.h"
#include "Core/Application.h"
#include "Core/Utility.h"
#include <GL/glew.h>
#include <Core/Base.h>
#include <GL/freeglut.h>
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
		float* Colors = nullptr, *Vertices = nullptr;
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
	};
	RendererData s_Data;

	void SceneRenderer::Init()
	{
		InitData();
		CreateShaders();
		CreateBuffers();
	}

	void SceneRenderer::Shutdown()
	{
		DestroyData();
		DestroyShaders();
		DestroyBuffers();
	}

	void SceneRenderer::SumitRenderSpec(SceneRendererSpec spec)
	{
		s_Data.DataScene.Camera = spec.Camera;
		s_Data.DataScene.Scene = spec.Scene;
		s_Data.DataScene.Width = spec.Width;
		s_Data.DataScene.Height = spec.Height;
		s_Data.DataScene.MaxDepth = spec.MaxDepth;
		// Pixel buffer to be used in the Save Image function
		InitData();
	}

	void SceneRenderer::SumitObject(Object* obj)
	{
		s_Data.DataScene.Objects.push_back(obj);
	}

	void SceneRenderer::OnResize(int width, int height)
	{
		s_Data.DataScene.Width = width;
		s_Data.DataScene.Height = width;
		DestroyData();
		InitData();

	}
	/////////////////////////////////////////////////////////////////////////////////////////
	/// Render
	/////////////////////////////////////////////////////////////////////////////////////////
	Vec3 SceneRenderer::TraceRays(Ray& ray, uint32_t depth, float refractionIndex)
	{
		float tmin = DBL_MAX;
		RayCastHit hit = GetClosestHit(ray, tmin);
		if (!hit)
			return s_Data.DataScene.Scene->GetBackgroundColor();

		Material* material = hit.Object->GetMaterial();
		Vec3 color;
		Vec3 normal = hit.Object->GetNormal(hit.InterceptionPoint);
		Vec3 viewDir = ray.Direction; // Unit vector
		auto& lights = s_Data.DataScene.Scene->GetLights();
		for (auto& light : lights)
		{
			// Beucause 
			Vec3 lightDir = Vec3(light->position - hit.InterceptionPoint).Normalized();
			if (IsPointInShadow(hit, lightDir))
				continue; // Zero light contribution for this point
			// Calculate ligth
			color += BlinnPhong(material, light, lightDir, viewDir, normal);
		}
		// If we reach the max deth end the recursive call
		if (depth >= s_Data.DataScene.MaxDepth)
			return color;
		
		// Reflected Ray
		if (material->GetShine() >= 0)
		{
			Ray reflected = RayCastHit::CalculateReflectedRay(hit, viewDir);
			Vec3 reflectedColor = TraceRays(reflected, depth + 1, refractionIndex);
			color += material->GetSpecular() * reflectedColor;
		}
		// Refracted
		if (material->GetTransmittance() != 0)
		{
			Ray refracted = RayCastHit::CalculateRefractedRay(hit, ray, refractionIndex, material->GetRefrIndex());
			if (refracted.Direction != Vec3(0))
			{
				Vec3 refractedColor = TraceRays(refracted, depth + 1, refractionIndex);
				color += material->GetTransmittance() * refractedColor;
			}
		}
		return color;
	}
	RayCastHit SceneRenderer::GetClosestHit(Ray& ray, float tmin)
	{
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
	RayCastHit SceneRenderer::IsPointInShadow(RayCastHit& hit, Vec3& lightDir)
	{
		Ray shadowFeeler(hit.InterceptionPoint, lightDir);
		for (auto obj : s_Data.DataScene.Objects)
		{
			RayCastHit shadowHit = obj->Intercepts(shadowFeeler);
			if (shadowHit)
				return true;
		}
		return false;
	}
	Vec3 SceneRenderer::BlinnPhong(Material* mat, Light* light, Vec3& lightDir, Vec3& viewDir, Vec3& normal)
	{
		Vec3 objectColor = mat->GetDiffColor(); // Check this
		float diffuseIntensity = std::fmax(DotProduct(lightDir, normal), 0.0f);
		if (diffuseIntensity > 0)
		{
			// Diffuse
			float KdLamb = mat->GetDiffuse() * diffuseIntensity;
			Vec3 diffuseColor = light->color * KdLamb;
			/** /
			diffuseColor.r = color.r * light->color.r * KdLamb;
			diffuseColor.g = color.g * light->color.g * KdLamb;
			diffuseColor.b = color.b * light->color.b * KdLamb;
			/**/
			// Specular
			Vec3 halfwayVector = viewDir + lightDir;
			Vec3 reflected = halfwayVector.Normalized();
			float specAngle = std::fmax(DotProduct(reflected, -lightDir), 0.0f);
			float specular = pow(specAngle, mat->GetShine());
			float ksSpec = mat->GetSpecular() * specular;
			Vec3 specularColor = light->color * ksSpec;
			/** /
			specularColor.r = color.r * light->color.r * ksSpec;
			specularColor.g = color.g * light->color.g * ksSpec;
			specularColor.b = color.b * light->color.b * ksSpec;
			/**/
			Vec3 color = diffuseColor + specularColor;
			color.r *= objectColor.r;
			color.g *= objectColor.g;
			color.b *= objectColor.b;
			return color;
		}
		return Vec3(0.f);
	}
	
	void SceneRenderer::Render()
	{
		auto& width = s_Data.DataScene.Width;
		auto& height = s_Data.DataScene.Height;

		int index_pos = 0;
		int index_col = 0;
		unsigned int counter = 0;
	
		glClear(GL_COLOR_BUFFER_BIT);
		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				Vec3 color;

				Vec3 pixel;  //viewport coordinates
				pixel.x = x + 0.5f;
				pixel.y = y + 0.5f;

				// Calculos of the color
				Ray ray = s_Data.DataScene.Camera->PrimaryRay(pixel);
				color = TraceRays(ray, 1, 1.0);
				// Reinhard tonemapping
				static constexpr float exposure = 0.3f;
				color = utils::ApplyToneMapping(color, exposure);
				// Gamma correction
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
				}
			}

		}
		// Render Data
		Flush();
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
		glBindVertexArray(s_Data.VaoId);
		glUseProgram(s_Data.ProgramId);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VboId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, s_Data.VerticesSize, s_Data.Vertices);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VboId[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, s_Data.ColorsSize, s_Data.Colors);

		glUniformMatrix4fv(s_Data.UniformId, 1, GL_FALSE, s_Data.DataScene.Camera->GetProjectionMatrix().Data());
		glDrawArrays(GL_POINTS, 0, s_Data.DataScene.Width * s_Data.DataScene.Height);
		glFinish();

		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CheckOpenGLError("ERROR: Could not draw scene.");

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

		/* Só se faz a alocação dos arrays glBufferData (NULL), e o envio dos pontos para a placa gráfica
		é feito na drawPoints com GlBufferSubData em tempo de execução pois os arrays são GL_DYNAMIC_DRAW */
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