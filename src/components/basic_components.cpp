#include "basic_components.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include "../core/terminal.h"

// ==========================================
// ۱. پیاده‌سازی کلاس مقاومت (Resistor)
// ==========================================
Resistor::Resistor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Resistor::boundingRect() const { return QRectF(-35, -25, 70, 50); }

void Resistor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -20, 0);
    painter->drawLine(-20, 0, -15, -10);
    painter->drawLine(-15, -10, -5, 10);
    painter->drawLine(-5, 10, 5, -10);
    painter->drawLine(5, -10, 15, 10);
    painter->drawLine(15, 10, 20, 0);
    painter->drawLine(20, 0, 30, 0);

    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -25, 70, 15), Qt::AlignCenter, resistance);
}

void Resistor::process() {}

QMap<QString, QString> Resistor::getProperties() const {
    QMap<QString, QString> props;
    props["Resistance"] = resistance;
    return props;
}

void Resistor::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Resistance")) resistance = props["Resistance"];
}


// ==========================================
// ۲. پیاده‌سازی کلاس خازن (Capacitor)
// ==========================================
Capacitor::Capacitor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Capacitor::boundingRect() const { return QRectF(-35, -30, 70, 60); }

void Capacitor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -5, 0);
    painter->drawLine(5, 0, 30, 0);
    painter->drawLine(-5, -15, -5, 15);
    painter->drawLine(5, -15, 5, 15);

    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -30, 70, 15), Qt::AlignCenter, capacitance);
}

void Capacitor::process() {}

QMap<QString, QString> Capacitor::getProperties() const {
    QMap<QString, QString> props;
    props["Capacitance"] = capacitance;
    return props;
}

void Capacitor::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Capacitance")) capacitance = props["Capacitance"];
}


// ==========================================
// ۳. پیاده‌سازی سلف (Inductor)
// ==========================================
Inductor::Inductor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Inductor::boundingRect() const { return QRectF(-35, -25, 70, 50); }

void Inductor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -20, 0);
    painter->drawArc(-20, -10, 13, 20, 0, 180 * 16);
    painter->drawArc(-7, -10, 13, 20, 0, 180 * 16);
    painter->drawArc(6, -10, 13, 20, 0, 180 * 16);
    painter->drawLine(19, 0, 30, 0);

    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -25, 70, 15), Qt::AlignCenter, inductance);
}

void Inductor::process() {}

QMap<QString, QString> Inductor::getProperties() const {
    QMap<QString, QString> props;
    props["Inductance"] = inductance;
    return props;
}

void Inductor::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Inductance")) inductance = props["Inductance"];
}


// ==========================================
// ۴. پیاده‌سازی پالس کلاک (Pulse Generator)
// ==========================================
PulseGenerator::PulseGenerator() {
    (new Terminal(this))->setPos(30, 0);
}

QRectF PulseGenerator::boundingRect() const { return QRectF(-25, -35, 55, 60); }

void PulseGenerator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawEllipse(-20, -20, 40, 40);
    painter->drawLine(-10, 10, -10, -10);
    painter->drawLine(-10, -10, 0, -10);
    painter->drawLine(0, -10, 0, 10);
    painter->drawLine(0, 10, 10, 10);
    painter->drawLine(20, 0, 30, 0);

    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-25, -35, 50, 15), Qt::AlignCenter, frequency);
}

void PulseGenerator::process() {}

QMap<QString, QString> PulseGenerator::getProperties() const {
    QMap<QString, QString> props;
    props["Frequency"] = frequency;
    return props;
}

void PulseGenerator::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Frequency")) frequency = props["Frequency"];
}


// ==========================================
// ۵. کلید قطع و وصل (Switch)
// ==========================================
Switch::Switch() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF Switch::boundingRect() const { return QRectF(-35, -30, 70, 60); }

void Switch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -15, 0);
    painter->drawEllipse(-15, -2, 4, 4);

    if (initialState == "Closed") {
        painter->drawLine(-13, -2, 11, -2);
    } else {
        painter->drawLine(-13, -2, 10, -15);
    }

    painter->drawEllipse(11, -2, 4, 4);
    painter->drawLine(15, 0, 30, 0);

    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -30, 70, 15), Qt::AlignCenter, initialState);
}

void Switch::process() {}

QMap<QString, QString> Switch::getProperties() const {
    QMap<QString, QString> props;
    props["Initial State"] = initialState;
    return props;
}

void Switch::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Initial State")) initialState = props["Initial State"];
}


// ==========================================
// ۶. دکمه فشاری (Push Button)
// ==========================================
PushButton::PushButton() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF PushButton::boundingRect() const { return QRectF(-35, -35, 70, 60); }

void PushButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -10, 0);
    painter->drawEllipse(-10, -2, 4, 4);
    painter->drawLine(-15, -15, 15, -15);
    painter->drawLine(0, -15, 0, -25);
    painter->drawEllipse(6, -2, 4, 4);
    painter->drawLine(10, 0, 30, 0);

    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -35, 70, 15), Qt::AlignCenter, type);
}

void PushButton::process() {}

QMap<QString, QString> PushButton::getProperties() const {
    QMap<QString, QString> props;
    props["Type"] = type;
    return props;
}

void PushButton::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Type")) type = props["Type"];
}


// ==========================================
// ۷. دیود نورانی (LED)
// ==========================================
LED::LED() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}

QRectF LED::boundingRect() const { return QRectF(-35, -35, 70, 65); }

void LED::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, 0, -10, 0);
    painter->drawLine(10, 0, 30, 0);

    QPolygonF triangle;
    triangle << QPointF(-10, -10) << QPointF(-10, 10) << QPointF(10, 0);
    painter->drawPolygon(triangle);
    painter->drawLine(10, -10, 10, 10);

    painter->drawLine(0, -12, 10, -22);
    painter->drawLine(10, -22, 5, -22);
    painter->drawLine(10, -22, 10, -17);
    painter->drawLine(8, -8, 18, -18);
    painter->drawLine(18, -18, 13, -18);
    painter->drawLine(18, -18, 18, -13);

    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-35, -35, 70, 15), Qt::AlignCenter, color);
}

void LED::process() {}

QMap<QString, QString> LED::getProperties() const {
    QMap<QString, QString> props;
    props["Color"] = color;
    return props;
}

void LED::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Color")) color = props["Color"];
}


// ==========================================
// ۸. سون‌سگمنت (7-Segment)
// ==========================================
SevenSegment::SevenSegment() {}

QRectF SevenSegment::boundingRect() const { return QRectF(-25, -45, 50, 90); }

void SevenSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawRect(-20, -30, 40, 60);

    QPen segPen(QColor(220, 220, 220), 4);
    painter->setPen(segPen);
    painter->drawLine(-10, -20, 10, -20);
    painter->drawLine(15, -15, 15, -5);
    painter->drawLine(15, 5, 15, 15);
    painter->drawLine(-10, 20, 10, 20);
    painter->drawLine(-15, 5, -15, 15);
    painter->drawLine(-15, -15, -15, -5);
    painter->drawLine(-10, 0, 10, 0);
    painter->drawEllipse(15, 20, 3, 3);

    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-25, -45, 50, 15), Qt::AlignCenter, color);
}

void SevenSegment::process() {}

QMap<QString, QString> SevenSegment::getProperties() const {
    QMap<QString, QString> props;
    props["Color"] = color;
    return props;
}

void SevenSegment::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Color")) color = props["Color"];
}


// ============================================================================
// ۹. پیاده‌سازی Ground (زمین)
// ============================================================================
Ground::Ground() {
    outGnd = new Terminal(this);
    outGnd->setPos(0, -20);
}
QRectF Ground::boundingRect() const { return QRectF(-20, -25, 40, 45); }
void Ground::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(0, -20, 0, 0);
    painter->drawLine(-15, 0, 15, 0);
    painter->drawLine(-10, 5, 10, 5);
    painter->drawLine(-5, 10, 5, 10);
}
void Ground::process() {
    outGnd->setVoltage(0.0);
}


// ============================================================================
// ۱۰. پیاده‌سازی DC Voltage Source (هوشمند)
// ============================================================================
DCVoltageSource::DCVoltageSource() {
    outPos = new Terminal(this);
    outPos->setPos(0, -20);
}
QRectF DCVoltageSource::boundingRect() const { return QRectF(-20, -25, 40, 50); }
void DCVoltageSource::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(0, -20, 0, -10);
    painter->drawEllipse(-10, -10, 20, 20);
    painter->drawLine(-5, 0, 5, 0);
    painter->drawLine(0, -5, 0, 5);

    painter->setFont(QFont("Consolas", 8));
    painter->drawText(QRectF(-20, 15, 40, 15), Qt::AlignCenter, voltage);
}
void DCVoltageSource::process() {
    double v = voltage.replace("V", "").toDouble();
    outPos->setVoltage(v);
}
QMap<QString, QString> DCVoltageSource::getProperties() const {
    QMap<QString, QString> props;
    props["Voltage"] = voltage;
    return props;
}
void DCVoltageSource::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Voltage")) voltage = props["Voltage"];
}


// ============================================================================
// ۱۱. پیاده‌سازی Clock Generator
// ============================================================================
ClockGenerator::ClockGenerator() {
    outClk = new Terminal(this);
    outClk->setPos(25, 0);
}
QRectF ClockGenerator::boundingRect() const { return QRectF(-25, -25, 55, 65); }
void ClockGenerator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawRect(-20, -20, 40, 40);
    painter->drawLine(20, 0, 25, 0);

    painter->drawLine(-10, 10, -5, 10);
    painter->drawLine(-5, 10, -5, -10);
    painter->drawLine(-5, -10, 5, -10);
    painter->drawLine(5, -10, 5, 10);
    painter->drawLine(5, 10, 10, 10);

    painter->setFont(QFont("Consolas", 7));
    painter->drawText(QRectF(-25, 25, 50, 15), Qt::AlignCenter, frequency);
}
void ClockGenerator::process() {
    double v = currentState ? amplitude.replace("V", "").toDouble() : 0.0;
    outClk->setVoltage(v);
}
void ClockGenerator::toggleClock() {
    currentState = !currentState;
    update();
}
QMap<QString, QString> ClockGenerator::getProperties() const {
    QMap<QString, QString> props;
    props["Frequency"] = frequency;
    props["Amplitude"] = amplitude;
    return props;
}
void ClockGenerator::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Frequency")) frequency = props["Frequency"];
    if (props.contains("Amplitude")) amplitude = props["Amplitude"];
}