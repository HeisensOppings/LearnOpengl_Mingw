#include "mainwindow.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QColorDialog>
#include <QComboBox>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

#include <functional>

#include "appcontrol.h"
#include "camera.h"

namespace
{
const int kFrameSliderSteps = 1000;

QDoubleSpinBox *makeSpin(double min, double max, double step, double value, std::function<void(double)> onChange)
{
    auto *spin = new QDoubleSpinBox;
    spin->setRange(min, max);
    spin->setSingleStep(step);
    spin->setDecimals(3);
    spin->setValue(value);
    QObject::connect(spin, &QDoubleSpinBox::valueChanged, spin, onChange);
    return spin;
}

// three spin boxes editing a glm::vec3 in place
QWidget *makeVec3Editor(glm::vec3 *target, double min, double max, double step)
{
    auto *row = new QWidget;
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < 3; ++i)
        layout->addWidget(makeSpin(min, max, step, (*target)[i], [target, i](double v) { (*target)[i] = float(v); }));
    return row;
}

QColor toQColor(const glm::vec3 &c)
{
    return QColor::fromRgbF(c.r, c.g, c.b);
}

void setButtonColor(QPushButton *button, const glm::vec3 &c)
{
    button->setStyleSheet(QStringLiteral("background-color: %1;").arg(toQColor(c).name()));
}

// colour button editing a glm::vec3 through QColorDialog
QPushButton *makeColorButton(QWidget *parent, glm::vec3 *target, const QString &title)
{
    auto *button = new QPushButton(QObject::tr("Pick..."));
    setButtonColor(button, *target);
    QObject::connect(button, &QPushButton::clicked, button, [parent, button, target, title]() {
        const QColor color = QColorDialog::getColor(toQColor(*target), parent, title);
        if (!color.isValid())
            return;
        *target = glm::vec3(color.redF(), color.greenF(), color.blueF());
        setButtonColor(button, *target);
    });
    return button;
}

// ImGui::CollapsingHeader equivalent
QGroupBox *makeSection(const QString &title, bool expanded, QLayout *content)
{
    auto *box = new QGroupBox(title);
    box->setCheckable(true);
    box->setChecked(expanded);
    auto *inner = new QWidget;
    inner->setLayout(content);
    auto *outer = new QVBoxLayout(box);
    outer->addWidget(inner);
    inner->setVisible(expanded);
    QObject::connect(box, &QGroupBox::toggled, inner, &QWidget::setVisible);
    return box;
}
} // namespace

MainWindow::MainWindow(GLWidget::SceneKind kind, QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("mashiro"));

    m_view = new GLWidget(kind, this);
    setCentralWidget(m_view);

    auto *dock = new QDockWidget(tr("LearnOpenGL"), this);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setWidget(createControlPanel());
    dock->setWidget(scroll);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    resize(1600, 900);
    QPoint savedPos;
    if (m_view->savedWindowPos(savedPos))
        move(savedPos);

    connect(m_view, &GLWidget::sceneReady, this, &MainWindow::populateAnimations);
    connect(m_view, &GLWidget::frameStats, this, [this](double ms, double fps) {
        m_fpsLabel->setText(QStringLiteral("(%1 ms)(%2 fps)").arg(ms, 0, 'f', 3).arg(fps, 0, 'f', 1));
    });
    connect(m_view, &GLWidget::fatalError, this, [this](const QString &message) {
        QMessageBox::critical(this, tr("OpenGL error"), message);
        close();
    });

    m_uiTimer = new QTimer(this);
    connect(m_uiTimer, &QTimer::timeout, this, &MainWindow::refreshUi);
    m_uiTimer->start(50);
    m_view->setFocus();
}

QWidget *MainWindow::createControlPanel()
{
    auto *panel = new QWidget;
    auto *layout = new QVBoxLayout(panel);

    // ---------------------------------------- camera
    m_posLabel = new QLabel(QStringLiteral("Pos: -"));
    auto *resetButton = new QPushButton(tr("Pos_Reset"));
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        if (m_view->camera())
            m_view->camera()->m_cameraPos = glm::vec3(0.0f);
    });
    m_fpsLabel = new QLabel(QStringLiteral("(- ms)(- fps)"));
    layout->addWidget(m_posLabel);
    layout->addWidget(resetButton);
    layout->addWidget(m_fpsLabel);
    layout->addWidget(new QLabel(tr("Right click: toggle mouse look\nWASD / Space / Shift: move\n"
                                    "Wheel: speed, Ctrl+Wheel (look mode): zoom\n"
                                    "Cascaded scene: N debug layer, C freeze frustum")));

    // ---------------------------------------- background (solid colour, replaces the HDR skybox)
    {
        auto *form = new QFormLayout;
        form->addRow(tr("Color"), makeColorButton(this, &App::background_color, tr("Background color")));
        layout->addWidget(makeSection(tr("Background"), false, form));
    }
    // ---------------------------------------- light
    {
        auto *form = new QFormLayout;
        form->addRow(tr("PointLightPos"), makeVec3Editor(&App::pointLightPos, 0.0, 4.0, 0.05));
        form->addRow(tr("PointLightColor"), makeVec3Editor(&App::pointLightColor, 0.0, 1.0, 0.01));
        layout->addWidget(makeSection(tr("Light"), false, form));
    }
    // ---------------------------------------- pbr
    {
        auto *form = new QFormLayout;
        form->addRow(tr("point"), makeColorButton(this, &App::irradiance_color, tr("PBR light color")));
        layout->addWidget(makeSection(tr("PBR"), false, form));
    }
    // ---------------------------------------- animation
    {
        auto *form = new QFormLayout;
        m_animCombo = new QComboBox;
        connect(m_animCombo, &QComboBox::currentIndexChanged, this, [](int index) {
            if (index >= 0 && index < int(App::animNames.size()))
                App::animIndex = index;
        });
        form->addRow(tr("Animations"), m_animCombo);

        m_frameSlider = new QSlider(Qt::Horizontal);
        m_frameSlider->setRange(0, kFrameSliderSteps);
        connect(m_frameSlider, &QSlider::valueChanged, this, [](int value) {
            App::currentFrame = App::duration * float(value) / kFrameSliderSteps;
        });
        form->addRow(tr("PerFrame"), m_frameSlider);

        m_playButton = new QPushButton(tr("Play"));
        connect(m_playButton, &QPushButton::clicked, this, [this]() {
            App::playBackState = !App::playBackState;
            m_playButton->setText(App::playBackState ? tr("Pause") : tr("Play"));
        });
        auto *speed = makeSpin(0.1, 2.0, 0.1, App::playSpeed, [](double v) { App::playSpeed = float(v); });
        auto *row = new QWidget;
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->addWidget(m_playButton);
        rowLayout->addWidget(new QLabel(tr("PlaySpeed")));
        rowLayout->addWidget(speed);
        form->addRow(row);
        layout->addWidget(makeSection(tr("Animation"), true, form));
    }
    // ---------------------------------------- cascaded shadow map
    {
        auto *form = new QFormLayout;
        form->addRow(tr("bias_offs"), makeSpin(0.1, 2.0, 0.05, App::bias_offs, [](double v) { App::bias_offs = float(v); }));
        form->addRow(tr("bias_mids"), makeSpin(1.0, 25.0, 0.5, App::bias_mids, [](double v) { App::bias_mids = float(v); }));
        layout->addWidget(makeSection(tr("Cascaded CSM"), false, form));
    }
    layout->addStretch();
    return panel;
}

void MainWindow::populateAnimations()
{
    QSignalBlocker blocker(m_animCombo);
    m_animCombo->clear();
    for (const auto &name : App::animNames)
        m_animCombo->addItem(QString::fromStdString(name));
    if (App::animIndex >= 0 && App::animIndex < m_animCombo->count())
        m_animCombo->setCurrentIndex(App::animIndex);
}

void MainWindow::refreshUi()
{
    if (Camera *cam = m_view->camera())
    {
        m_posLabel->setText(QStringLiteral("Pos: %1,%2,%3")
                                .arg(cam->m_cameraPos.x, 0, 'f', 2)
                                .arg(cam->m_cameraPos.y, 0, 'f', 2)
                                .arg(cam->m_cameraPos.z, 0, 'f', 2));
    }
    // follow the animation time while playing (ImGui slider was bound to App::currentFrame)
    if (App::playBackState && !m_frameSlider->isSliderDown() && App::duration > 0.0f)
    {
        QSignalBlocker blocker(m_frameSlider);
        m_frameSlider->setValue(int(kFrameSliderSteps * App::currentFrame / App::duration));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_view->saveCameraState(pos());
    QMainWindow::closeEvent(event);
}
