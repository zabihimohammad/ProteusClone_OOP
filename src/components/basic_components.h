#pragma once
#include "../core/element.h"
#include <QString>

// پیش‌تعریف کلاس ترمینال
class Terminal;

// ==========================================
// ۱. مقاومت (Resistor)
// ==========================================
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

// ==========================================
// ۲. خازن (Capacitor)
// ==========================================
class Capacitor : public Element {
private:
    QString capacitance = "10uF";
public:
    Capacitor();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Capacitor"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// ۳. سلف (Inductor)
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

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// ۴. پالس کلاک (Pulse Generator)
// ==========================================
class PulseGenerator : public Element {
private:
    QString frequency = "1kHz";
public:
    PulseGenerator();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Pulse Generator"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// ۵. کلید قطع و وصل (Switch)
// ==========================================
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
};

// ==========================================
// ۶. دکمه فشاری (Push Button)
// ==========================================
class PushButton : public Element {
private:
    QString type = "NO"; // Normally Open
public:
    PushButton();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "Push Button"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// ۷. دیود نورانی (LED)
// ==========================================
class LED : public Element {
private:
    QString color = "Red";
public:
    LED();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;
    QString getComponentName() const override { return "LED"; }

    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// ۸. سون‌سگمنت (7-Segment)
// ==========================================
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

// ==========================================
// ۹. منبع تغذیه زمین (Ground / GND)
// ==========================================
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

// ==========================================
// ۱۰. منبع ولتاژ مستقیم (DC Voltage Source)
// ==========================================
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

// ==========================================
// ۱۱. تولیدکننده کلاک (Clock Generator)
// ==========================================
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
};