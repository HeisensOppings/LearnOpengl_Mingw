#include <QApplication>
#include <QSurfaceFormat>

#include <iostream>
#include <string>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // OpenGL 4.2 core profile with 4x MSAA (same as the original GLFW window hints).
    // Must be set before the QApplication is created.
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(4, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    GLWidget::SceneKind kind = GLWidget::SceneKind::Default;
    const QStringList args = app.arguments();
    if (args.size() < 2)
    {
        std::cout << "Usage: opengl [-cascaded | -animations]" << std::endl;
    }
    else if (args.at(1) == QLatin1String("-cascaded"))
    {
        kind = GLWidget::SceneKind::CascadedShadowMap;
    }
    else if (args.at(1) == QLatin1String("-animations"))
    {
        kind = GLWidget::SceneKind::Animations;
    }
    else
    {
        std::cout << "Usage: opengl [-cascaded | -animations]" << std::endl;
    }

    MainWindow window(kind);
    window.show();
    return app.exec();
}
