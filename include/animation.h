#pragma once
#include "bone.h"
#include "model.h"

struct Morph
{
    // shapekey names and weight
    unordered_map<string, float> shapekeys;
    double timeStamp;
};

class Animation
{
public:
    Animation(const aiAnimation *animation, StringBoneInfoMap &modelBoneInfoMap, UIntStringMap &modelShapeKeysNameID);
    Bone *FindBone(const string &name);
    inline float GetTicksPerSecond() { return m_TicksPerSecond; }
    inline float GetDuration() { return m_Duration; }

public:
    // laod bone animations, also check missing bone
    void ReadBonesAnims(const aiAnimation *animation, StringBoneInfoMap &modelBoneInfoMap);
    void ReadMorphAnims(const aiAnimation *animation, UIntStringMap &modelShapeKeysNameID);

    unordered_map<string, float> morphAnimUpdate(float animationTime);

    string m_Name;
    float m_Duration;
    int m_TicksPerSecond;

    StringBoneMap m_BoneKeys;
    vector<Morph> m_MorphKeys;
};

class Animations
{
private:
    vector<Animation> m_Animations;
    vector<string> m_Names;
    BoneNode m_RootBoneNode;

public:
    Animations(const std::string &animationPath, Model *model);
    Animations(const aiScene *scene, Model *model);
    vector<Animation> &GetAnimations() { return m_Animations; }
    vector<string> GetAnimationNames() { return m_Names; }

    void ReadHierarchyData(BoneNode &dest, const aiNode *src);

    inline const BoneNode &GetBoneRootNode() { return m_RootBoneNode; }
};

class Animator
{
public:
    Animator(Animations *animations, Model *model);

    void UpdateAnimation(float dt);
    void PlayAnimation(Animation *pAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
    }
    inline void SetCurrentTime(float time) { m_CurrentTime = time; }

    void CalculateBoneTransform(const BoneNode *node, glm::mat4 parentTransform);

    inline std::vector<glm::mat4> &GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
    inline float GetAnimationDuration() { return m_CurrentAnimation->m_Duration; }
    inline float GetCurrentFrame() { return m_CurrentTime; }
    inline std::string GetAnimationName() { return m_CurrentAnimation->m_Name; }

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation *m_CurrentAnimation;
    Animations *m_Animations;
    Model *m_Model;
    float m_CurrentTime;
    unsigned int boneMatrixTexture;
};

class ModelImporter
{
private:
    Model *m_Model = nullptr;
    Animations *m_Animations = nullptr;
    Animator *m_Animator = nullptr;
    Shader *m_Shader = nullptr;

public:
    static void ModelImport(const string path, Model **model, Animations **animations, Animator **animator, Shader &shader);
};