#pragma once

/* Container for bone data */
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <list>
#include <vector>

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AssimpGLMHelpers.h"

#define COUTL(param) cout << #param << ":" << param << endl
#define COUT(param) cout << #param << ":" << param << " "
#define CHECK(param)                                        \
    if (!(param))                                           \
    {                                                       \
        cout << __FUNCTION__ <<" "<< #param << ":error" << endl; \
        exit(-1);                                           \
    }
// Missing bones are usually bones that are not used in animation except pose.
// In joins in blender, set it to 1 to load the full bone information.
#define MISSING_BONES 0

struct BoneNode
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<BoneNode> children;
};

struct BoneInfo
{
    // id is index in finalBoneMatrices
    int id;
    // offset matrix transforms vertex from model space to bone space
    glm::mat4 offset;
};

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class Bone
{
public:
    Bone(const std::string &name, int ID, const aiNodeAnim *channel)
        : m_Name(name),
          m_ID(ID),
          m_LocalTransform(1.0f)
    {
        m_NumPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            m_Positions.push_back({AssimpGLMHelpers::GetGLMVec(aiPosition), timeStamp});
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            m_Rotations.push_back({AssimpGLMHelpers::GetGLMQuat(aiOrientation), timeStamp});
        }

        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            m_Scales.push_back({AssimpGLMHelpers::GetGLMVec(scale), timeStamp});
        }
    }

    void Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        m_LocalTransform = translation * rotation * scale;
    }

    glm::mat4 GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() { return m_ID; }

    int GetPositionIndex(float animationTime)
    {
        int index = 0;
        for (index = 0; index < m_NumPositions - 1; ++index)
        {
            if (animationTime < m_Positions[index + 1].timeStamp)
                return index;
        }
        // If "Force Start/End Keying" is not selected when exporting the model in blender
        // Return last frame key index
        return index;
    }

    int GetRotationIndex(float animationTime)
    {
        int index = 0;
        for (index = 0; index < m_NumRotations - 1; ++index)
        {
            if (animationTime < m_Rotations[index + 1].timeStamp)
                return index;
        }
        return index;
    }

    int GetScaleIndex(float animationTime)
    {
        int index = 0;
        for (index = 0; index < m_NumScalings - 1; ++index)
        {
            if (animationTime < m_Scales[index + 1].timeStamp)
                return index;
        }
        return index;
    }

private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        return ((animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp));
    }

    glm::mat4 InterpolatePosition(float animationTime)
    {
        if (1 == m_NumPositions)
            return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

        int pIndex = GetPositionIndex(animationTime);

        // If "Force Start/End Keying" is not selected when exporting the model in blender
        if ((pIndex == 0 && m_Positions[pIndex].timeStamp >= animationTime) || pIndex == m_NumPositions - 1)
            return glm::translate(glm::mat4(1.0f), m_Positions[pIndex].position);

        float scaleFactor = GetScaleFactor(m_Positions[pIndex].timeStamp, m_Positions[pIndex + 1].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(m_Positions[pIndex].position, m_Positions[pIndex + 1].position, scaleFactor);

        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 InterpolateRotation(float animationTime)
    {
        if (1 == m_NumRotations)
            return glm::toMat4(m_Rotations[0].orientation);

        int pIndex = GetRotationIndex(animationTime);

        if ((pIndex == 0 && m_Rotations[pIndex].timeStamp >= animationTime) || pIndex == m_NumRotations - 1)
            return glm::toMat4(m_Rotations[pIndex].orientation);

        float scaleFactor = GetScaleFactor(m_Rotations[pIndex].timeStamp, m_Rotations[pIndex + 1].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[pIndex].orientation, m_Rotations[pIndex + 1].orientation, scaleFactor);

        return glm::toMat4(finalRotation);
    }

    glm::mat4 InterpolateScaling(float animationTime)
    {
        if (1 == m_NumScalings)
            return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

        int pIndex = GetScaleIndex(animationTime);

        if ((pIndex == 0 && m_Scales[pIndex].timeStamp >= animationTime) || pIndex == m_NumScalings - 1)
            return glm::scale(glm::mat4(1.0f), m_Scales[pIndex].scale);

        float scaleFactor = GetScaleFactor(m_Scales[pIndex].timeStamp, m_Scales[pIndex + 1].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(m_Scales[pIndex].scale, m_Scales[pIndex + 1].scale, scaleFactor);

        return glm::scale(glm::mat4(1.0f), finalScale);
    }

    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;

public:
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    std::string m_Name;
    int m_ID;
    glm::mat4 m_LocalTransform;
};

typedef std::unordered_map<unsigned int, std::string> UIntStringMap;
typedef std::unordered_map<std::string, BoneInfo> StringBoneInfoMap;
typedef std::unordered_map<std::string, Bone> StringBoneMap;
