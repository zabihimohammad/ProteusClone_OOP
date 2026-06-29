#pragma once
#include "../core/element.h"

// ==========================================
// کلاس مقاومت (Resistor)
// ==========================================
class Resistor : public Element {
private:
    QString resistance = "10k";
    QString tolerance = "5%";

public:
    Resistor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Resistor"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Resistance (Ohm)"] = resistance;
        props["Tolerance"] = tolerance;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Resistance (Ohm)")) resistance = props["Resistance (Ohm)"];
        if (props.contains("Tolerance")) tolerance = props["Tolerance"];
    }
};

// ==========================================
// کلاس خازن (Capacitor)
// ==========================================
class Capacitor : public Element {
private:
    QString capacitance = "100uF";
    QString maxVoltage = "16V";

public:
    Capacitor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Capacitor"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Capacitance"] = capacitance;
        props["Max Voltage"] = maxVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Capacitance")) capacitance = props["Capacitance"];
        if (props.contains("Max Voltage")) maxVoltage = props["Max Voltage"];
    }
};

// ==========================================
// کلاس منبع ولتاژ مستقیم (DC Voltage Source)
// ==========================================
class DCVoltageSource : public Element {
private:
    QString voltage = "5V";

public:
    DCVoltageSource();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "DC Battery"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Voltage (V)"] = voltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Voltage (V)")) voltage = props["Voltage (V)"];
    }
};

// ==========================================
// کلاس زمین (Ground)
// ==========================================
class Ground : public Element {
public:
    Ground();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Ground (GND)"; }
    // زمین معمولاً تنظیماتی ندارد، بنابراین توابع Properties را بازنویسی نمی‌کنیم
};

// ==========================================
// کلاس سلف (Inductor)
// ==========================================
class Inductor : public Element {
private:
    QString inductance = "1mH";

public:
    Inductor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Inductor"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Inductance"] = inductance;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Inductance")) inductance = props["Inductance"];
    }
};

// ==========================================
// پالس کلاک (Pulse Generator)
// ==========================================
class PulseGenerator : public Element {
private:
    QString amplitude = "5V";
    QString frequency = "1kHz";
    QString dutyCycle = "50%";

public:
    PulseGenerator();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Clock / Pulse"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Amplitude (V)"] = amplitude;
        props["Frequency (Hz)"] = frequency;
        props["Duty Cycle (%)"] = dutyCycle;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Amplitude (V)")) amplitude = props["Amplitude (V)"];
        if (props.contains("Frequency (Hz)")) frequency = props["Frequency (Hz)"];
        if (props.contains("Duty Cycle (%)")) dutyCycle = props["Duty Cycle (%)"];
    }
};

// ==========================================
// کلید قطع و وصل (Switch)
// ==========================================
class Switch : public Element {
private:
    QString initialState = "Open"; // Open or Closed

public:
    Switch();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "SPST Switch"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Initial State"] = initialState;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Initial State")) initialState = props["Initial State"];
    }
};

// ==========================================
// دکمه فشاری (Push Button)
// ==========================================
class PushButton : public Element {
private:
    QString type = "Normally Open";

public:
    PushButton();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "Push Button"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Button Type"] = type;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Button Type")) type = props["Button Type"];
    }
};

// ==========================================
// دیود نورانی (LED)
// ==========================================
class LED : public Element {
private:
    QString color = "Red";
    QString forwardVoltage = "2.2V";

public:
    LED();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "LED"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Color"] = color;
        props["Forward Voltage"] = forwardVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Color")) color = props["Color"];
        if (props.contains("Forward Voltage")) forwardVoltage = props["Forward Voltage"];
    }
};

// ==========================================
// سون‌سگمنت (7-Segment Display)
// ==========================================
class SevenSegment : public Element {
private:
    QString color = "Red";
    QString type = "Common Cathode";

public:
    SevenSegment();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "7-Segment Display"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["LED Color"] = color;
        props["Configuration"] = type;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("LED Color")) color = props["LED Color"];
        if (props.contains("Configuration")) type = props["Configuration"];
    }
};