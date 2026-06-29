#pragma once
#include "../core/element.h"
#include "../core/terminal.h"

// ==========================================
// گیت AND
// ==========================================
class AndGate : public Element {
private:
    QString propagationDelay = "10ns";
    QString highVoltage = "5V";

public:
    AndGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "AND Gate"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Propagation Delay"] = propagationDelay;
        props["Logic HIGH (V)"] = highVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
        if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
    }
};

// ==========================================
// گیت OR
// ==========================================
class OrGate : public Element {
private:
    QString propagationDelay = "10ns";
    QString highVoltage = "5V";

public:
    OrGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "OR Gate"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Propagation Delay"] = propagationDelay;
        props["Logic HIGH (V)"] = highVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
        if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
    }
};

// ==========================================
// گیت NOT
// ==========================================
class NotGate : public Element {
private:
    QString propagationDelay = "5ns"; // گیت نات معمولا سریع‌تر است
    QString highVoltage = "5V";

public:
    NotGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "NOT Gate"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Propagation Delay"] = propagationDelay;
        props["Logic HIGH (V)"] = highVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
        if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
    }
};

// ==========================================
// گیت XOR
// ==========================================
class XorGate : public Element {
private:
    QString propagationDelay = "12ns";
    QString highVoltage = "5V";

public:
    XorGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "XOR Gate"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Propagation Delay"] = propagationDelay;
        props["Logic HIGH (V)"] = highVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
        if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
    }
};

// ==========================================
// گیت NAND
// ==========================================
class NandGate : public Element {
private:
    QString propagationDelay = "10ns";
    QString highVoltage = "5V";

public:
    NandGate();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "NAND Gate"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Propagation Delay"] = propagationDelay;
        props["Logic HIGH (V)"] = highVoltage;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
        if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
    }
};

// ==========================================
// فلیپ‌فلاپ (D-FlipFlop)
// ==========================================
class DFlipFlop : public Element {
private:
    QString propagationDelay = "15ns";
    QString initialQState = "0"; // وضعیت اولیه خروجی Q

public:
    DFlipFlop();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void process() override;

    QString getComponentName() const override { return "D-Type Flip-Flop"; }

    QMap<QString, QString> getProperties() const override {
        QMap<QString, QString> props;
        props["Propagation Delay"] = propagationDelay;
        props["Initial Q State (0/1)"] = initialQState;
        return props;
    }

    void setProperties(const QMap<QString, QString>& props) override {
        if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
        if (props.contains("Initial Q State (0/1)")) initialQState = props["Initial Q State (0/1)"];
    }
};