#include "Model.h"
#include <glew.h>	// GL extension wrangler
#include <glfw3.h>	// GL framework


Model::Model(std::vector<std::shared_ptr<Mesh>> p_meshes, std::vector<std::shared_ptr<Material>> p_materials)
	: meshes(std::min(p_meshes.size(), p_materials.size()))
{
	for (int i = 0; i < meshes.size(); i++) 
	{
		glGenVertexArrays(1, &(meshes[i].VAO));

		glBindVertexArray(meshes[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, p_meshes[i]->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p_meshes[i]->IBO);

		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		p_meshes[i]->vertexSetup();

		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &(meshes[i].Size));
		meshes[i].Size /= 4; 

		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
}

void Model::draw()
{
	for (RenderInfo renderInfo : meshes) 
	{
		glBindVertexArray(renderInfo.VAO);
		glDrawElements(GL_TRIANGLES, renderInfo.Size, GL_UNSIGNED_INT, NULL);
	}

}
