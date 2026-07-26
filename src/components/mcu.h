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

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Hex File Path")) hexFilePath = props["Hex File Path"];
        if (props.contains("Clock Frequency")) clockFrequency = props["Clock Frequency"];
    }
};