#include "scene.h"

#if 0 // OIT

glm::mat4 calculate_model_matrix(const glm::vec3 &position, const glm::vec3 &rotation = glm::vec3(0.0f), const glm::vec3 &scale = glm::vec3(1.0f));
glm::mat4 calculate_model_matrix(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
    glm::mat4 trans = glm::mat4(1.0f);

    trans = glm::translate(trans, position);
    trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
    trans = glm::rotate(trans, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
    trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, scale);

    return trans;
}

unsigned int opaqueFBO, transparentFBO;
unsigned int accumTexture;
unsigned int revealTexture;
unsigned int opaqueTexture;

// set up transformation matrices
// ------------------------------------------------------------------
glm::mat4 redModelMat = calculate_model_matrix(glm::vec3(0.0f, 0.0f, 0.0f));
glm::mat4 greenModelMat = calculate_model_matrix(glm::vec3(0.0f, 0.0f, 1.0f));
glm::mat4 blueModelMat = calculate_model_matrix(glm::vec3(0.0f, 0.0f, 2.0f));

// set up intermediate variables
// ------------------------------------------------------------------
glm::vec4 zeroFillerVec(0.0f);
glm::vec4 oneFillerVec(1.0f);

#endif

Scene::Scene(Camera *camera, unsigned int scr_width, unsigned int scr_height)
    : Width(scr_width), Height(scr_height)
{
    m_camera = camera;
    App::projection = glm::perspective(glm::radians(m_camera->m_Fov), (float)Width / (float)Height, zbuffer_near, zbuffer_far);
    App::view = m_camera->GetViewMatrix();
    App::vp = App::projection * App::view;

    // --------------------------------------------------- shader file
    ResourceManager::ShaderInit("./src/shaders.glsl");

    // --------------------------------------------------- OIT shader
#if 0 // OIT
    ResourceManager::SetShader("solid", 21, 25);
    ResourceManager::SetShader("transparent", 21, 26);
    ResourceManager::SetShader("composite", 22, 27);
    ResourceManager::SetShader("screen", 23, 28);
    ResourceManager::SetShader("solid1", 24, 29);
    ResourceManager::SetShader("transparent1", 24, 30);
#endif

    // --------------------------------------------------- skybox vao
    ResourceManager::SetVAO("cubes", {3, 3, 2}, CubesVertices, sizeof(CubesVertices));
    ResourceManager::SetVAO("frame", {3, 2}, FrameVertices, sizeof(FrameVertices));

    // --------------------------------------------------- model shader
    ResourceManager::SetShader("model", 20, 24).Bind();
    ResourceManager::GetShader("model").SetUniform4m("pvm", App::vp * mat4(1.0));
    // --------------------------------------------------- model animation shader
    ResourceManager::SetShader("modelAnim", 25, 24).Bind();
    ResourceManager::GetShader("modelAnim").SetUniform4m("pvm", App::vp * mat4(1.0));

    // --------------------------------------------------- normal model
    // model_miku = new Model("./model/YYB Hatsune Miku_10th/YYB Hatsune Miku_10th_v1.02.pmx");

    ModelImporter::ModelImport("./model/hutao/hutao_multi2.fbx", &model_aru, &pAnimations, &pAnimator, ResourceManager::GetShader("modelAnim"));
    // ModelImporter::ModelImport("./model/niru/niru.fbx", &model_aru, &pAnimations, &pAnimator, ResourceManager::GetShader("modelAnim"));
    // ModelImporter::ModelImport("./model/Aru/aru.gltf", &model_aru, &pAnimations, &pAnimator, ResourceManager::GetShader("modelAnim"));

    // model_IndoorScene_Dq_Uyutei = new Model("./model/Dq_Uyutei/IndoorScene_Dq_Uyutei.obj");

    App::animNames = pAnimations->GetAnimationNames();
    App::duration = pAnimator->GetAnimationDuration();

    // --------------------------------------------------- pbr model
    PBRObject::PBRInit(ResourceManager::SetShader("PBR", 16, 18), "./src/image/newport_loft.hdr");
    pbr_model = new PBRObject(ResourceManager::GetShader("PBR"), "./model/DamagedHelmet/glTF/DamagedHelmet.gltf");

    // --------------------------------------------------- skybox
    // need to be after PBRObject -- PBRObject::envCubemap
    skybox = new SKYObject(ResourceManager::SetShader("SKY", 4, 4), ResourceManager::GetVAO("cubes"), PBRObject::envCubemap);

    // --------------------------------------------------- text
    text = new TextObject(ResourceManager::SetShader("text", 19, 23), u32text_string, vec2(0.0), vec2(8.0, 0.0));

#if 0 // OIT
    // set up framebuffers and their texture attachments
    // ------------------------------------------------------------------
    // unsigned int opaqueFBO, transparentFBO;
    glGenFramebuffers(1, &opaqueFBO);
    glGenFramebuffers(1, &transparentFBO);
    // set up attachments for opaque framebuffer
    // unsigned int opaqueTexture;
    glGenTextures(1, &opaqueTexture);
    glBindTexture(GL_TEXTURE_2D, opaqueTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, App::scr_width, App::scr_height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, App::scr_width, App::scr_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opaqueTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Opaque framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // set up attachments for transparent framebuffer
    // unsigned int accumTexture;
    glGenTextures(1, &accumTexture);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, App::scr_width, App::scr_height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // unsigned int revealTexture;
    glGenTextures(1, &revealTexture);
    glBindTexture(GL_TEXTURE_2D, revealTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, App::scr_width, App::scr_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0); // opaque framebuffer's depth texture

    const GLenum transparentDrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, transparentDrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Transparent framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

void Scene::Update(float dt)
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    App::view = m_camera->GetViewMatrix();
    App::vp = App::projection * App::view;
    App::deltaTime = dt;
}

void Scene::Render()
{
    glm::mat4 model = glm::mat4(1.0f);

#if 0 // OIT
    // configure render states
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // bind opaque framebuffer to render solid objects
    glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);
    // skybox->Render();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    model = glm::rotate(model, glm::radians((float)90), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
    pbr_model->Render(model, m_camera->m_cameraPos);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    ResourceManager::GetVAO("frame").Bind();

    // // draw model_anon
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    model = mat4(1.0);
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.5f));
    model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
    ResourceManager::GetShader("solid1").Bind();
    ResourceManager::GetShader("solid1").SetUniform4m("mvp", App::vp * model);
    model_miku->Draw(ResourceManager::GetShader("solid1"));
    // model_anon->Draw(ResourceManager::GetShader("solid1"));

    // // ResourceManager::GetShader("model").Bind();
    // // ResourceManager::GetShader("model").SetUniform4m("mvp", App::vp * model);
    // // model_anon->Draw(ResourceManager::GetShader("model"));

    // use solid shader
    ResourceManager::GetShader("solid").Bind();

    // draw red quad
    ResourceManager::GetShader("solid").SetUniform4m("mvp", App::vp * redModelMat);
    ResourceManager::GetShader("solid").SetUniform3f("color", glm::vec3(1.0f, 0.0f, 0.0f));
    ResourceManager::GetVAO("frame").Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // draw transparent objects (transparent pass)
    // -----

    // configure render states
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    glBlendEquation(GL_FUNC_ADD);

    // bind transparent framebuffer to render transparent objects
    glBindFramebuffer(GL_FRAMEBUFFER, transparentFBO);
    glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
    glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

    // draw model_anon
    model = mat4(1.0);
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.5f));
    model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
    // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    ResourceManager::GetShader("transparent1").Bind();
    ResourceManager::GetShader("transparent1").SetUniform4m("mvp", App::vp * model);
    model_miku->Draw(ResourceManager::GetShader("transparent1"));
    // model_anon->Draw(ResourceManager::GetShader("transparent1"));

    // use transparent shader
    ResourceManager::GetShader("transparent").Bind();

    // draw green quad
    ResourceManager::GetShader("transparent").SetUniform4m("mvp", App::vp * greenModelMat);
    ResourceManager::GetShader("transparent").SetUniform4f("color", glm::vec4(0.0f, 1.0f, 0.0f, 0.5f));
    ResourceManager::GetVAO("frame").Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // draw blue quad
    ResourceManager::GetShader("transparent").SetUniform4m("mvp", App::vp * blueModelMat);
    ResourceManager::GetShader("transparent").SetUniform4f("color", glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
    ResourceManager::GetVAO("frame").Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // draw composite image (composite pass)
    // -----

    // set render states
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // bind opaque framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, opaqueFBO);

    // use composite shader
    ResourceManager::GetShader("composite").Bind();

    // draw screen quad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, revealTexture);
    ResourceManager::GetVAO("frame").Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // draw to backbuffer (final pass)
    // -----

    // set render states
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
    glDisable(GL_BLEND);

    // bind backbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // use screen shader
    ResourceManager::GetShader("screen").Bind();

    // draw final screen quad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, opaqueTexture);
    ResourceManager::GetVAO("frame").Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#endif

    glEnable(GL_BLEND);

    //  --------------------------------------------------- normal model render
    // model = mat4(1.0f);
    // model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.5f));
    // // model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
    // model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
    // // model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    // ResourceManager::GetShader("model").Bind();
    // ResourceManager::GetShader("model").SetUniform4m("pvm", App::vp * model);
    // // model_miku->Draw(ResourceManager::GetShader("model"));
    // // model_aru->Draw(ResourceManager::GetShader("model"));
    // // model_DamageHelmet->Draw(ResourceManager::GetShader("model"));
    // model_IndoorScene_Dq_Uyutei->Draw(ResourceManager::GetShader("model"));

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
    model_aru->Draw(modelAnimShader);

    //  --------------------------------------------------- pbr model render
    ResourceManager::GetShader("PBR").Bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
    model = glm::rotate(model, glm::radians((float)90), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
    pbr_model->Render(model, m_camera->m_cameraPos);

    //  --------------------------------------------------- skybox render
    skybox->Render();

    //  --------------------------------------------------- text render
    model = glm::mat4(1.0f);
    model = glm::scale(model, vec3(0.01));
    // text->LoadString(u32text_string1, vec2(.0), vec2(8.0, 0.0));
    text->Render(model);
}
