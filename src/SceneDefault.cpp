#include "SceneDefault.h"

const float CubesVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
    // Right face
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
};

const float FrameVertices[] = {
    // positions        // texture Coords
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

SceneDefault::SceneDefault(Camera *camera, unsigned int width, unsigned int height) : Scene(camera, width, height)
{
    // --------------------------------------------------- IBL capture geometry
    ResourceManager::SetVAO("cubes", {3, 3, 2}, CubesVertices, sizeof(CubesVertices));
    ResourceManager::SetVAO("frame", {3, 2}, FrameVertices, sizeof(FrameVertices));
    // --------------------------------------------------- model shader
    ResourceManager::SetShader("model", 18, 21).Bind();
    ResourceManager::GetShader("model").SetUniform4m("pvm", App::vp * mat4(1.0));
    // compile shader for model
    ResourceManager::SetShader("modelAnim", 23, 21).Bind();
    ResourceManager::GetShader("modelAnim").SetUniform4m("pvm", App::vp * mat4(1.0));

    // load model data by assimp
    ModelImporter::ModelImport(string(RES_DIR) + "model/hutao/hutao_multi2.fbx", &modelHutao, &pAnimations, &pAnimator, ResourceManager::GetShader("modelAnim"));

    // UI update
    App::animNames = pAnimations->GetAnimationNames();
    App::duration = pAnimator->GetAnimationDuration();

    // --------------------------------------------------- pbr model
    PBRObject::PBRInit(ResourceManager::SetShader("PBR", 15, 17));
    pbr_model = new PBRObject(ResourceManager::GetShader("PBR"), string(RES_DIR) + "model/DamagedHelmet/glTF/DamagedHelmet.gltf");
}

SceneDefault::~SceneDefault()
{
    DELETE_PTR(modelHutao)
    DELETE_PTR(pAnimations);
    DELETE_PTR(pAnimator);

    DELETE_PTR(pbr_model);
}

void SceneDefault::Render()
{
    glm::mat4 model = glm::mat4(1.0f);
    glf()->glEnable(GL_BLEND);

    //  --------------------------------------------------- model animation render
    if (App::animNames[App::animIndex] != pAnimator->GetAnimationName())
    {
        pAnimator->PlayAnimation(&pAnimations->GetAnimations()[App::animIndex]);
        App::duration = pAnimator->GetAnimationDuration();
    }
    if (App::playBackState)
    {
        pAnimator->UpdateAnimation(App::deltaTime * App::playSpeed);
        App::currentFrame = pAnimator->GetCurrentFrame();
    }
    else
    {
        pAnimator->SetCurrentTime(App::currentFrame);
        pAnimator->UpdateAnimation(.0f);
    }

    model = mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.5f, .0f, 0.5f));
    // model = glm::scale(model, glm::vec3(0.01, 0.01, 0.01));
    // model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));

    auto &modelAnimShader = ResourceManager::GetShader("modelAnim");
    modelAnimShader.Bind();
    modelAnimShader.SetUniform4m("pvm", App::vp * model);
    modelHutao->Draw(modelAnimShader);

    //  --------------------------------------------------- pbr model render
    ResourceManager::GetShader("PBR").Bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    model = glm::rotate(model, glm::radians((float)90), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
    pbr_model->Render(model, sceneCamera->m_cameraPos);
}
