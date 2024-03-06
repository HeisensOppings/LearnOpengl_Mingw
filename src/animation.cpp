#include "animation.h"

Animation::Animation(const aiAnimation *animation, StringBoneInfoMap &modelBoneInfoMap, UIntStringMap &modelShapeKeysNameID)
{
    cout << endl
         << "-----------Animation:"
         << "Name:" << animation->mName.data << endl;
    m_Name = animation->mName.data;
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;
    cout << " Tick:" << m_TicksPerSecond << " Duration:" << m_Duration << " ";

    ReadBonesAnims(animation, modelBoneInfoMap);
    ReadMorphAnims(animation, modelShapeKeysNameID);
}

Bone *Animation::FindBone(const string &name)
{
    auto iter = m_BoneKeys.find(name);
    if (iter != m_BoneKeys.end())
        return &(iter->second);
    return nullptr;
}

void Animation::ReadBonesAnims(const aiAnimation *animation, StringBoneInfoMap &modelBoneInfoMap)
{
    // COUT(animation->mNumChannels);

    auto &boneInfoMap = modelBoneInfoMap;
    // reading channels(bones engaged in an animation and their keyframes)
    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        // check missing bone
        if (boneInfoMap.find(boneName) != boneInfoMap.end())
            m_BoneKeys.insert(std::make_pair(boneName, Bone(boneName, boneInfoMap[boneName].id, channel)));
#if (MISSING_BONES)
        // add missing bones
        else
        {
            boneInfoMap[boneName].id = boneInfoMap.size();
            cout << "missing bone:" << boneName << " ";
        }
#endif
    }
    COUT(m_BoneKeys.size());
}

void Animation::ReadMorphAnims(const aiAnimation *animation, UIntStringMap &modelShapeKeysNameID)
{
    if (animation->mNumMorphMeshChannels)
    {
        auto morphmeshchannels = animation->mMorphMeshChannels[0];
        cout << "morphName:" << morphmeshchannels->mName.data << " ";

        for (unsigned int i = 0; i < morphmeshchannels->mNumKeys; ++i)
        {
            auto &keys = morphmeshchannels->mKeys[i];

            unordered_map<string, float> shapekeys;
            for (unsigned int j = 0; j < keys.mNumValuesAndWeights; ++j)
            {
                // cout << "v:" << keys.mValues[j] << " w:" << keys.mWeights[j] << " | ";
                shapekeys.insert(make_pair(modelShapeKeysNameID[(keys.mValues[j])].data(), keys.mWeights[j]));
            }

            // insert missing from last frame
            if (m_MorphKeys.size())
                for (auto &lastShapeKey : m_MorphKeys.back().shapekeys)
                {
                    if (shapekeys.count(lastShapeKey.first) == 0 && lastShapeKey.second != 0)
                        shapekeys.insert(lastShapeKey);
                }

            m_MorphKeys.emplace_back(Morph{shapekeys, keys.mTime});
        }

        // If "Force Start/End Key" is selected when exporting the fbx model in blender
        // This will add all shapekeys to the start / end frames, we don't need these
        // Reomve first and last frame
        if (modelShapeKeysNameID.size() == (*(m_MorphKeys.begin())).shapekeys.size())
            m_MorphKeys.erase(m_MorphKeys.begin());
        if (modelShapeKeysNameID.size() == (*(m_MorphKeys.end() - 1)).shapekeys.size())
            m_MorphKeys.pop_back();
    }
    COUTL(m_MorphKeys.size());
}

unordered_map<string, float> Animation::morphAnimUpdate(float animationTime)
{
    unordered_map<string, float> morphAnimKey;
    // Get morphAnim index
    unsigned int index;
    if (m_MorphKeys.size())
    {
        for (index = 0; index < m_MorphKeys.size() - 1; ++index)
        {
            if (animationTime < m_MorphKeys[index + 1].timeStamp)
                break;
        }
    }
    else
        return morphAnimKey;

    // first key frame and last key frame
    if (index == 0 || index == m_MorphKeys.size() - 1)
        return m_MorphKeys[index].shapekeys;

    int p0Index = index;
    int p1Index = p0Index + 1;

    float scaleFactor = (animationTime - m_MorphKeys[p0Index].timeStamp) / (m_MorphKeys[p1Index].timeStamp - m_MorphKeys[p0Index].timeStamp);

    for (const auto &pair : m_MorphKeys[p0Index].shapekeys)
    {
        string shapeKeyName = pair.first;
        float p0_weight = pair.second;
        float fin_weight;
        if (m_MorphKeys[p1Index].shapekeys.count(pair.first) != 0)
        {
            float p1_weight = m_MorphKeys[p1Index].shapekeys[pair.first];
            fin_weight = glm::mix(p0_weight, p1_weight, scaleFactor);
        }
        else
            fin_weight = p0_weight;

        morphAnimKey.emplace(shapeKeyName, fin_weight);
        // cout << shapeKeyName << ":" << fin_weight << " ";
    }
    return morphAnimKey;
}

Animations::Animations(const std::string &animationPath, Model *model)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    CHECK(scene && scene->mRootNode);

    // load bone nodes
    ReadHierarchyData(m_RootBoneNode, scene->mRootNode);

    // load bone morph animations
    COUTL(scene->mNumAnimations);
    for (unsigned int index = 0; index < scene->mNumAnimations; ++index)
    {
        auto animation = scene->mAnimations[index];
        m_Animations.emplace_back(Animation(animation, model->GetBoneInfoMap(), model->shapeKeysNameID));
        m_Names.push_back(animation->mName.data);
    }
}

Animations::Animations(const aiScene *scene, Model *model)
{
    // load bone nodes
    ReadHierarchyData(m_RootBoneNode, scene->mRootNode);

    // load bone morph animations
    COUTL(scene->mNumAnimations);
    for (unsigned int index = 0; index < scene->mNumAnimations; ++index)
    {
        auto animation = scene->mAnimations[index];
        m_Animations.emplace_back(Animation(animation, model->GetBoneInfoMap(), model->shapeKeysNameID));
        m_Names.push_back(animation->mName.data);
    }
}

void Animations::ReadHierarchyData(BoneNode &dest, const aiNode *src)
{
    CHECK(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (unsigned int i = 0; i < src->mNumChildren; i++)
    {
        BoneNode newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

Animator::Animator(Animations *animations, Model *model)
{
    if (animations == nullptr || animations->GetAnimations().empty())
    {
        std::cout << "animations is empty" << std::endl;
        exit(-1);
    }

    m_Animations = animations;
    m_Model = model;

    // default first animation
    m_CurrentTime = 0.0;
    m_CurrentAnimation = &m_Animations->GetAnimations()[0];
    m_FinalBoneMatrices.resize(m_Model->GetBoneInfoMap().size());

    // ---------------------------------- sampler2D boneMatrixImage;
    glGenTextures(1, &boneMatrixTexture);
    glBindTexture(GL_TEXTURE_2D, boneMatrixTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, m_Model->GetBoneInfoMap().size(), 0, GL_RGBA, GL_FLOAT, NULL);
}

void Animator::UpdateAnimation(float dt)
{
    if (m_CurrentAnimation)
    {
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

        auto morphAnimKeys = m_CurrentAnimation->morphAnimUpdate(m_CurrentTime);
        m_Model->SetMorphAnimKeys(morphAnimKeys);

        CalculateBoneTransform(&m_Animations->GetBoneRootNode(), glm::mat4(1.0f));
    }

    // ---------------------------  sampler2D boneMatrixImage;
    std::vector<float> pixelData(m_FinalBoneMatrices.size() * 16);
    memcpy(pixelData.data(), m_FinalBoneMatrices.data(), pixelData.size() * sizeof(float));
    // int elementIndex = 0;
    // for (unsigned int i = 0; i < transforms.size(); ++i)
    // {
    //     glm::mat4 boneMatrix = transforms[i];
    //     // loop row and column
    //     for (int j = 0; j < 4; ++j)
    //     {
    //         for (int k = 0; k < 4; ++k, ++elementIndex)
    //         {
    //             pixelData[elementIndex] = boneMatrix[j][k];
    //         }
    //     }
    // }
    // "boneMatrixImage", 10 -- GL_TEXTURE10
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, boneMatrixTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, m_FinalBoneMatrices.size(), GL_RGBA, GL_FLOAT, &pixelData[0]);
}

void Animator::CalculateBoneTransform(const BoneNode *node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto &boneInfoMap = m_Model->GetBoneInfoMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        m_FinalBoneMatrices[boneInfoMap.at(nodeName).id] = globalTransformation * boneInfoMap.at(nodeName).offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
    {
        CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}

void ModelImporter::ModelImport(const string path, Model **model, Animations **animations, Animator **animator, Shader &shader)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    CHECK(scene && scene->mRootNode);
    shader.Bind();
    shader.SetUniform1i("boneMatrixImage", 10);

    if (*model != nullptr || *animations != nullptr || *animator != nullptr)
        cout
            << __FUNCTION__ << ":error" << endl;

    *model = new Model(scene, path);
    *animations = new Animations(scene, *model);
    *animator = new Animator(*animations, *model);
}
