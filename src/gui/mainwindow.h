#pragma once

#include <QMainWindow>
#include <QSize>
#include <QTextEdit>

class CircuitScene;
class CircuitView;
class QLabel;
class QTimer;
class SimulationEngine;
class HelpDialog;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QSize &canvasSize = QSize(1600, 1000),
                        const QString &projectPath = {}, QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    void newProjectRequested();
    void openProjectRequested();
    void homeRequested();

private:
    void buildInterface();
    void applyTheme();
    void configureSimulation();
    void saveProject();

    CircuitScene *scene = nullptr;
    CircuitView *view = nullptr;
    SimulationEngine *m_engine = nullptr;
    QTimer *m_simulationTimer = nullptr;
    QLabel *m_coordinates = nullptr;
    QLabel *m_zoomLabel = nullptr;
    QTextEdit *m_simulationLog = nullptr;
    HelpDialog *m_helpDialog = nullptr;
    QSize m_canvasSize;
    QString m_projectPath;
};
