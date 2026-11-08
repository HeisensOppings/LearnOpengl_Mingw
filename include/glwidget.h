#ifndef __GLWIDGET_H__
#define __GLWIDGET_H__

#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QPoint>
#include <QSet>
#include <QString>

class Camera;
class Scene;

// OpenGL 4.2 core viewport. Replaces the GLFW window, the glad loader and the
// GLFW input callbacks of the original project.
class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    enum class SceneKind
    {
        Default,
        CascadedShadowMap,
        Animations
    };

    explicit GLWidget(SceneKind kind, QWidget *parent = nullptr);
    ~GLWidget() override;

    Camera *camera() const { return m_camera; }
    bool isInitialized() const { return m_scene != nullptr; }
    // camera / window position persisted in res/config.ini
    bool savedWindowPos(QPoint &pos) const;
    void saveCameraState(const QPoint &windowPos);

signals:
    void sceneReady();
    void frameStats(double frameMs, double fps);
    void fatalError(const QString &message);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void cleanup();
    void processHeldKeys(float dt);
    void setMouseLook(bool enabled);

    SceneKind m_kind;
    Camera *m_camera = nullptr;
    Scene *m_scene = nullptr;
    bool m_hasSavedPos = false;
    QPoint m_savedPos;

    QElapsedTimer m_clock;
    float m_lastFrame = 0.0f;
    int m_statFrames = 0;
    float m_statTime = 0.0f;

    QSet<int> m_heldKeys;
    bool m_mouseLook = false;  // right-click toggles the original "camera look" mode
    bool m_skipNextMove = true;
    float m_moveSpeed = 2.0f;
};

#endif
