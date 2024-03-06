#include "model.h"

// Most of the program startup time is spent on stbi_load loading textures.
// Using threads can speed up startup time a bit.
#define TEXTURE_THREAD 1

#if TEXTURE_THREAD
vector<thread> texture_threads;
std::mutex texture_mtx;
struct TextureData
{
    int width, height, nrComponents;
    unsigned int textureID;
    unsigned char *data;
};
vector<TextureData> textureData;

void TextureFromData(vector<TextureData> &textureData)
{
    static double time = .0f;
    for (auto &texture : textureData)
    {
        double startTime = glfwGetTime();
        GLenum internalformat = 0;
        GLenum dataformat = 0;
        if (texture.nrComponents == 1)
        {
            internalformat = dataformat = GL_RED;
        }
        else if (texture.nrComponents == 3)
        {
            internalformat = GL_RGB;
            dataformat = GL_RGB;
        }
        else if (texture.nrComponents == 4)
        {
            internalformat = GL_RGBA;
            dataformat = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D, texture.textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, texture.width, texture.height, 0, dataformat, GL_UNSIGNED_BYTE, texture.data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(texture.data);
        time += glfwGetTime() - startTime;
        // cout << time << endl;
    }
    textureData.clear();
}

void TextureFromFileThread(unsigned int texture_id, string filename)
{
    stbi_set_flip_vertically_on_load(false);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        std::lock_guard<std::mutex> lock(texture_mtx);
        textureData.push_back({width, height, nrComponents, texture_id, data});
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }
}

#else

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
unsigned int TextureFromFile(const char *path, const string &directory, [[maybe_unused]] bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    stbi_set_flip_vertically_on_load(false);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalformat = 0;
        GLenum dataformat = 0;
        if (nrComponents == 1)
        {
            internalformat = dataformat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalformat = gamma ? GL_SRGB : GL_RGB;
            dataformat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalformat = gamma ? GL_SRGB : GL_RGBA;
            dataformat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, dataformat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif

vector<TetxtureType> textureTypes{
    {aiTextureType_DIFFUSE, "d"},
    {aiTextureType_HEIGHT, "n"},
    {aiTextureType_NORMALS, "n"},
    {aiTextureType_METALNESS, "m"},
    {aiTextureType_SPECULAR, "m"},
    {aiTextureType_DIFFUSE_ROUGHNESS, "r"},
    {aiTextureType_SHININESS, "r"},
    {aiTextureType_LIGHTMAP, "a"},
    {aiTextureType_AMBIENT, "a"},
    {aiTextureType_EMISSIVE, "e"}};

Model::Model(string const &path, [[maybe_unused]] bool gamma)
{
#if TEXTURE_THREAD
    texture_threads.clear();
#endif

    loadModel(path);

#if TEXTURE_THREAD
    for (auto &thread : texture_threads)
        thread.join();
    TextureFromData(textureData);
#endif
}

Model::Model(const aiScene *scene, const string path)
{
#if TEXTURE_THREAD
    texture_threads.clear();
#endif

    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));
    // process assimp's root node recursive
    processNode(scene->mRootNode, scene);

#if TEXTURE_THREAD
    for (auto &thread : texture_threads)
        thread.join();
    TextureFromData(textureData);
#endif
}

void Model::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < m_meshes.size(); ++i)
        m_meshes[i].Draw(shader, morphAnimKeys);
}

void Model::DrawInstance(Shader &shader)
{
    for (unsigned int i = 0; i < m_meshes.size(); ++i)
        m_meshes[i].DrawInstance(shader);
}

void Model::loadModel(string const &path)
{
    // read file via assimp
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP" << importer.GetErrorString() << endl;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));
    // process assimp's root node recursive
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        // COUT(node->mName.data);
        // COUTL(node->mNumMeshes);
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        static double process = .0f;
        auto startprocess = glfwGetTime();
        m_meshes.push_back(processMesh(mesh, scene));
        process += glfwGetTime() - startprocess;
        // COUTL("processMesh" << process);
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Materials> materials;

    // ----------------------------morph shapeKeysNameID
    {
        static bool first = true;
        // first mesh storge all the shapekeys
        if (first)
            for (unsigned int i = 0; i < mesh->mNumAnimMeshes; ++i)
            {
                shapeKeysNameID.insert(make_pair(i, mesh->mAnimMeshes[i]->mName.data));
            }
        first = false;
    }

    //  ----------------------------morph
    unordered_map<string, vector<glm::vec3>> morphAnims;
    for (unsigned int i = 0; i < mesh->mNumAnimMeshes; ++i)
    {
        vector<glm::vec3> vecs;
        vecs.resize(mesh->mNumVertices);
        for (unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            vecs[j] = (AssimpGLMHelpers::GetGLMVec(mesh->mAnimMeshes[i]->mVertices[j] - mesh->mVertices[j]));
        }

        // The shapekey of the mesh has not changed, so it is not needed.
        bool nonZeroPresent = std::any_of(vecs.begin(), vecs.end(), [](const glm::vec3 &vec)
                                          { return vec != glm::vec3(.0f); });

        if (nonZeroPresent)
            morphAnims.insert(make_pair(mesh->mAnimMeshes[i]->mName.data, vecs));
    }

    // ----------------------------vertices
    vertices.resize(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        SetVertexBoneDataToDefault(vertex);

        vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);

        // normals
        if (mesh->HasNormals())
        {
            vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vertex.Tangent = AssimpGLMHelpers::GetGLMVec(mesh->mTangents[i]);
            // bitangent
            vertex.Bitangent = AssimpGLMHelpers::GetGLMVec(mesh->mBitangents[i]);
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices[i] = (vertex);
    }

    // ----------------------------indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // ----------------------------materials
    // DiffuseTexture = "map_Kd";
    // AmbientTexture = "map_Ka";
    // SpecularTexture = "map_Ks";
    // OpacityTexture = "map_d";
    // EmissiveTexture1 = "map_emissive";
    // EmissiveTexture2 = "map_Ke";
    // BumpTexture1 = "map_bump";
    // BumpTexture2 = "bump";
    // NormalTextureV1 = "map_Kn";
    // NormalTextureV2 = "norm";
    // ReflectionTexture = "refl";
    // DisplacementTexture1 = "map_disp";
    // DisplacementTexture2 = "disp";
    // SpecularityTexture = "map_ns";
    // RoughnessTexture = "map_Pr";
    // MetallicTexture = "map_Pm";
    // SheenTexture = "map_Ps";
    // RMATexture = "map_Ps";

    // gltf----------------------------obj---------------------------------name
    // aiTextureType_DIFFUSE           aiTextureType_DIFFUSE        map_Kd albedo      0
    // aiTextureType_NORMALS           aiTextureType_NORMALS        map_Kn normal      1
    // aiTextureType_METALNESS         aiTextureType_SPECULAR       map_Ks metallic    2
    // aiTextureType_DIFFUSE_ROUGHNESS aiTextureType_SHININESS      map_Ns roughness   3
    // aiTextureType_LIGHTMAP          aiTextureType_AMBIENT        map_Ka ao          4
    // aiTextureType_EMISSIVE          aiTextureType_EMISSIVE       map_Ke emissive    5
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    for (const auto &textureType : textureTypes)
    {
        loadMaterialTextures(materials, material, textureType.type, textureType.prefix);
    }

    // if there is no texture, colorOnly
    // if (materials.size() == 0)
    // {
    //     Materials material_temp;
    //     material_temp.id = -1;
    //     aiColor3D color;
    //     material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    //     material_temp.color = glm::vec3(color.r, color.g, color.b);
    //     materials.push_back(material_temp);
    // }

    // ---------------------------- BoneWeights
    ExtractBoneWeightForVertices(vertices, mesh);

    // return a mesh object created from the extracted mesh data
    if (morphAnims.size())
        return Mesh(vertices, indices, materials, morphAnims);
    else
        return Mesh(vertices, indices, materials);
}

void Model::ExtractBoneWeightForVertices(vector<Vertex> &vertices, aiMesh *mesh)
{
    int newBoneID = m_BoneInfoMap.size();
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = newBoneID;
            newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = newBoneID;
            ++newBoneID;
        }
        else
        {
            boneID = m_BoneInfoMap[boneName].id;
        }
        CHECK(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;
        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            unsigned int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            CHECK(vertexId <= vertices.size());
            if (weight != .0f)
                SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

void Model::loadMaterialTextures(vector<Materials> &materials, aiMaterial *mat, aiTextureType type, string typeName)
{
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < m_materials.size(); j++)
        {
            if (std::strcmp(m_materials[j].path.data(), str.C_Str()) == 0)
            {
                materials.push_back(m_materials[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        { // if texture hasn't been loaded already, load it
            Materials texture;

#if TEXTURE_THREAD
            unsigned int textureID;
            glGenTextures(1, &textureID);
            texture.id = textureID;
            string filepath = this->directory + '/' + str.C_Str();
            texture_threads.emplace_back(TextureFromFileThread, textureID, filepath);
#else
            texture.id = TextureFromFile(str.C_Str(), this->directory);
#endif
            // texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            materials.push_back(texture);
            m_materials.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
}