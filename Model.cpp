#include "Model.hpp"

Model::Model(string const& path, bool gamma) : gammaCorrection(gamma)
{
	loadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
		meshes[i].Draw(shader);
}