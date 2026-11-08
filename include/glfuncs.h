#ifndef __GLFUNCS_H__
#define __GLFUNCS_H__

// Qt6 OpenGL function access.
//
// The renderer targets an OpenGL 4.2 core profile context created by
// QOpenGLWidget (see glwidget.h). All GL entry points are resolved by Qt
// through QOpenGLFunctions_4_2_Core, replacing glad/GLFW.
//
// Every GL call in the engine goes through glf(), which returns the function
// table of the *current* context. GL resources must therefore only be created
// or destroyed while the GL widget's context is current (initializeGL /
// paintGL / resizeGL, or after QOpenGLWidget::makeCurrent()).

#include <QOpenGLContext>
#include <QOpenGLFunctions_4_2_Core>
#include <QOpenGLVersionFunctionsFactory>

inline QOpenGLFunctions_4_2_Core *glf()
{
    // cache the function table per thread for the current context
    thread_local QOpenGLContext *cachedCtx = nullptr;
    thread_local QOpenGLFunctions_4_2_Core *cachedFuncs = nullptr;
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    Q_ASSERT_X(ctx, "glf()", "no current OpenGL context");
    if (ctx != cachedCtx)
    {
        cachedCtx = ctx;
        cachedFuncs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_2_Core>(ctx);
        if (cachedFuncs)
            cachedFuncs->initializeOpenGLFunctions();
    }
    return cachedFuncs;
}

#endif
