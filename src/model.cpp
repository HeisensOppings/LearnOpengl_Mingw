#include "model.h"

bool texture_dir_output = false;

vector<string> textureTypeStr{
    "NONE",
    "DIFFUSE",
    "SPECULAR",
    "AMBIENT",
    "EMISSIVE",
    "HEIGHT",
    "NORMALS",
    "SHININESS",
    "OPACITY",
    "DISPLACEMENT",
    "LIGHTMAP",
    "REFLECTION",
    "BASE_COLOR",
    "NORMAL_CAMERA",
    "EMISSION_COLOR",
    "METALNESS",
    "DIFFUSE_ROUGHNESS",
    "AMBIENT_OCCLUSION",
    "SHEEN",
    "CLEARCOAT",
    "TRANSMISSION",
    "UNKNOWN"};

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

Model::Model(string const &path, [[maybe_unused]] bool gamma)
{
    loadModel(path);
}

void Model::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); ++i)
        meshes[i].Draw(shader);
}

void Model::DrawInstance(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); ++i)
        meshes[i].DrawInstance(shader);
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
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
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
    vector<Texture_config> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
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
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // map_Kd map_Ks map_Ka map_Ke map_Bump map_Kn map_Ns map_d map_disp map_Pr map_Ps map_Pm

    // 1 map_Kd        DIFFUSE      albedo diffuse
    // 2 map_Ks        SPECULAR     metallic specular
    // 3 map_Ka        AMBIENT      ambient occlusion
    // 4 map_Ke        EMISSIVE     emissive
    // 5 map_Bump      HEIGHT       height
    // 6 map_Kn        NORMALS      normal
    // 7 map_Ns        SHININESS    roughness
    // 8 map_d         OPACITY
    // 9 map_disp      DISPLACEMENT

    // static const std::string DiffuseTexture = "map_Kd";
    // static const std::string AmbientTexture = "map_Ka";
    // static const std::string SpecularTexture = "map_Ks";
    // static const std::string OpacityTexture = "map_d";
    // static const std::string EmissiveTexture1 = "map_emissive";
    // static const std::string EmissiveTexture2 = "map_Ke";
    // static const std::string BumpTexture1 = "map_bump";
    // static const std::string BumpTexture2 = "bump";
    // static const std::string NormalTextureV1 = "map_Kn";
    // static const std::string NormalTextureV2 = "norm";
    // static const std::string ReflectionTexture = "refl";
    // static const std::string DisplacementTexture1 = "map_disp";
    // static const std::string DisplacementTexture2 = "disp";
    // static const std::string SpecularityTexture = "map_ns";
    // static const std::string RoughnessTexture = "map_Pr";
    // static const std::string MetallicTexture = "map_Pm";
    // static const std::string SheenTexture = "map_Ps";
    // static const std::string RMATexture = "map_Ps";

    // gltf----------------------------obj---------------------------------name
    // aiTextureType_DIFFUSE           aiTextureType_DIFFUSE        map_Kd albedo      0      
    // aiTextureType_NORMALS           aiTextureType_NORMALS        map_Kn normal      1
    // aiTextureType_METALNESS         aiTextureType_SPECULAR       map_Ks metallic    2
    // aiTextureType_DIFFUSE_ROUGHNESS aiTextureType_SHININESS      map_Ns roughness   3
    // aiTextureType_LIGHTMAP          aiTextureType_AMBIENT        map_Ka ao          4
    // aiTextureType_EMISSIVE          aiTextureType_EMISSIVE       map_Ke emissive    5

    vector<Texture_config> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "d");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    vector<Texture_config> nMaps1 = loadMaterialTextures(material, aiTextureType_HEIGHT, "n");
    textures.insert(textures.end(), nMaps1.begin(), nMaps1.end());
    vector<Texture_config> nMaps2 = loadMaterialTextures(material, aiTextureType_NORMALS, "n");
    textures.insert(textures.end(), nMaps2.begin(), nMaps2.end());

    vector<Texture_config> mMaps1 = loadMaterialTextures(material, aiTextureType_METALNESS, "m");
    textures.insert(textures.end(), mMaps1.begin(), mMaps1.end());
    vector<Texture_config> mMaps2 = loadMaterialTextures(material, aiTextureType_SPECULAR, "m");
    textures.insert(textures.end(), mMaps2.begin(), mMaps2.end());

    vector<Texture_config> rMaps1 = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "r");
    textures.insert(textures.end(), rMaps1.begin(), rMaps1.end());
    vector<Texture_config> rMaps2 = loadMaterialTextures(material, aiTextureType_SHININESS, "r");
    textures.insert(textures.end(), rMaps2.begin(), rMaps2.end());

    vector<Texture_config> aMaps1 = loadMaterialTextures(material, aiTextureType_LIGHTMAP, "a");
    textures.insert(textures.end(), aMaps1.begin(), aMaps1.end());
    vector<Texture_config> aMaps2 = loadMaterialTextures(material, aiTextureType_AMBIENT, "a");
    textures.insert(textures.end(), aMaps2.begin(), aMaps2.end());

    vector<Texture_config> eMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "e");
    textures.insert(textures.end(), eMaps.begin(), eMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}
vector<Texture_config> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    // if (texture_dir_output)
    // {
    //     // aiMaterial *mat;
    //     for (int ty = aiTextureType_NONE; ty <= aiTextureType_TRANSMISSION; ++ty)
    //     {
    //         aiString str_;
    //         mat->GetTextureCount((aiTextureType)ty);
    //         // if (mat->GetTextureCount((aiTextureType)ty))
    //         {
    //             mat->GetTexture((aiTextureType)ty, 0, &str_);
    //             cout << setw(18) << textureTypeStr[ty] << " "
    //                  << setw(2) << mat->GetTextureCount((aiTextureType)ty) << " " << setw(2) << ty << ": " << str_.C_Str() << endl;
    //         }
    //     }
    // }
    vector<Texture_config> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        { // if texture hasn't been loaded already, load it
            Texture_config texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            // texture.id = TextureFromFile(str.C_Str(), this->directory, typeName == "texture_diffuse" ? true : false);
            texture.type = typeName;
            texture.path = str.C_Str();
            // if (mat->GetTextureCount(aiTextureType_NORMALS))
                // cout << texture.id << " " << texture.type << " " << texture.path << endl;
            textures.push_back(texture);
            textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    // if (texture_dir_output)
        // texture_dir_output = false;
    return textures;
}