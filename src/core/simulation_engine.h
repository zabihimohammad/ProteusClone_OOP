#pragma once
#include <QObject>
#include <QMap>
#include "../canvas/circuit_scene.h"

class SimulationEngine : public QObject {
Q_OBJECT
public:
    explicit SimulationEngine(CircuitScene *scene, QObject *parent = nullptr);

    // اجرای یک گام کامل از محاسبات مدار
    void stepSimulation();

private:
    CircuitScene *m_scene;

    // 🛠️ حافظه برای ذخیره ولتاژ/جریان قبلی خازن‌ها و سلف‌ها
    QMap<Element*, double> historicalState;

    void propagateVoltages(); // این متد حالا تحلیل گره (Nodal Analysis) را انجام می‌دهد
};