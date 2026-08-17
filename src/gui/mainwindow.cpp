#include "mainwindow.h"

#include "../canvas/circuit_scene.h"
#include "../canvas/circuit_view.h"
#include "../core/probe_item.h"
#include "../core/element.h"
#include "../core/simulation_engine.h"
#include "../core/terminal.h"
#include "../core/wire.h"
#include "../io/file_manager.h"
#include "../ui/componentlibrary.h"
#include "../ui/helpdialog.h"

#include <QActionGroup>
#include <QApplication>
#include <QCoreApplication>
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
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSaveFile>
#include <QStatusBar>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>
#include <QImage>
#include <QPainter>
#include <QTextEdit>

static QTextEdit* g_logWidget = nullptr;
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (g_logWidget) {
        QString color = "#00FF00";
        if (type == QtWarningMsg) color = "#FFD700";
        else if (type == QtCriticalMsg || type == QtFatalMsg) color = "#FF3333";
        g_logWidget->append(QString("<span style='color:%1'>%2</span>").arg(color, msg));
    }
}

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

MainWindow::~MainWindow()
{
    if (g_logWidget == m_simulationLog) {
        qInstallMessageHandler(nullptr);
        g_logWidget = nullptr;
    }
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

    auto *stepBtn = new QPushButton(tr("⏭ Step"));
    auto *restartBtn = new QPushButton(tr("↻ Restart"));
    auto *run = new QPushButton(tr("▶  Run"));
    run->setCheckable(true);
    run->setObjectName("primaryButton");

    topLayout->addWidget(brand);
    topLayout->addSpacing(12);
    topLayout->addWidget(project);
    topLayout->addStretch();
    topLayout->addWidget(restartBtn);
    topLayout->addWidget(stepBtn);
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
    toolsLayout->addWidget(select);
    toolsLayout->addWidget(wire);
    toolsLayout->addWidget(probe);
    toolsLayout->addStretch();
    toolsLayout->addWidget(zoomOut);
    toolsLayout->addWidget(reset);
    toolsLayout->addWidget(zoomIn);

    m_simulationLog = new QTextEdit();
    m_simulationLog->setReadOnly(true);
    m_simulationLog->setMaximumHeight(120);
    m_simulationLog->setStyleSheet("background-color: #1E1E1E; color: #00FF00; font-family: Consolas; font-size: 11px; border-radius: 8px; padding: 5px;");
    m_simulationLog->append(">> Circuit Studio Simulation Log Initialized...");
    g_logWidget = m_simulationLog;
    qInstallMessageHandler(customMessageHandler);

    auto *workspaceWithLogLayout = new QVBoxLayout();
    workspaceWithLogLayout->setContentsMargins(0,0,0,0);
    workspaceWithLogLayout->addWidget(view, 1);
    workspaceWithLogLayout->addWidget(m_simulationLog);

    workspaceLayout->addWidget(tools);
    workspaceLayout->addLayout(workspaceWithLogLayout, 1);

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

    auto saveAsProject = [this]() {
        QString newPath = QFileDialog::getSaveFileName(this, tr("Save Project As"), "", tr("Circuit projects (*.circuit.json *.json)"));
        if (!newPath.isEmpty()) {
            m_projectPath = newPath;
            findChild<QLabel*>("projectName")->setText(QFileInfo(m_projectPath).completeBaseName());
            setWindowTitle(QFileInfo(m_projectPath).completeBaseName() + tr(" — Circuit Studio"));
            saveProject();
        }
    };

    auto exportImage = [this]() {
        QString imagePath = QFileDialog::getSaveFileName(this, tr("Export Circuit Image"), "", tr("PNG Image (*.png);;JPEG Image (*.jpg)"));
        if (imagePath.isEmpty()) return;
        scene->clearSelection();
        QRectF circuitArea = scene->itemsBoundingRect();
        if (circuitArea.isEmpty()) {
            QMessageBox::warning(this, tr("Export Failed"), tr("The canvas is empty!"));
            return;
        }
        circuitArea.adjust(-20, -20, 20, 20);
        QImage image(circuitArea.size().toSize(), QImage::Format_ARGB32);
        image.fill(Qt::white);
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter, QRectF(), circuitArea);
        painter.end();
        if (image.save(imagePath)) statusBar()->showMessage(tr("Image exported successfully!"), 3000);
        else QMessageBox::critical(this, tr("Export Failed"), tr("Could not save the image."));
    };

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newAction = fileMenu->addAction(tr("New Project..."));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newProjectRequested);
    QAction *openAction = fileMenu->addAction(tr("Open Project..."));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openProjectRequested);
    QAction *homeAction = fileMenu->addAction(tr("Back to Welcome"));
    connect(homeAction, &QAction::triggered, this, &MainWindow::homeRequested);
    fileMenu->addSeparator();
    QAction *saveAction = fileMenu->addAction(tr("Save"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveProject);
    QAction *saveAsAction = fileMenu->addAction(tr("Save As..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, saveAsProject);
    QAction *exportAction = fileMenu->addAction(tr("Export Image..."));
    connect(exportAction, &QAction::triggered, this, exportImage);
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(tr("Exit"));
    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QAction *undoAction = editMenu->addAction(tr("Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, [this] { FileManager::undo(scene); });
    QAction *redoAction = editMenu->addAction(tr("Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, [this] { FileManager::redo(scene); });
    editMenu->addSeparator();
    QAction *copyAction = editMenu->addAction(tr("Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, scene, &CircuitScene::copySelectedComponents);
    QAction *pasteAction = editMenu->addAction(tr("Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, [this] {
        scene->pasteCopiedComponents(view->mapToScene(view->viewport()->rect().center()));
    });
    QAction *selectAllAction = editMenu->addAction(tr("Select All"));
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAction, &QAction::triggered, this, [this] {
        for (QGraphicsItem *item : scene->items()) {
            if (dynamic_cast<Element *>(item)) item->setSelected(true);
        }
    });

    QSettings settings;
    scene->setGridVisible(settings.value("view/gridVisible", true).toBool());
    scene->setSnapEnabled(settings.value("view/snapEnabled", true).toBool());
    scene->setGridSize(settings.value("view/gridSize", 20).toInt());
    const QString savedGridStyle = settings.value("view/gridStyle", "dots").toString();
    scene->setGridStyle(savedGridStyle == "lines" ? CircuitScene::GridStyle::Lines
                                                   : CircuitScene::GridStyle::Dots);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    QAction *showGridAction = viewMenu->addAction(tr("Show Grid"));
    showGridAction->setCheckable(true);
    showGridAction->setChecked(scene->isGridVisible());
    showGridAction->setShortcut(Qt::Key_G);
    connect(showGridAction, &QAction::toggled, this, [this](bool visible) {
        scene->setGridVisible(visible);
        QSettings().setValue("view/gridVisible", visible);
    });

    QAction *snapAction = viewMenu->addAction(tr("Snap to Grid"));
    snapAction->setCheckable(true);
    snapAction->setChecked(scene->isSnapEnabled());
    connect(snapAction, &QAction::toggled, this, [this](bool enabled) {
        scene->setSnapEnabled(enabled);
        QSettings().setValue("view/snapEnabled", enabled);
    });

    QMenu *gridStyleMenu = viewMenu->addMenu(tr("Grid Style"));
    QActionGroup *gridStyleGroup = new QActionGroup(this);
    QAction *dotsAction = gridStyleMenu->addAction(tr("Dots"));
    QAction *linesAction = gridStyleMenu->addAction(tr("Lines"));
    for (QAction *action : {dotsAction, linesAction}) {
        action->setCheckable(true);
        gridStyleGroup->addAction(action);
    }
    dotsAction->setChecked(scene->gridStyle() == CircuitScene::GridStyle::Dots);
    linesAction->setChecked(scene->gridStyle() == CircuitScene::GridStyle::Lines);
    connect(dotsAction, &QAction::triggered, this, [this] {
        scene->setGridStyle(CircuitScene::GridStyle::Dots);
        QSettings().setValue("view/gridStyle", "dots");
    });
    connect(linesAction, &QAction::triggered, this, [this] {
        scene->setGridStyle(CircuitScene::GridStyle::Lines);
        QSettings().setValue("view/gridStyle", "lines");
    });

    QMenu *gridSizeMenu = viewMenu->addMenu(tr("Grid Size"));
    QActionGroup *gridSizeGroup = new QActionGroup(this);
    for (int size : {10, 20, 40}) {
        QAction *sizeAction = gridSizeMenu->addAction(tr("%1 px").arg(size));
        sizeAction->setCheckable(true);
        sizeAction->setChecked(scene->gridSpacing() == size);
        gridSizeGroup->addAction(sizeAction);
        connect(sizeAction, &QAction::triggered, this, [this, size] {
            scene->setGridSize(size);
            QSettings().setValue("view/gridSize", size);
        });
    }

    viewMenu->addSeparator();
    QAction *showLibraryAction = viewMenu->addAction(tr("Component Library"));
    showLibraryAction->setCheckable(true);
    showLibraryAction->setChecked(true);
    connect(showLibraryAction, &QAction::toggled, library, &QWidget::setVisible);
    QAction *showLogAction = viewMenu->addAction(tr("Simulation Log"));
    showLogAction->setCheckable(true);
    showLogAction->setChecked(true);
    connect(showLogAction, &QAction::toggled, m_simulationLog, &QWidget::setVisible);
    viewMenu->addSeparator();
    QAction *zoomInAction = viewMenu->addAction(tr("Zoom In"));
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, view, &CircuitView::zoomIn);
    QAction *zoomOutAction = viewMenu->addAction(tr("Zoom Out"));
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, view, &CircuitView::zoomOut);
    QAction *fitAction = viewMenu->addAction(tr("Fit Canvas"));
    connect(fitAction, &QAction::triggered, view, &CircuitView::fitCanvas);

    QMenu *simulationMenu = menuBar()->addMenu(tr("&Simulation"));
    QAction *runAction = simulationMenu->addAction(tr("Run / Pause"));
    runAction->setShortcut(Qt::Key_F5);
    connect(runAction, &QAction::triggered, run, &QPushButton::click);
    QAction *stepAction = simulationMenu->addAction(tr("Single Step"));
    stepAction->setShortcut(Qt::Key_F10);
    connect(stepAction, &QAction::triggered, stepBtn, &QPushButton::click);
    QAction *restartAction = simulationMenu->addAction(tr("Restart"));
    restartAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F5));
    connect(restartAction, &QAction::triggered, restartBtn, &QPushButton::click);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    auto showHelp = [this](HelpDialog::Topic topic) {
        if (!m_helpDialog) {
            m_helpDialog = new HelpDialog(topic, this);
            connect(m_helpDialog, &QObject::destroyed, this, [this] {
                m_helpDialog = nullptr;
            });
        } else {
            m_helpDialog->selectTopic(topic);
        }
        m_helpDialog->show();
        m_helpDialog->raise();
        m_helpDialog->activateWindow();
    };
    QAction *helpCenterAction = helpMenu->addAction(tr("Help Center..."));
    helpCenterAction->setShortcut(Qt::Key_F1);
    connect(helpCenterAction, &QAction::triggered, this, [showHelp] {
        showHelp(HelpDialog::Overview);
    });
    QAction *shortcutsAction = helpMenu->addAction(tr("Keyboard Shortcuts"));
    connect(shortcutsAction, &QAction::triggered, this, [showHelp] {
        showHelp(HelpDialog::Shortcuts);
    });
    helpMenu->addSeparator();
    QAction *aboutAction = helpMenu->addAction(tr("About Circuit Studio"));
    connect(aboutAction, &QAction::triggered, this, [this] {
        QMessageBox::about(this, tr("About Circuit Studio"),
                           tr("Circuit Studio\n"
                              "OOP Circuit Design and Simulation Project\n\n"
                              "Team roles:\n"
                              "Shahab Javaheri — UX design and project integration\n"
                              "Yasin Taati — Backend and simulation engine\n"
                              "Mohammad Javad Zabihi — Frontend and wiring system"));
    });

    connect(wire, &QPushButton::clicked, this, [this]() { scene->setWiringMode(true); });
    connect(select, &QPushButton::clicked, this, [this]() { scene->setWiringMode(false); });

    // اجرای یک گام شبیه‌سازی
    connect(stepBtn, &QPushButton::clicked, this, [this, run]() {
        if (run->isChecked()) {
            run->setChecked(false);
            m_simulationTimer->stop();
            run->setText(tr("▶  Run"));
        }
        m_engine->stepSimulation();
        scene->update();
        m_simulationLog->append(tr(">> Step execution completed."));
    });

    connect(restartBtn, &QPushButton::clicked, this, [this, run]() {
        run->setChecked(false);
        m_simulationTimer->stop();
        run->setText(tr("▶  Run"));
        m_engine->resetSimulation();
        m_simulationLog->append(tr(">> Simulation restarted."));
        statusBar()->showMessage(tr("Simulation restarted"), 2500);
    });

    connect(zoomIn, &QPushButton::clicked, view, &CircuitView::zoomIn);
    connect(zoomOut, &QPushButton::clicked, view, &CircuitView::zoomOut);
    connect(reset, &QPushButton::clicked, view, &CircuitView::fitCanvas);
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
        if (scene->isProbeEnabled && scene->voltageProbe) {
            bool found = false;
            for (QGraphicsItem *item : scene->items(point)) {
                if (auto *wire = dynamic_cast<Wire*>(item)) {
                    scene->voltageProbe->updateProbe(wire->voltageLevel, point);
                    found = true; break;
                }
                if (auto *terminal = dynamic_cast<Terminal*>(item)) {
                    scene->voltageProbe->updateProbe(terminal->voltageLevel, point);
                    found = true; break;
                }
            }
            if (!found) scene->voltageProbe->hide();
        }
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
        scene->update();
        if (isActiveWindow() && view->underMouse() && scene->isProbeEnabled && scene->voltageProbe) {
            const QPoint viewPos = view->mapFromGlobal(QCursor::pos());
            const QPointF scenePos = view->mapToScene(viewPos);
            bool found = false;
            for (QGraphicsItem *item : scene->items(scenePos)) {
                if (auto *wire = dynamic_cast<Wire *>(item)) {
                    scene->voltageProbe->updateProbe(wire->voltageLevel, scenePos);
                    found = true; break;
                }
                if (auto *terminal = dynamic_cast<Terminal *>(item)) {
                    scene->voltageProbe->updateProbe(terminal->voltageLevel, scenePos);
                    found = true; break;
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
    statusBar()->showMessage(tr("Project saved"), 2500);
}

void MainWindow::applyTheme()
{
    setStyleSheet(R"(
        QMainWindow, QWidget { background: #EEF1F5; color: #1D2530; }
        QMenuBar {
            background: #FFFFFF; color: #253143; border-bottom: 1px solid #DDE2E8;
            padding: 3px 8px;
        }
        QMenuBar::item { padding: 6px 10px; border-radius: 6px; }
        QMenuBar::item:selected { background: #E8F2FF; color: #0A66D3; }
        QMenu { background: #FFFFFF; color: #253143; border: 1px solid #DDE2E8; padding: 5px; }
        QMenu::item { padding: 7px 28px 7px 22px; border-radius: 5px; }
        QMenu::item:selected { background: #E8F2FF; color: #0A66D3; }
        QMenu::separator { height: 1px; background: #E6E9ED; margin: 5px 8px; }
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
