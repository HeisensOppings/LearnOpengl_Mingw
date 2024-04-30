#include "SceneAnimations.h"

SceneAnimations::SceneAnimations(Camera *camera, unsigned int width, unsigned int height) : Scene(camera, width, height)
{
    // compile shader for model
    ResourceManager::SetShader("modelAnim", 25, 24).Bind();
    ResourceManager::GetShader("modelAnim").SetUniform4m("pvm", App::vp * mat4(1.0));

    // load model data by assimp
    ModelImporter::ModelImport(string(RES_DIR) + "model/hutao/hutao_multi2.fbx", &modelHutao, &pAnimations, &pAnimator, ResourceManager::GetShader("modelAnim"));

    // imgui update
    App::animNames = pAnimations->GetAnimationNames();
    App::duration = pAnimator->GetAnimationDuration();
}

SceneAnimations::~SceneAnimations()
{
    DELETE_PTR(modelHutao)
    DELETE_PTR(pAnimations);
    DELETE_PTR(pAnimator);
}

void SceneAnimations::Render()
{
    glm::mat4 model = glm::mat4(1.0f);
    glEnable(GL_BLEND);

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
}
