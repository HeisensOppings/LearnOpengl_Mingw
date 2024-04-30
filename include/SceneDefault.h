#pragma once
#include "scene.h"

class SceneDefault : public Scene
{
public:
    Model *modelHutao = nullptr;
    Animations *pAnimations = nullptr;
    Animator *pAnimator = nullptr;
    
    TextObject *text = nullptr;
    PBRObject *pbr_model = nullptr;
    SKYObject *skybox = nullptr;

    SceneDefault(Camera *camera, unsigned int width, unsigned int height);
    virtual ~SceneDefault() override;
    virtual void Render() override;
};