#include "simulation_engine.h"
#include "wire.h"
#include "terminal.h"
#include "element.h"

SimulationEngine::SimulationEngine(CircuitScene *scene, QObject *parent)
        : QObject(parent), m_scene(scene) {}

void SimulationEngine::stepSimulation() {
    if (!m_scene) return;

    // ۱. اجرای منطق تمام قطعات مدار (فراخوانی توابعی که هم‌گروهی شما می‌نویسد)
    QList<QGraphicsItem*> allItems = m_scene->items();
    for (QGraphicsItem *item : allItems) {
        Element *element = dynamic_cast<Element*>(item);
        if (element) {
            element->process(); // قطعه ورودی‌ها را بررسی کرده و ولتاژ پین خروجی‌اش را ست می‌کند
        }
    }

    // ۲. پخش کردن ولتاژهای خروجی در طول سیم‌ها و رساندن آن‌ها به پین‌های ورودی قطعات بعدی
    propagateVoltages();
}

void SimulationEngine::propagateVoltages() {
    QList<QGraphicsItem*> allItems = m_scene->items();

    // حرکت روی تمام سیم‌های بوم
    for (QGraphicsItem *item : allItems) {
        Wire *wire = dynamic_cast<Wire*>(item);
        if (!wire) continue;

        // استفاده از گترهای جدیدی که در گام اول ساختیم
        Terminal *startTerm = wire->getStartTerminal();
        Terminal *endTerm = wire->getEndTerminal();

        QString targetVoltage = "Undefined";

        // قانون انتشار ولتاژ: اگر پین ابتدا یا انتها ولتاژ معتبری داشته باشد، کل سیم آن ولتاژ را می‌گیرد
        if (startTerm && startTerm->voltageLevel != "Undefined") {
            targetVoltage = startTerm->voltageLevel;
        } else if (endTerm && endTerm->voltageLevel != "Undefined") {
            targetVoltage = endTerm->voltageLevel;
        }

        // به‌روزرسانی ولتاژ سیم
        wire->voltageLevel = targetVoltage;

        // انتقال ولتاژ به پین‌های متصل جهت یکپارچگی مدار
        if (startTerm) startTerm->voltageLevel = targetVoltage;
        if (endTerm) endTerm->voltageLevel = targetVoltage;
    }
}