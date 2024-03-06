#include "mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Materials> materials)
{
    this->vertices = vertices;
    this->indices = indices;
    this->materials = materials;
    setupMesh();
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Materials> materials, unordered_map<string, vector<glm::vec3>> morphAnims)
{
    this->vertices = vertices;
    this->indices = indices;
    this->materials = materials;
    this->morphAnims = morphAnims;
    setupMesh();
}

void Mesh::Draw([[maybe_unused]] Shader &shader)
{
    // bind appropriate textures
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        // if (materials[i].id != -1)
        // {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, materials[i].id);
        // shader.SetUniform4f("colorOnly", glm::vec4(-1.0));
        // }
        // else
        //     shader.SetUniform4f("colorOnly", glm::vec4(materials[i].color, 1.0));
    }
    // draw mesh
    glBindVertexArray(VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices.size()));
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<unsigned int>(vertices.size()));
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::Draw([[maybe_unused]] Shader &shader, unordered_map<string, float> morphanimkeys)
{
    // bind appropriate textures
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        // if (materials[i].id != -1)
        // {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, materials[i].id);
        // shader.SetUniform4f("colorOnly", glm::vec4(-1.0));
        // }
        // else
        // shader.SetUniform4f("colorOnly", glm::vec4(materials[i].color, 1.0));
    }
    glBindVertexArray(VAO);

    if (!morphAnims.empty())
    {
        vector<glm::vec3> vers = mPositions;
        if (!morphanimkeys.empty())
        {
            for (const auto &morphanimkey : morphanimkeys)
            {
                string keyName = morphanimkey.first;
                float keyValue = morphanimkey.second;
                if (morphAnims.count(keyName))
                {
                    auto &positions = morphAnims[keyName];
                    for (unsigned int i = 0; i < mPositions.size(); ++i)
                    {
                        vers[i] += (positions[i] * keyValue);
                    }
                }
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Position);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vers.size() * sizeof(glm::vec3), vers.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawInstance([[maybe_unused]] Shader &shader)
{
    // bind appropriate textures
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, materials[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, 3000);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{

    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // -------------
    if (morphAnims.size())
    {
        mPositions.resize(vertices.size());
        std::transform(vertices.begin(), vertices.end(), mPositions.begin(),
                       [](const Vertex &vertice)
                       { return vertice.Position; });
        glGenBuffers(1, &VBO_Position);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Position);
        glBufferData(GL_ARRAY_BUFFER, mPositions.size() * sizeof(glm::vec3), mPositions.data(), GL_DYNAMIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A greate thing about struct is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    if (morphAnims.size() == 0)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    }
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}