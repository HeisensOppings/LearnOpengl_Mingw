#include "glwidget.h"

#include <QCursor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QWheelEvent>

#include <algorithm>
#include <exception>
#include <iostream>

#include "glfuncs.h"
#include "appcontrol.h"
#include "camera.h"
#include "resource.h"
#include "SceneDefault.h"
#include "SceneAnimations.h"
#include "SceneCascadedShadowMap.h"

GLWidget::GLWidget(SceneKind kind, QWidget *parent) : QOpenGLWidget(parent), m_kind(kind)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // the camera does not need a GL context: create it now so the saved window
    // position can be restored before the window is shown.
    m_camera = new Camera();
    int x = 0, y = 0;
    m_hasSavedPos = m_camera->loadCameraPosition(&x, &y);
    m_savedPos = QPoint(x, y);
}

GLWidget::~GLWidget()
{
    cleanup();
    delete m_camera;
}

bool GLWidget::savedWindowPos(QPoint &pos) const
{
    if (m_hasSavedPos)
        pos = m_savedPos;
    return m_hasSavedPos;
}

void GLWidget::saveCameraState(const QPoint &windowPos)
{
    m_camera->saveCameraPosition(windowPos.x(), windowPos.y());
}

void GLWidget::cleanup()
{
    if (!m_scene)
        return;
    // all GL objects (shaders, VAOs, textures, FBOs) must be released with the
    // widget's context current.
    makeCurrent();
    delete m_scene; // Scene::~Scene() also clears ResourceManager
    m_scene = nullptr;
    doneCurrent();
}

void GLWidget::initializeGL()
{
    // release resources before the context goes away (e.g. widget re-parenting / app exit)
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup, Qt::UniqueConnection);

    const QSurfaceFormat fmt = context()->format();
    std::cout << "OpenGL context: " << fmt.majorVersion() << "." << fmt.minorVersion()
              << (fmt.profile() == QSurfaceFormat::CoreProfile ? " core" : "") << std::endl;
    if (!glf())
    {
        emit fatalError(tr("OpenGL 4.2 core profile is not available (got %1.%2).")
                            .arg(fmt.majorVersion())
                            .arg(fmt.minorVersion()));
        return;
    }

    auto *gl = glf();
    std::cout << "GL_RENDERER: " << reinterpret_cast<const char *>(gl->glGetString(GL_RENDERER)) << std::endl;

    App::defaultFBO = defaultFramebufferObject();
    App::scr_width = std::max(1, int(width() * devicePixelRatio()));
    App::scr_height = std::max(1, int(height() * devicePixelRatio()));
    gl->glViewport(0, 0, App::scr_width, App::scr_height);
    gl->glEnable(GL_MULTISAMPLE);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);
    gl->glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QElapsedTimer initClock;
    initClock.start();
    try
    {
        ResourceManager::ShaderInit(std::string(RES_DIR) + "shaders.glsl");
        switch (m_kind)
        {
        case SceneKind::CascadedShadowMap:
            m_scene = new SceneCascadedShadowMap(m_camera, App::scr_width, App::scr_height);
            break;
        case SceneKind::Animations:
            m_scene = new SceneAnimations(m_camera, App::scr_width, App::scr_height);
            break;
        default:
            m_scene = new SceneDefault(m_camera, App::scr_width, App::scr_height);
            break;
        }
    }
    catch (const std::exception &e)
    {
        emit fatalError(QString::fromLocal8Bit(e.what()));
        return;
    }
    std::cout << "Initialization time(s):" << initClock.elapsed() / 1000.0 << std::endl;

    m_clock.start();
    m_lastFrame = 0.0f;
    // continuous rendering: schedule the next frame once the current one was presented
    connect(this, &QOpenGLWidget::frameSwapped, this, QOverload<>::of(&QWidget::update), Qt::UniqueConnection);
    emit sceneReady();
}

void GLWidget::resizeGL(int w, int h)
{
    App::scr_width = std::max(1, int(w * devicePixelRatio()));
    App::scr_height = std::max(1, int(h * devicePixelRatio()));
    if (m_scene)
        m_scene->Resize(App::scr_width, App::scr_height);
}

void GLWidget::paintGL()
{
    if (!m_scene)
        return;

    // QOpenGLWidget renders into its own FBO, which may be recreated on resize:
    // every "bind framebuffer 0" in the engine uses App::defaultFBO instead.
    App::defaultFBO = defaultFramebufferObject();
    App::scr_width = std::max(1, int(width() * devicePixelRatio()));
    App::scr_height = std::max(1, int(height() * devicePixelRatio()));
    m_scene->Resize(App::scr_width, App::scr_height);
    glf()->glViewport(0, 0, App::scr_width, App::scr_height);

    const float now = m_clock.nsecsElapsed() / 1e9f;
    const float dt = now - m_lastFrame;
    m_lastFrame = now;
    App::elapsedTime = now;

    processHeldKeys(dt);

    m_scene->Update(dt);
    m_scene->Render();

    ++m_statFrames;
    m_statTime += dt;
    if (m_statTime >= 0.25f)
    {
        const double fps = m_statFrames / m_statTime;
        emit frameStats(1000.0 / fps, fps);
        m_statFrames = 0;
        m_statTime = 0.0f;
    }
}

void GLWidget::processHeldKeys(float dt)
{
    if (m_heldKeys.contains(Qt::Key_D))
        m_camera->ProcessKeyBoard(MOVE_RIGHT, dt);
    if (m_heldKeys.contains(Qt::Key_A))
        m_camera->ProcessKeyBoard(MOVE_LEFT, dt);
    if (m_heldKeys.contains(Qt::Key_W))
        m_camera->ProcessKeyBoard(MOVE_FORWARD, dt);
    if (m_heldKeys.contains(Qt::Key_S))
        m_camera->ProcessKeyBoard(MOVE_BACKWARD, dt);
    if (m_heldKeys.contains(Qt::Key_Space))
        m_camera->ProcessKeyBoard(MOVE_UP, dt);
    if (m_heldKeys.contains(Qt::Key_Shift))
        m_camera->ProcessKeyBoard(MOVE_DOWN, dt);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (m_mouseLook)
            setMouseLook(false);
        else
            window()->close();
        return;
    }
    if (!event->isAutoRepeat())
        m_heldKeys.insert(event->key());
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
    m_heldKeys.remove(event->key());
    if (m_scene)
        m_scene->keyReleased(event->key());
}

void GLWidget::focusOutEvent(QFocusEvent *event)
{
    m_heldKeys.clear();
    setMouseLook(false);
    QOpenGLWidget::focusOutEvent(event);
}

void GLWidget::setMouseLook(bool enabled)
{
    if (m_mouseLook == enabled)
        return;
    m_mouseLook = enabled;
    if (enabled)
    {
        setCursor(Qt::BlankCursor);
        grabMouse();
        m_skipNextMove = true;
        QCursor::setPos(mapToGlobal(rect().center()));
    }
    else
    {
        releaseMouse();
        unsetCursor();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus(Qt::MouseFocusReason);
    if (event->button() == Qt::RightButton)
        setMouseLook(!m_mouseLook);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_mouseLook)
        return;
    const QPoint center = rect().center();
    const QPoint delta = event->position().toPoint() - center;
    if (m_skipNextMove)
    {
        m_skipNextMove = false;
    }
    else if (!delta.isNull())
    {
        // y is inverted: moving the mouse up looks up
        m_camera->ProcessMouseMovement(float(delta.x()), float(-delta.y()));
    }
    if (!delta.isNull())
        QCursor::setPos(mapToGlobal(center));
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    const float steps = event->angleDelta().y() / 120.0f;
    if (steps == 0.0f)
        return;
    if (m_mouseLook && (event->modifiers() & Qt::ControlModifier))
    {
        // Ctrl + wheel in look mode: zoom (field of view)
        m_camera->ProcessMouseScroll(steps);
    }
    else if (steps > 0)
    {
        m_moveSpeed += (m_moveSpeed < 1.0f) ? 0.1f : 0.5f;
        m_camera->SetSpeed(m_moveSpeed);
    }
    else
    {
        m_moveSpeed -= (m_moveSpeed <= 1.0f) ? 0.1f : 0.5f;
        m_moveSpeed = std::max(m_moveSpeed, 0.1f);
        m_camera->SetSpeed(m_moveSpeed);
    }
    event->accept();
}
