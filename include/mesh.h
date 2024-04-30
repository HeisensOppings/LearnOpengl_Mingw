#ifndef __MESH_H__
#define __MESH_H__

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include <vector>
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

struct Materials
{
    int id;
    string type;
    string path;
    // if mesh has color only, id = -1
    glm::vec3 color;
};

class Mesh
{
public:
    vector<glm::vec3> mPositions;
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Materials> materials;
    unordered_map<string, vector<glm::vec3>> morphAnims;
    unsigned int VAO;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Materials> materials);
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Materials> materials, unordered_map<string, vector<glm::vec3>> morphAnims);
    ~Mesh();
    void Draw(Shader &shader);
    void Draw(Shader &shader, unordered_map<string, float> morphanimkeys);
    void DrawInstance(Shader &shader);

private:
    unsigned int VBO, EBO;
    unsigned int VBO_Position;
    void setupMesh();
};

#endif