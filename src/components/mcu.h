#pragma once
#include "../core/element.h"
#include "../core/terminal.h"

class MCUChip : public Element {
private:
    QString hexFilePath = "Not Loaded";
    QString clockFrequency = "8MHz";
    int PC = 0; // Program Counter
    int accumulator = 0; // ثبات اصلی
    QMap<int, int> RAM; // حافظه داخلی
    QStringList firmwareInstructions; // کدهای لود شده

    QMap<int, uint8_t> rom; // 🛠️ حافظه فلش میکروکنترلر
    void loadHexFile(const QString& path);

public:
    MCUChip();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override {
        return "Microcontroller (MCU)";
    }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Hex File Path"] = hexFilePath;
        props["Clock Frequency"] = clockFrequency;
        return props;
    }

    // 🛠️ فیکس ارور: اینجا تابع را فقط معرفی می‌کنیم و بدنه‌اش در mcu.cpp می‌ماند
    void setProperties(const QMap<QString, QString>& props) override;
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
};
