#ifndef __MODEL_H__
#define __MODEL_H__

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "AssimpGLMHelpers.h"

#include "mesh.h"
#include "shader.h"
#include "bone.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <thread>
#include <mutex>

struct TetxtureType
{
    aiTextureType type;
    string prefix;
};

class Model
{
public:
    // model data
    vector<Materials> m_materials; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> m_meshes;
    // vector<Animation> animations;
    string directory;
    bool gammaCorrection;
    // morph data
    unordered_map<string, vector<glm::vec3>> morphAnims;
    unordered_map<unsigned int, string> shapeKeysNameID;
    unordered_map<string, float> morphAnimKeys;

    // constructor, expects a filepath to a 3D model.
    Model(string const &path, bool gamma = false);
    Model(const aiScene *scene, const string path);
    // draws the model, and thus all its meshes
    void Draw(Shader &shader);
    void DrawInstance(Shader &shader);
    auto &GetBoneInfoMap() { return m_BoneInfoMap; }
    int &GetBoneCount() { return m_BoneCounter; }

    void SetMorphAnimKeys(unordered_map<string, float> morphanimkeys) { morphAnimKeys = morphanimkeys; }

private:
    // for animation
    std::unordered_map<string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    void SetVertexBoneDataToDefault(Vertex &vertex)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            vertex.m_BoneIDs[i] = -1;
            vertex.m_Weights[i] = .0f;
        }
    }
    void SetVertexBoneData(Vertex &vertex, int boneID, float weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.m_BoneIDs[i] < 0)
            {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }
    void ExtractBoneWeightForVertices(vector<Vertex> &vertices, aiMesh *mesh);

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes int the meshse vector.
    void loadModel(string const &path);
    // processes a node in a recursive. Processes each individual mesh located at the node and repeats this process on its children nodes
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    void loadMaterialTextures(vector<Materials> &materials, aiMaterial *mat, aiTextureType type, string typeName);
};

#endif