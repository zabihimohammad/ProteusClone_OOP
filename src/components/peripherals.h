#pragma once
#include "../core/element.h"

// حافظه خارجی (RAM / EEPROM)
class MemoryChip : public Element {
private:
    QString memorySize = "64 KB";
    QString initialHexPath = "Not Loaded";
    QMap<int, int> memoryData; // آرایه داخلی حافظه

public:
    MemoryChip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override; // پیاده‌سازی منطق Read/Write

    QString getComponentName() const override { return "External Memory Chip"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;

    // ذخیره محتوای RAM هنگام Save
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
};

// نمایشگر کاراکتری (LCD 16x2)
class LCD16x2 : public Element {
private:
    QString busMode = "8-Bit Parallel";
    QString i2cAddress = "0x27";
    QString displayText[2] = {"", ""}; // بافر خطوط LCD
    int cursorRow = 0;
    int cursorCol = 0;
    bool lastE = false; // ذخیره وضعیت کلاک (لبه بالارونده)

public:
    LCD16x2();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override; // منطق ۸-بیت

    QString getComponentName() const override { return "LCD 16x2 Display"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
};

// صفحه کلید ماتریسی (Keypad 4x4)
class Keypad : public Element {
private:
    QString debounceTimeMs = "20ms";
    int pressedRow = -1; // ذخیره سطر فشرده شده موس
    int pressedCol = -1; // ذخیره ستون فشرده شده موس

public:
    Keypad();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override; // اسکن ماتریسی

    QString getComponentName() const override { return "Matrix Keypad 4x4"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;

protected:
    // تشخیص تعامل موس
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};
// مبدل آنالوگ به دیجیتال (ADC)
class ADC_Chip : public Element {
private:
    QString resolutionBits = "10-Bit";
    QString referenceVoltage = "5.0V";

public:
    ADC_Chip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Analog to Digital Converter (ADC)"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Resolution (Bits)"] = resolutionBits;
        props["VREF Voltage (V)"] = referenceVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Resolution (Bits)")) resolutionBits = props["Resolution (Bits)"];
        if (props.contains("VREF Voltage (V)")) referenceVoltage = props["VREF Voltage (V)"];
    }
};

// مبدل دیجیتال به آنالوگ (DAC)
class DAC_Chip : public Element {
private:
    QString resolutionBits = "8-Bit";
    QString maxOutputVoltage = "5.0V";

public:
    DAC_Chip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Digital to Analog Converter (DAC)"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Resolution (Bits)"] = resolutionBits;
        props["Full-Scale VOUT"] = maxOutputVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Resolution (Bits)")) resolutionBits = props["Resolution (Bits)"];
        if (props.contains("Full-Scale VOUT")) maxOutputVoltage = props["Full-Scale VOUT"];
    }
};
