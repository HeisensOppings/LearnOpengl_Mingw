#ifndef __MODEL_H__
#define __MODEL_H__

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
class Model
{
public:
    // model data
    vector<Texture_config> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;
    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false);
    // draws the model, and thus all its meshes
    void Draw(Shader &shader);
    void DrawInstance(Shader &shader);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes int the meshse vector.
    void loadModel(string const &path);
    // processes a node in a recursive. Processes each individual mesh located at the node and repeats this process on its children nodes
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture_config> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif