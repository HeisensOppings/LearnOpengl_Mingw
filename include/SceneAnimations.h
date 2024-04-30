#pragma once
#include "scene.h"

class SceneAnimations : public Scene
{
public:
    Model *modelHutao = nullptr;
    Animations *pAnimations = nullptr;
    Animator *pAnimator = nullptr;
    SceneAnimations(Camera *camera, unsigned int width, unsigned int height);
    virtual ~SceneAnimations() override;
    virtual void Render() override;
};