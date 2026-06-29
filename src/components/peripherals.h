#pragma once
#include "../core/element.h"

// ==========================================
// حافظه خارجی (RAM / EEPROM)
// ==========================================
class MemoryChip : public Element {
private:
    QString memorySize = "64 KB";
    QString initialHexPath = "Not Loaded";

public:
    MemoryChip();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "External Memory Chip"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Memory Size"] = memorySize;
        props["Storage Firmware (.hex)"] = initialHexPath;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Memory Size")) memorySize = props["Memory Size"];
        if (props.contains("Storage Firmware (.hex)")) initialHexPath = props["Storage Firmware (.hex)"];
    }
};

// ==========================================
// نمایشگر کاراکتری (LCD 16x2)
// ==========================================
class LCD16x2 : public Element {
private:
    QString busMode = "4-Bit Parallel";
    QString i2cAddress = "0x27";

public:
    LCD16x2();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "LCD 16x2 Display"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Bus Interface Mode"] = busMode;
        props["I2C Address (If Used)"] = i2cAddress;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Bus Interface Mode")) busMode = props["Bus Interface Mode"];
        if (props.contains("I2C Address (If Used)")) i2cAddress = props["I2C Address (If Used)"];
    }
};

// ==========================================
// صفحه کلید ماتریسی (Keypad 4x4)
// ==========================================
class Keypad : public Element {
private:
    QString debounceTimeMs = "20ms";

public:
    Keypad();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Matrix Keypad 4x4"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Debounce Delay"] = debounceTimeMs;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Debounce Delay")) debounceTimeMs = props["Debounce Delay"];
    }
};

// ==========================================
// مبدل آنالوگ به دیجیتال (ADC)
// ==========================================
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

// ==========================================
// مبدل دیجیتال به آنالوگ (DAC)
// ==========================================
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