#pragma once
#include<vector>

class ModelComponent
{
private:
	struct Material
	{
	private:
		unsigned int handle;	// uniform block handle
		glm::vec3 ka;		// ambient colour
		glm::vec3 kd;		// diffuse colour
		glm::vec3 ks;		// specular colour
		GLuint map_Kd;		// diffuse map
		float ns;			// specular highlights
		float ni;			// optical density
		float d;			// dissolve -> opacity
		GLuint illum;		// illumination model -> going to ignore -> its really kinda stupid

		// material definition from other sources
		// these could all be vec4s too
		GLuint Albedo;
		GLuint Normal;
		GLuint Metallic;
		GLuint Roughness;
		GLuint AO;


	public:
		Material(const char* filepath);
		void Bind();

	};

	struct Mesh
	{
	private:
		Material* mat;
		unsigned int VAO, VBO, IBO, length;
		int primitive;
	public:
		Mesh(unsigned int VAO, unsigned int VBO, unsigned int IBO, unsigned int length, int primitive);
		~Mesh();
		void Draw();
	};

	

private:
	std::vector<Mesh*> m_meshes;

public:
	ModelComponent();
	ModelComponent(const char* filename);
	~ModelComponent();

	void Draw();
};

