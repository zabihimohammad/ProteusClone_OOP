#pragma once
#include "../core/element.h"
#include <QString>

// پیش‌تعریف کلاس ترمینال
class Terminal;

// ۱. مقاومت (Resistor)
class Resistor : public Element {
private:
    QString resistance = "10k";
public:
    Resistor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Resistor"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ۲. خازن (Capacitor)
class Capacitor : public Element {
private:
    QString capacitance = "10uF";
    double storedVoltage = 0.0;
public:
    Capacitor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Capacitor"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
    double previousVoltage() const { return storedVoltage; }
    void setPreviousVoltage(double voltage) { storedVoltage = voltage; }
};

// ۳. سلف (Inductor)
class Inductor : public Element {
private:
    QString inductance = "1mH";
    double storedCurrent = 0.0;
public:
    Inductor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Inductor"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
    double previousCurrent() const { return storedCurrent; }
    void setPreviousCurrent(double current) { storedCurrent = current; }
};

// ۴. پالس کلاک (Pulse Generator)
class PulseGenerator : public Element {
private:
    int tickCount = 0; // شمارنده زمان
    QString currentMockVoltage = "0.0V"; // ولتاژ فعلی
    QString amplitude = "5V";
    QString frequency = "1kHz";
public:
    PulseGenerator();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Pulse Generator"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
};

// ۵. کلید قطع و وصل (Switch)
class Switch : public Element {
private:
    QString initialState = "Open";
public:
    Switch();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Switch"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;

    // وضعیت رسانایی
    bool isClosed() const { return initialState == "Closed"; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

// ۶. دکمه فشاری (Push Button)
class PushButton : public Element {
private:
    QString type = "NO"; // Normally Open
    bool isPressed = false;
public:
    PushButton();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Push Button"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;

    // وضعیت رسانایی
    bool isClosed() const { return (type == "NO") ? isPressed : !isPressed; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

// ۷. دیود نورانی (LED)
class LED : public Element {
private:
    QString color = "Red";
    bool isOn = false;
public:
    LED();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "LED"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ۸. سون‌سگمنت (7-Segment)
class SevenSegment : public Element {
private:
    QString color = "Red";
public:
    SevenSegment();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "7-Segment Display"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ۹. منبع تغذیه زمین (Ground / GND)
class Ground : public Element {
private:
    Terminal *outGnd; // پایه‌ی خروجی زمین
public:
    Ground();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Ground (GND)"; }
};

// ۱۰. منبع ولتاژ مستقیم (DC Voltage Source)
class DCVoltageSource : public Element {
private:
    QString voltage = "5V";
    Terminal *outPos; // پایه‌ی خروجی مثبت
public:
    DCVoltageSource();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "DC Voltage Source"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ۱۱. تولیدکننده کلاک (Clock Generator)
class ClockGenerator : public Element {
private:
    QString frequency = "1Hz";
    QString amplitude = "5V";
    Terminal *outClk; // پایه‌ی خروجی کلاک
    bool currentState = false; // وضعیت خاموش/روشن بودن کلاک
public:
    ClockGenerator();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    void toggleClock(); // متدی برای موتور شبیه‌ساز جهت تغییر لبه کلاک
    QString getComponentName() const override { return "Clock Generator"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
};

// ۱۲. گره اتصال سیم به سیم (Junction Node)
class JunctionNode : public Element {
public:
    Terminal *term; // تنها پایه این گره که سیم‌ها به آن وصل می‌شوند

    JunctionNode();
    ~JunctionNode() override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Junction Node"; }
};
// باتری واقعی (Real Battery with Internal Resistance)
class Battery : public Element {
private:
    QString voltage = "9V";
    QString internalResistance = "1"; // 1 Ohm مقاومت داخلی
    Terminal *outPos;
    Terminal *outNeg;
public:
    Battery();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Battery"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};
// ولت‌متر دیجیتال (Voltmeter)
class Voltmeter : public Element {
private: Terminal *t1; Terminal *t2;
public:
    Voltmeter();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Voltmeter"; }
};

// آمپرمتر دیجیتال (Ammeter)
class Ammeter : public Element {
private: Terminal *t1; Terminal *t2;
public:
    Ammeter();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Ammeter"; }
};
// ۱۳. اسیلوسکوپ (Oscilloscope)
class Oscilloscope : public Element {
private:
    Terminal *inChannel;          // پین ورودی کانال اسیلوسکوپ
    QVector<double> voltageHistory; // نمونه‌های قبلی
    const int maxSamples = 60;    // تعداد نمونه‌ها
    int updateCounter = 0;

public:
    Oscilloscope();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Oscilloscope"; }
    QJsonObject getDynamicState() const override;
    void setDynamicState(const QJsonObject& state) override;
    void resetSimulationState() override;
};
