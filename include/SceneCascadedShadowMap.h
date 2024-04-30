#pragma once
#include "scene.h"

// CSM
class SceneCascadedShadowMap : public Scene
{
public:
    Model *modelHutao = nullptr;
    Texture *texture = nullptr;
    VertexArray *planeVA = nullptr;
    VertexArray *quadVA = nullptr;
    unsigned int matricesUBO;
    unsigned int lightFBO;
    unsigned int lightDepthMaps;

    unsigned int debugLayer = 0;
    const glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 5.0f, 1.0f));
    unsigned int depthMapResolution = 1920;
    std::vector<glm::mat4> lightMatricesCache;
    std::vector<float> shadowCascadeLevels{5.0f, 10.0f, 20.0f};

    glm::mat4 cameraViewMatric;
    float cameraNearPlane = 0.001f;
    float cameraFarPlane = 30.0f;

    SceneCascadedShadowMap(Camera *camera, unsigned int width, unsigned int height);
    virtual ~SceneCascadedShadowMap() override;
    virtual void Render() override;
    virtual void processInput(GLFWwindow *window) override;

    void renderScene(Shader &shader);
    void renderQuad();
    std::vector<glm::mat4> getLightSpaceMatrices();
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &projview);
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view);
    glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane);
    void drawCascadeVolumeVisualizers(const std::vector<glm::mat4> &lightMatrices, Shader *shader);
    void drawVisualizerFrustums(Shader *shader);
};
