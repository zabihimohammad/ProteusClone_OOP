#pragma once
#include <QObject>
#include "../canvas/circuit_scene.h"

class SimulationEngine : public QObject {
Q_OBJECT
public:
    explicit SimulationEngine(CircuitScene *scene, QObject *parent = nullptr);

    // اجرای یک گام کامل از محاسبات مدار
    void stepSimulation();

private:
    CircuitScene *m_scene;
    void propagateVoltages();
};