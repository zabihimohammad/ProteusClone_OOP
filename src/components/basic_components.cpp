#include "basic_components.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include "../core/terminal.h"
#include <QGraphicsScene>

// ==========================================
// ۱. پیاده‌سازی کلاس مقاومت (Resistor)
// ==========================================
Resistor::Resistor() {
    (new Terminal(this))->setPos(-30, 0);
    (new Terminal(this))->setPos(30, 0);
}
QRectF Resistor::boundingRect() const
{
    return QRectF(-90, -55, 165, 110);
}
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
    drawReadableText(painter, QRectF(-35, -25, 70, 15), Qt::AlignCenter, resistance);
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
    drawReadableText(painter,QRectF(-35, -30, 70, 15), Qt::AlignCenter, capacitance);
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
    drawReadableText(painter,QRectF(-35, -25, 70, 15), Qt::AlignCenter, inductance);
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
    drawReadableText(painter,QRectF(-25, -35, 50, 15), Qt::AlignCenter, frequency);
}
void PulseGenerator::process() {
    tickCount++;
    if (tickCount >= 10) { // هر 1 ثانیه (10 تا 100 میلی‌ثانیه)
        tickCount = 0;
        if (currentMockVoltage == "0.0V") {
            currentMockVoltage = "5.0V";
        } else {
            currentMockVoltage = "0.0V";
        }
    }

    // 🛠️ فیکس طلایی: استخراج مقدار عددی ولتاژ (از QString کپی می‌گیریم تا متغیر اصلی خراب نشود)
    double v = QString(currentMockVoltage).replace("V", "").toDouble();

    for (QGraphicsItem *child : childItems()) {
        Terminal *term = dynamic_cast<Terminal*>(child);
        if (term) {
            // 🛠️ حالا موتور فیزیک می‌فهمد که این پایه یک منبع انرژی (Driven) است
            term->setVoltage(v);
        }
    }
}
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
    drawReadableText(painter,QRectF(-35, -30, 70, 15), Qt::AlignCenter, initialState);
}

// 🛠️ منطق اضافه شده برای سوییچ
void Switch::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        initialState = (initialState == "Open") ? "Closed" : "Open";
        update(); // بازرسم گرافیک
    }
    Element::mousePressEvent(event);
}

void Switch::process() {
    Terminal* t1 = dynamic_cast<Terminal*>(childItems()[0]);
    Terminal* t2 = dynamic_cast<Terminal*>(childItems()[1]);
    if (initialState == "Closed" && t1 && t2) {
        if (t1->getVoltage() > 0.1) t2->setVoltage(t1->getVoltage());
        else if (t2->getVoltage() > 0.1) t1->setVoltage(t2->getVoltage());
    }
}
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

    // اگر دکمه فشرده شده بود، پلات گرافیکی خط وسط را پایین‌تر می‌کشیم
    int offsetY = isPressed ? -2 : -15;
    painter->drawLine(-15, offsetY, 15, offsetY);
    painter->drawLine(0, offsetY, 0, offsetY - 10);

    painter->drawEllipse(6, -2, 4, 4);
    painter->drawLine(10, 0, 30, 0);

    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    drawReadableText(painter,QRectF(-35, -35, 70, 15), Qt::AlignCenter, type);
}

// 🛠️ منطق اضافه شده برای دکمه فشاری
void PushButton::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isPressed = true;
        update();
    }
    Element::mousePressEvent(event);
}

void PushButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isPressed = false;
        update();
    }
    Element::mouseReleaseEvent(event);
}

void PushButton::process() {
    Terminal* t1 = dynamic_cast<Terminal*>(childItems()[0]);
    Terminal* t2 = dynamic_cast<Terminal*>(childItems()[1]);
    bool active = (type == "NO") ? isPressed : !isPressed;
    if (active && t1 && t2) {
        if (t1->getVoltage() > 0.1) t2->setVoltage(t1->getVoltage());
        else if (t2->getVoltage() > 0.1) t1->setVoltage(t2->getVoltage());
    }
}
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

    // 🛠️ تغییر رنگ بر اساس روشن یا خاموش بودن در شبیه‌سازی
    painter->setBrush(isOn ? QColor(color) : QColor(50, 50, 50));
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
    drawReadableText(painter,QRectF(-35, -35, 70, 15), Qt::AlignCenter, color);
}

// 🛠️ منطق بررسی ولتاژ آند دیود
void LED::process() {
    Terminal* anode = dynamic_cast<Terminal*>(childItems()[0]);
    if (anode) {
        isOn = (anode->getVoltage() >= 2.0); // آستانه روشن شدن
        update();
    }
}
QMap<QString, QString> LED::getProperties() const {
    QMap<QString, QString> props;
    props["Color"] = color;
    return props;
}
void LED::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Color")) color = props["Color"];
}


// ==========================================
// پیاده‌سازی سون‌سگمنت
// ==========================================
SevenSegment::SevenSegment() {
    // 🛠️ اضافه کردن ۷ پین مجزا برای وصل کردن سیم!
    for(int i = 0; i < 7; i++) {
        Terminal *t = new Terminal(this);
        t->setPos(-15 + i * 5, 30); // پین‌ها در لبه‌ی پایینی قرار می‌گیرند
    }
}
QRectF SevenSegment::boundingRect() const { return QRectF(-25, -45, 50, 90); }
void SevenSegment::process() { update(); }
void SevenSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawRect(-20, -30, 40, 60);

    // 🛠️ خواندن ولتاژ از ۷ پین
    bool a = childItems().size() > 0 && dynamic_cast<Terminal*>(childItems()[0])->getLogicState();
    bool b = childItems().size() > 1 && dynamic_cast<Terminal*>(childItems()[1])->getLogicState();
    bool c = childItems().size() > 2 && dynamic_cast<Terminal*>(childItems()[2])->getLogicState();
    bool d = childItems().size() > 3 && dynamic_cast<Terminal*>(childItems()[3])->getLogicState();
    bool e = childItems().size() > 4 && dynamic_cast<Terminal*>(childItems()[4])->getLogicState();
    bool f = childItems().size() > 5 && dynamic_cast<Terminal*>(childItems()[5])->getLogicState();
    bool g = childItems().size() > 6 && dynamic_cast<Terminal*>(childItems()[6])->getLogicState();

    QColor onColor(255, 0, 0); // قرمز روشن
    QColor offColor(220, 220, 220); // خاکستری خاموش

    painter->setPen(QPen(a ? onColor : offColor, 4)); painter->drawLine(-10, -20, 10, -20); // A
    painter->setPen(QPen(b ? onColor : offColor, 4)); painter->drawLine(15, -15, 15, -5); // B
    painter->setPen(QPen(c ? onColor : offColor, 4)); painter->drawLine(15, 5, 15, 15); // C
    painter->setPen(QPen(d ? onColor : offColor, 4)); painter->drawLine(-10, 20, 10, 20); // D
    painter->setPen(QPen(e ? onColor : offColor, 4)); painter->drawLine(-15, 5, -15, 15); // E
    painter->setPen(QPen(f ? onColor : offColor, 4)); painter->drawLine(-15, -15, -15, -5); // F
    painter->setPen(QPen(g ? onColor : offColor, 4)); painter->drawLine(-10, 0, 10, 0); // G
}
QMap<QString, QString> SevenSegment::getProperties() const { QMap<QString, QString> props; props["Color"] = color; return props; }
void SevenSegment::setProperties(const QMap<QString, QString>& props) { if (props.contains("Color")) color = props["Color"]; }
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
    drawReadableText(painter,QRectF(-20, 15, 40, 15), Qt::AlignCenter, voltage);
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
    drawReadableText(painter,QRectF(-25, 25, 50, 15), Qt::AlignCenter, frequency);
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
// ==========================================
// گره اتصال (Junction Node)
// ==========================================
JunctionNode::JunctionNode() { term = new Terminal(this); term->setPos(0, 0); }
JunctionNode::~JunctionNode() { /* 🛠️ فیکس کرش: اینجا باید خالی باشد! */ }
QRectF JunctionNode::boundingRect() const { return QRectF(-6, -6, 12, 12); }
void JunctionNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setBrush(isSelected() ? Qt::red : Qt::black); painter->setPen(Qt::NoPen); painter->drawEllipse(-4, -4, 8, 8);
}
void JunctionNode::process() {}

// ==========================================
// ولت‌متر دیجیتال
// ==========================================
Voltmeter::Voltmeter() { t1 = new Terminal(this); t1->setPos(-30, 0); t2 = new Terminal(this); t2->setPos(30, 0); }
QRectF Voltmeter::boundingRect() const { return QRectF(-35, -25, 70, 50); }
void Voltmeter::process() { update(); }
void Voltmeter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2); if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen); painter->drawEllipse(-20, -20, 40, 40); painter->drawLine(-30, 0, -20, 0); painter->drawLine(20, 0, 30, 0);
    painter->setFont(QFont("Consolas", 12, QFont::Bold)); painter->setPen(Qt::darkBlue);
    drawReadableText(painter, QRectF(-20, -20, 40, 40), Qt::AlignCenter, "V");

    double diff = qAbs(t1->exactVoltage - t2->exactVoltage);
    painter->setFont(QFont("Consolas", 8, QFont::Bold)); painter->setPen(QColor(0, 150, 0));
    drawReadableText(painter, QRectF(-35, 25, 70, 15), Qt::AlignCenter, QString::number(diff, 'f', 2) + " V");
}

// ==========================================
// آمپرمتر دیجیتال
// ==========================================
Ammeter::Ammeter() { t1 = new Terminal(this); t1->setPos(-30, 0); t2 = new Terminal(this); t2->setPos(30, 0); }
QRectF Ammeter::boundingRect() const { return QRectF(-35, -25, 70, 50); }
void Ammeter::process() { update(); }
void Ammeter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2); if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen); painter->drawEllipse(-20, -20, 40, 40); painter->drawLine(-30, 0, -20, 0); painter->drawLine(20, 0, 30, 0);
    painter->setFont(QFont("Consolas", 12, QFont::Bold)); painter->setPen(Qt::darkBlue);
    drawReadableText(painter, QRectF(-20, -20, 40, 40), Qt::AlignCenter, "A");

    // 🛠️ فیکس دقت صفر ماندن آمپرمتر
    double current = qAbs(t1->exactVoltage - t2->exactVoltage) / 0.001;
    QString currentStr;
    if (current < 1e-6) currentStr = "0.00 mA";
    else if (current < 1e-3) currentStr = QString::number(current * 1e6, 'f', 1) + " µA";
    else if (current < 1.0) currentStr = QString::number(current * 1e3, 'f', 1) + " mA";
    else currentStr = QString::number(current, 'f', 2) + " A";

    painter->setFont(QFont("Consolas", 8, QFont::Bold)); painter->setPen(QColor(150, 0, 0));
    drawReadableText(painter, QRectF(-35, 25, 70, 15), Qt::AlignCenter, currentStr);
}

// ==========================================
// اسیلوسکوپ گرافیکی
// ==========================================
Oscilloscope::Oscilloscope() {
    inChannel = new Terminal(this);
    inChannel->setPos(-80, 0);
    voltageHistory.fill(0.0, maxSamples);
}
QRectF Oscilloscope::boundingRect() const {
    return QRectF(-90, -55, 165, 110); // 🛠️ فیکس کادر برای کلیک شدن روی پایه
}
void Oscilloscope::process() {
    double currentV = inChannel ? inChannel->exactVoltage : 0.0;
    voltageHistory.pop_front();
    voltageHistory.append(currentV);
    update();
}
void Oscilloscope::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save(); painter->setRenderHint(QPainter::Antialiasing);

    QPen pinPen(Qt::black, 2); if (isSelected()) pinPen.setColor(Qt::red);
    painter->setPen(pinPen);
    painter->drawLine(-80, 0, -70, 0); // 🛠️ رسم پین گمشده

    painter->setFont(QFont("Consolas", 7, QFont::Bold)); painter->setPen(Qt::darkBlue);
    drawReadableText(painter, QRectF(-85, -15, 20, 10), Qt::AlignLeft, "IN");

    QLinearGradient bgGrad(-70, -50, 70, 50);
    bgGrad.setColorAt(0.0, QColor("#2D3540")); bgGrad.setColorAt(1.0, QColor("#1A1F26"));
    painter->setBrush(bgGrad);
    QPen bodyPen(isSelected() ? Qt::red : QColor("#4F5D6B"), 2.5); painter->setPen(bodyPen);
    painter->drawRoundedRect(-70, -50, 140, 100, 10, 10);

    QRectF screenRect(-60, -42, 120, 74);
    painter->setBrush(QColor("#0D1117")); painter->setPen(QPen(QColor("#30363D"), 1.5));
    painter->drawRoundedRect(screenRect, 4, 4);
    painter->setClipRect(screenRect);

    QColor gridColor("#00FFCC"); gridColor.setAlpha(25);
    painter->setPen(QPen(gridColor, 1, Qt::DotLine));
    for (int x = -50; x <= 50; x += 15) painter->drawLine(x, -42, x, 32);
    for (int y = -30; y <= 30; y += 12) painter->drawLine(-60, y, 60, y);

    QPainterPath wavePath;
    double stepX = 120.0 / (maxSamples - 1);
    auto voltageToY = [&](double v) { return 26.0 - (qBound(0.0, v, 5.0) / 5.0) * 56.0; };

    wavePath.moveTo(-60.0, voltageToY(voltageHistory[0]));
    for (int i = 1; i < maxSamples; ++i) wavePath.lineTo(-60.0 + (i * stepX), voltageToY(voltageHistory[i]));

    QColor glowColor("#00FFCC"); glowColor.setAlpha(60);
    painter->setPen(QPen(glowColor, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(wavePath);
    painter->setPen(QPen(QColor("#FFFFFF"), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(wavePath);

    painter->setClipping(false);
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(QColor("#00FFCC"));
    painter->drawText(QRectF(-55, -38, 50, 15), Qt::AlignLeft, QString::number(voltageHistory.last(), 'f', 1) + "V");

    painter->setPen(QColor("#9AA6B5")); painter->setFont(QFont("Segoe UI", 7, QFont::DemiBold));
    drawReadableText(painter, QRectF(-70, 37, 140, 12), Qt::AlignCenter, "DIGITAL OSCILLOSCOPE");
    painter->restore();
}
// ==========================================
// پیاده‌سازی باتری واقعی (Battery)
// ==========================================
Battery::Battery() {
    outPos = new Terminal(this); outPos->setPos(0, -20);
    outNeg = new Terminal(this); outNeg->setPos(0, 20);
}
QRectF Battery::boundingRect() const { return QRectF(-25, -30, 50, 60); }
void Battery::process() {}
void Battery::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2); if (isSelected()) pen.setColor(Qt::red); painter->setPen(pen);

    painter->drawLine(0, -20, 0, -5);
    painter->drawLine(-15, -5, 15, -5); // قطب مثبت (خط بلند)
    painter->drawLine(-8, 5, 8, 5);     // قطب منفی (خط کوتاه)
    painter->drawLine(0, 5, 0, 20);

    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    drawReadableText(painter, -20, -10, "+");
    drawReadableText(painter, QRectF(-25, -30, 50, 15), Qt::AlignCenter, voltage);
}
QMap<QString, QString> Battery::getProperties() const {
    QMap<QString, QString> props;
    props["Voltage"] = voltage;
    props["Internal Resistance"] = internalResistance;
    return props;
}
void Battery::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Voltage")) voltage = props["Voltage"];
    if (props.contains("Internal Resistance")) internalResistance = props["Internal Resistance"];
}