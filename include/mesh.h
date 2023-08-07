#ifndef __MESH_H__
#define __MESH_H__

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include <vector>
using namespace std;
#define MAX_BONE_INFLUENCE 4
struct Vertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    // bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};
struct Texture_config
{
    unsigned int id;
    string type;
    string path;
};

class Mesh
{
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture_config> textures;
    unsigned int VAO;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture_config> textures);
    void Draw(Shader &shader);
    void DrawInstance(Shader &shader);

private:
    unsigned int VBO, EBO;
    void setupMesh();
};

#endif