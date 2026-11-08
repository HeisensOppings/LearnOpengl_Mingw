#include "../include/mesh.h"

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

Mesh::~Mesh()
{
    glf()->glDeleteVertexArrays(1, &VAO);
    glf()->glDeleteBuffers(1, &VBO);
    glf()->glDeleteBuffers(1, &EBO);
    glf()->glDeleteBuffers(1, &VBO_Position);
}

void Mesh::Draw([[maybe_unused]] Shader &shader)
{
    // bind appropriate textures
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        // if (materials[i].id != -1)
        // {
        glf()->glActiveTexture(GL_TEXTURE0 + i);
        glf()->glBindTexture(GL_TEXTURE_2D, materials[i].id);
        // shader.SetUniform4f("colorOnly", glm::vec4(-1.0));
        // }
        // else
        //     shader.SetUniform4f("colorOnly", glm::vec4(materials[i].color, 1.0));
    }
    // draw mesh
    glf()->glBindVertexArray(VAO);

    // glf()->glBindBuffer(GL_ARRAY_BUFFER, 0);

    glf()->glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    // glf()->glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices.size()));
    // glf()->glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<unsigned int>(vertices.size()));
    glf()->glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glf()->glActiveTexture(GL_TEXTURE0);
}

void Mesh::Draw([[maybe_unused]] Shader &shader, unordered_map<string, float> morphanimkeys)
{
    // bind appropriate textures
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        // if (materials[i].id != -1)
        // {
        glf()->glActiveTexture(GL_TEXTURE0 + i);
        glf()->glBindTexture(GL_TEXTURE_2D, materials[i].id);
        // shader.SetUniform4f("colorOnly", glm::vec4(-1.0));
        // }
        // else
        // shader.SetUniform4f("colorOnly", glm::vec4(materials[i].color, 1.0));
    }
    glf()->glBindVertexArray(VAO);

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
        glf()->glBindBuffer(GL_ARRAY_BUFFER, VBO_Position);
        glf()->glBufferSubData(GL_ARRAY_BUFFER, 0, vers.size() * sizeof(glm::vec3), vers.data());
        glf()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glf()->glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glf()->glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glf()->glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawInstance([[maybe_unused]] Shader &shader)
{
    // bind appropriate textures
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        glf()->glActiveTexture(GL_TEXTURE0 + i);
        glf()->glBindTexture(GL_TEXTURE_2D, materials[i].id);
    }

    // draw mesh
    glf()->glBindVertexArray(VAO);
    glf()->glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, 3000);
    glf()->glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glf()->glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glf()->glGenVertexArrays(1, &VAO);
    glf()->glGenBuffers(1, &VBO);
    glf()->glGenBuffers(1, &EBO);

    glf()->glBindVertexArray(VAO);

    // -------------
    if (morphAnims.size())
    {
        mPositions.resize(vertices.size());
        std::transform(vertices.begin(), vertices.end(), mPositions.begin(),
                       [](const Vertex &vertice)
                       { return vertice.Position; });
        glf()->glGenBuffers(1, &VBO_Position);
        glf()->glBindBuffer(GL_ARRAY_BUFFER, VBO_Position);
        glf()->glBufferData(GL_ARRAY_BUFFER, mPositions.size() * sizeof(glm::vec3), mPositions.data(), GL_DYNAMIC_DRAW);
    }

    glf()->glEnableVertexAttribArray(0);
    glf()->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

    // load data into vertex buffers
    glf()->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A greate thing about struct is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which again translates to 3/2 floats which translates to a byte array.
    glf()->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glf()->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glf()->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    if (morphAnims.size() == 0)
    {
        glf()->glEnableVertexAttribArray(0);
        glf()->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    }
    // vertex normals
    glf()->glEnableVertexAttribArray(1);
    glf()->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    // vertex texture coords
    glf()->glEnableVertexAttribArray(2);
    glf()->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    // vertex tangent
    glf()->glEnableVertexAttribArray(3);
    glf()->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
    // vertex bitangent
    glf()->glEnableVertexAttribArray(4);
    glf()->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
    // ids
    glf()->glEnableVertexAttribArray(5);
    glf()->glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));
    // weights
    glf()->glEnableVertexAttribArray(6);
    glf()->glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
    glf()->glBindVertexArray(0);
}