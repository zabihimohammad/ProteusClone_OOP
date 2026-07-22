#include "mainwindow.h"

#include "../canvas/circuit_scene.h"
#include "../canvas/circuit_view.h"
#include "../core/probe_item.h"
#include "../core/simulation_engine.h"
#include "../core/terminal.h"
#include "../core/wire.h"
#include "../io/file_manager.h"
#include "../ui/componentlibrary.h"

#include <QCheckBox>
#include <QComboBox>
#include <QCursor>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

MainWindow::MainWindow(const QSize &canvasSize, const QString &projectPath, QWidget *parent)
    : QMainWindow(parent), m_canvasSize(canvasSize), m_projectPath(projectPath)
{
    resize(1280, 780);
    setMinimumSize(900, 600);
    const QString title = projectPath.isEmpty() ? tr("Untitled") : QFileInfo(projectPath).completeBaseName();
    setWindowTitle(title + tr(" — Circuit Studio"));
    buildInterface();
    applyTheme();
    configureSimulation();
}

void MainWindow::buildInterface()
{
    scene = new CircuitScene(this);
    scene->setCanvasSize(m_canvasSize);
    view = new CircuitView;
    view->setScene(scene);

    if (!m_projectPath.isEmpty() && QFileInfo::exists(m_projectPath)) {
        FileManager::loadCircuit(m_projectPath, scene);
        scene->voltageProbe = new ProbeItem();
        scene->addItem(scene->voltageProbe);
        scene->voltageProbe->hide();
    }

    auto *root = new QWidget;
    auto *rootLayout = new QVBoxLayout(root);
    rootLayout->setContentsMargins(14, 10, 14, 12);
    rootLayout->setSpacing(10);

    auto *top = new QFrame;
    top->setObjectName("topBar");
    auto *topLayout = new QHBoxLayout(top);
    topLayout->setContentsMargins(14, 9, 14, 9);
    auto *brand = new QLabel(tr("◉  Circuit Studio"));
    brand->setObjectName("brand");
    auto *project = new QLabel(QFileInfo(m_projectPath).completeBaseName());
    if (project->text().isEmpty()) project->setText(tr("Untitled circuit"));
    project->setObjectName("projectName");
    auto *save = new QPushButton(tr("Save"));
    auto *undo = new QPushButton(tr("↶"));
    auto *redo = new QPushButton(tr("↷"));
    auto *run = new QPushButton(tr("▶  Run"));
    run->setCheckable(true);
    run->setObjectName("primaryButton");
    topLayout->addWidget(brand);
    topLayout->addSpacing(12);
    topLayout->addWidget(project);
    topLayout->addStretch();
    topLayout->addWidget(save);
    topLayout->addWidget(undo);
    topLayout->addWidget(redo);
    topLayout->addSpacing(8);
    topLayout->addWidget(run);

    auto *content = new QWidget;
    auto *contentLayout = new QHBoxLayout(content);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(10);
    auto *library = new ComponentLibrary;

    auto *workspace = new QFrame;
    workspace->setObjectName("workspace");
    auto *workspaceLayout = new QVBoxLayout(workspace);
    workspaceLayout->setContentsMargins(0, 0, 0, 0);
    workspaceLayout->setSpacing(0);
    auto *tools = new QFrame;
    tools->setObjectName("canvasTools");
    auto *toolsLayout = new QHBoxLayout(tools);
    toolsLayout->setContentsMargins(12, 8, 12, 8);
    auto *select = new QPushButton(tr("⌁  Select"));
    select->setObjectName("selectedTool");
    auto *wire = new QPushButton(tr("⌁  Wire"));
    auto *probe = new QPushButton(tr("Probe"));
    probe->setCheckable(true);
    auto *zoomOut = new QPushButton(tr("−"));
    auto *zoomIn = new QPushButton(tr("+"));
    auto *reset = new QPushButton(tr("Fit"));
    auto *gridSize = new QComboBox;
    gridSize->addItems({tr("Grid 10"), tr("Grid 20"), tr("Grid 40")});
    gridSize->setCurrentIndex(1);
    auto *snap = new QCheckBox(tr("Snap"));
    snap->setChecked(true);
    toolsLayout->addWidget(select);
    toolsLayout->addWidget(wire);
    toolsLayout->addWidget(probe);
    toolsLayout->addStretch();
    toolsLayout->addWidget(gridSize);
    toolsLayout->addWidget(snap);
    toolsLayout->addSpacing(8);
    toolsLayout->addWidget(zoomOut);
    toolsLayout->addWidget(reset);
    toolsLayout->addWidget(zoomIn);

    workspaceLayout->addWidget(tools);
    workspaceLayout->addWidget(view, 1);
    contentLayout->addWidget(library);
    contentLayout->addWidget(workspace, 1);
    rootLayout->addWidget(top);
    rootLayout->addWidget(content, 1);
    setCentralWidget(root);

    m_coordinates = new QLabel(tr("X 0   Y 0"));
    m_zoomLabel = new QLabel(tr("Zoom 100%"));
    statusBar()->addWidget(new QLabel(tr("● Ready")));
    statusBar()->addPermanentWidget(m_coordinates);
    statusBar()->addPermanentWidget(m_zoomLabel);

    connect(save, &QPushButton::clicked, this, &MainWindow::saveProject);
    connect(undo, &QPushButton::clicked, this, [this] { FileManager::undo(scene); });
    connect(redo, &QPushButton::clicked, this, [this] { FileManager::redo(scene); });
    connect(zoomIn, &QPushButton::clicked, view, &CircuitView::zoomIn);
    connect(zoomOut, &QPushButton::clicked, view, &CircuitView::zoomOut);
    connect(reset, &QPushButton::clicked, view, &CircuitView::fitCanvas);
    connect(snap, &QCheckBox::toggled, scene, &CircuitScene::setSnapEnabled);
    connect(gridSize, qOverload<int>(&QComboBox::currentIndexChanged), this, [scene = scene, gridSize] {
        scene->setGridSize(gridSize->currentText().section(' ', 1).toInt());
    });
    connect(probe, &QPushButton::toggled, this, [this](bool checked) {
        scene->isProbeEnabled = checked;
        if (!checked && scene->voltageProbe) scene->voltageProbe->hide();
    });
    connect(run, &QPushButton::toggled, this, [this, run](bool running) {
        if (running) {
            m_simulationTimer->start(100);
            run->setText(tr("Ⅱ  Pause"));
        } else {
            m_simulationTimer->stop();
            run->setText(tr("▶  Run"));
        }
    });
    connect(view, &CircuitView::cursorPositionChanged, this, [this](const QPointF &point) {
        m_coordinates->setText(tr("X %1   Y %2").arg(qRound(point.x())).arg(qRound(point.y())));
    });
    connect(view, &CircuitView::zoomChanged, this, [this](int percent) {
        m_zoomLabel->setText(tr("Zoom %1%").arg(percent));
    });
    connect(library, &ComponentLibrary::componentActivated, this, [this](const QString &id) {
        scene->addComponent(id, view->mapToScene(view->viewport()->rect().center()));
        statusBar()->showMessage(tr("Component added to the canvas"), 2500);
    });
    QTimer::singleShot(0, view, &CircuitView::fitCanvas);
}

void MainWindow::configureSimulation()
{
    m_engine = new SimulationEngine(scene, this);
    m_simulationTimer = new QTimer(this);
    connect(m_simulationTimer, &QTimer::timeout, this, [this] {
        m_engine->stepSimulation();
        if (isActiveWindow() && view->underMouse() && scene->isProbeEnabled && scene->voltageProbe) {
            const QPoint viewPos = view->mapFromGlobal(QCursor::pos());
            const QPointF scenePos = view->mapToScene(viewPos);
            bool found = false;
            for (QGraphicsItem *item : scene->items(scenePos)) {
                if (auto *wire = dynamic_cast<Wire *>(item)) {
                    scene->voltageProbe->updateProbe(wire->voltageLevel, scenePos);
                    found = true;
                    break;
                }
                if (auto *terminal = dynamic_cast<Terminal *>(item)) {
                    scene->voltageProbe->updateProbe(terminal->voltageLevel, scenePos);
                    found = true;
                    break;
                }
            }
            if (!found) scene->voltageProbe->hide();
        } else if (scene->voltageProbe) {
            scene->voltageProbe->hide();
        }
    });
}

void MainWindow::saveProject()
{
    if (m_projectPath.isEmpty()) {
        m_projectPath = QFileDialog::getSaveFileName(
            this, tr("Save circuit"), {}, tr("Circuit projects (*.circuit.json *.json)"));
    }
    if (m_projectPath.isEmpty()) return;
    if (!FileManager::saveCircuit(m_projectPath, scene)) {
        QMessageBox::critical(this, tr("Save failed"), tr("The project could not be saved."));
        return;
    }

    QFile source(m_projectPath);
    if (source.open(QIODevice::ReadOnly)) {
        QJsonObject root = QJsonDocument::fromJson(source.readAll()).object();
        source.close();
        root["name"] = QFileInfo(m_projectPath).completeBaseName().remove(".circuit");
        root["canvas"] = QJsonObject{{"width", m_canvasSize.width()},
                                     {"height", m_canvasSize.height()}};
        QSaveFile target(m_projectPath);
        if (target.open(QIODevice::WriteOnly)) {
            target.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
            target.commit();
        }
    }
    statusBar()->showMessage(tr("Project saved"), 2500);
}

void MainWindow::applyTheme()
{
    setStyleSheet(R"(
        QMainWindow, QWidget { background: #EEF1F5; color: #1D2530; }
        QLabel, QCheckBox { color: #354152; }
        QFrame#topBar, QFrame#libraryPanel, QFrame#workspace {
            background: #FFFFFF; border: 1px solid #DDE2E8; border-radius: 15px;
        }
        QFrame#workspace { border-radius: 14px; }
        QFrame#canvasTools { background: #FFFFFF; border: none; border-bottom: 1px solid #E6E9ED; }
        QLabel#brand { font-size: 17px; font-weight: 700; color: #1473E6; }
        QLabel#projectName { color: #7A8491; }
        QLabel#panelTitle { font-size: 18px; font-weight: 700; }
        QLabel#sectionTitle { font-size: 12px; font-weight: 700; color: #667180; }
        QLabel#symbolPreview { font-family: Consolas; font-size: 23px; color: #1473E6; min-height: 42px; }
        QLabel#previewTitle { font-weight: 700; }
        QLabel#emptyState { color: #8C96A3; padding: 18px; }
        QFrame#previewCard { background: #F5F8FC; border: 1px solid #E1E7EF; border-radius: 12px; }
        QLineEdit, QComboBox {
            color: #1D2530; background: #F3F5F8; border: 1px solid transparent;
            border-radius: 10px; padding: 8px 11px; selection-background-color: #1473E6;
        }
        QLineEdit:focus, QComboBox:focus { border-color: #1473E6; background: white; }
        QTreeWidget, QListWidget { border: none; background: transparent; outline: none; }
        QTreeWidget::item { padding: 6px; border-radius: 7px; }
        QTreeWidget::item:selected { background: #E8F2FF; color: #0A66D3; }
        QPushButton {
            color: #253143; background: #F1F3F6; border: none; border-radius: 9px;
            padding: 8px 12px; font-weight: 600;
        }
        QPushButton:hover { background: #E5E9EF; }
        QPushButton#primaryButton { background: #1473E6; color: white; padding-left: 16px; padding-right: 16px; }
        QPushButton#selectedTool { background: #E8F2FF; color: #0A66D3; }
        QWidget#activeRow { background: #F5F8FC; border-radius: 8px; }
        QPushButton#removeComponent {
            color: #D64545; background: #FCEDEE; border: none; border-radius: 7px;
            padding: 0; font-size: 17px; font-weight: 700;
        }
        QPushButton#removeComponent:hover { color: white; background: #D64545; }
        QStatusBar { background: #FFFFFF; border-top: 1px solid #DDE2E8; }
        QStatusBar QLabel { background: transparent; padding: 2px 10px; color: #667180; }
    )");
}
