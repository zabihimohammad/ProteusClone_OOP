#pragma once
#include "../core/element.h"
#include "../core/terminal.h"

// ==========================================
// گیت AND (دو ورودی)
// ==========================================
class AndGate : public Element {
private:
    QString propagationDelay = "10ns";
    QString highVoltage = "5V";

    Terminal *inA;
    Terminal *inB;
    Terminal *outY;
    bool targetState = false;
    int delayTicks = -1;

public:
    AndGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "AND Gate"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// گیت OR (دو ورودی)
// ==========================================
class OrGate : public Element {
private:
    QString propagationDelay = "10ns";
    QString highVoltage = "5V";

    Terminal *inA;
    Terminal *inB;
    Terminal *outY;
    bool targetState = false;
    int delayTicks = -1;

public:
    OrGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "OR Gate"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// گیت NOT (وارون‌گر)
// ==========================================
class NotGate : public Element {
private:
    QString propagationDelay = "5ns";
    QString highVoltage = "5V";

    Terminal *inA;
    Terminal *outY;
    bool targetState = false;
    int delayTicks = -1;

public:
    NotGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "NOT Gate"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// گیت XOR (دو ورودی)
// ==========================================
class XorGate : public Element {
private:
    QString propagationDelay = "12ns";
    QString highVoltage = "5V";

    Terminal *inA;
    Terminal *inB;
    Terminal *outY;
    bool targetState = false;
    int delayTicks = -1;

public:
    XorGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "XOR Gate"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// گیت NAND (دو ورودی)
// ==========================================
class NandGate : public Element {
private:
    QString propagationDelay = "10ns";
    QString highVoltage = "5V";

    Terminal *inA;
    Terminal *inB;
    Terminal *outY;
    bool targetState = false;
    int delayTicks = -1;

public:
    NandGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "NAND Gate"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};

// ==========================================
// فلیپ‌فلاپ لبه‌بالارونده D (D-FlipFlop)
// ==========================================
class DFlipFlop : public Element {
private:
    QString propagationDelay = "15ns";
    QString initialQState = "0";

    Terminal *inD;
    Terminal *clk;
    Terminal *outQ;
    Terminal *outQBar;

    bool lastClockState = false; // ذخیره وضعیت قبلی کلاک برای تشخیص لبه بالارونده
    bool currentInternalQ = false;
    bool targetState = false;
    int delayTicks = -1;

public:
    DFlipFlop();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "D-Type Flip-Flop"; }
    QMap<QString, QString> getProperties() const override;
    void setProperties(const QMap<QString, QString>& props) override;
};