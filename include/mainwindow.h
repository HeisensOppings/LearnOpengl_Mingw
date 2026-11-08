#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include "glwidget.h"

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QPushButton;
class QSlider;
class QTimer;

// Qt replacement for the original ImGui "LearnOpenGL" panel.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(GLWidget::SceneKind kind, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QWidget *createControlPanel();
    void populateAnimations();
    void refreshUi();

    GLWidget *m_view = nullptr;
    QTimer *m_uiTimer = nullptr;

    QLabel *m_posLabel = nullptr;
    QLabel *m_fpsLabel = nullptr;
    QComboBox *m_animCombo = nullptr;
    QSlider *m_frameSlider = nullptr;
    QPushButton *m_playButton = nullptr;
};

#endif
