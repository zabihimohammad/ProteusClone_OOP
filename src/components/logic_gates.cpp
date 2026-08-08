#include "logic_gates.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QDebug>

// ============================================================================
// پیاده‌سازی گیت AND
// ============================================================================
AndGate::AndGate() {
    inA = new Terminal(this); inA->setPos(-30, -10);
    inB = new Terminal(this); inB->setPos(-30, 10);
    outY = new Terminal(this); outY->setPos(30, 0);
}

QRectF AndGate::boundingRect() const { return QRectF(-35, -25, 70, 65); }

void AndGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, -10, -15, -10);
    painter->drawLine(-30, 10, -15, 10);
    painter->drawLine(20, 0, 30, 0);

    painter->setBrush(QColor(240, 240, 240));
    QPainterPath path;
    path.moveTo(-15, -20);
    path.lineTo(0, -20);
    path.arcTo(QRectF(-20, -20, 40, 40), 90, -180);
    path.lineTo(-15, 20);
    path.closeSubpath();
    painter->drawPath(path);

    painter->setFont(QFont("Consolas", 6));
    painter->setPen(Qt::darkGray);
    drawReadableText(painter,QRectF(-30, 25, 60, 15), Qt::AlignCenter, propagationDelay);
}

void AndGate::process()
{
    if (inA->isFloating() || inB->isFloating() || inA->isUndefinedState() || inB->isUndefinedState()) {
        outY->setUndefined();
        return;
    }

    bool result = inA->getLogicState() && inB->getLogicState(); // 💡 برای گیت OR اینجا را || کن

    // 🛠️ سیستم تأخیر انتشار (Propagation Delay Event Queue)
    double delayNs = propagationDelay.replace("ns", "").toDouble();
    int requiredTicks = qMax(1, (int)(delayNs / 10.0)); // هر تیک شبیه‌ساز را ۱۰ نانوثانیه فرض می‌کنیم

    if (result != targetState) {
        targetState = result;
        delayTicks = requiredTicks; // استارت تایمر تأخیر
    }

    if (delayTicks > 0) {
        delayTicks--; // شمارش معکوس تا اعمال تغییر
    }

    if (delayTicks == 0) {
        // زمان تأخیر تمام شد، ولتاژ جدید اعمال می‌شود
        outY->setVoltage(targetState ? highVoltage.replace("V", "").toDouble() : 0.0);
        delayTicks = -1;
    }
}

QMap<QString, QString> AndGate::getProperties() const {
    QMap<QString, QString> props;
    props["Propagation Delay"] = propagationDelay;
    props["Logic HIGH (V)"] = highVoltage;
    return props;
}

void AndGate::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
    if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
}

// ============================================================================
// پیاده‌سازی گیت OR
// ============================================================================
OrGate::OrGate() {
    inA = new Terminal(this); inA->setPos(-30, -10);
    inB = new Terminal(this); inB->setPos(-30, 10);
    outY = new Terminal(this); outY->setPos(30, 0);
}

QRectF OrGate::boundingRect() const { return QRectF(-35, -25, 70, 65); }

void OrGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-30, -10, -10, -10);
    painter->drawLine(-30, 10, -10, 10);
    painter->drawLine(18, 0, 30, 0);

    painter->setBrush(QColor(240, 240, 240));
    QPainterPath path;
    path.moveTo(-15, -20);
    path.quadTo(QPointF(-5, -20), QPointF(18, 0));
    path.quadTo(QPointF(-5, 20), QPointF(-15, 20));
    path.quadTo(QPointF(-5, 0), QPointF(-15, -20));
    painter->drawPath(path);

    painter->setFont(QFont("Consolas", 6));
    painter->setPen(Qt::darkGray);
    drawReadableText(painter,QRectF(-30, 25, 60, 15), Qt::AlignCenter, propagationDelay);
}

void OrGate::process() {
    if (inA->isFloating() || inB->isFloating() || inA->isUndefinedState() || inB->isUndefinedState()) {
        outY->setUndefined();
        return;
    }

    bool result = inA->getLogicState() || inB->getLogicState(); // 💡 برای گیت OR اینجا را || کن

    // 🛠️ سیستم تأخیر انتشار (Propagation Delay Event Queue)
    double delayNs = propagationDelay.replace("ns", "").toDouble();
    int requiredTicks = qMax(1, (int)(delayNs / 10.0)); // هر تیک شبیه‌ساز را ۱۰ نانوثانیه فرض می‌کنیم

    if (result != targetState) {
        targetState = result;
        delayTicks = requiredTicks; // استارت تایمر تأخیر
    }

    if (delayTicks > 0) {
        delayTicks--; // شمارش معکوس تا اعمال تغییر
    }

    if (delayTicks == 0) {
        // زمان تأخیر تمام شد، ولتاژ جدید اعمال می‌شود
        outY->setVoltage(targetState ? highVoltage.replace("V", "").toDouble() : 0.0);
        delayTicks = -1;
    }
}
QMap<QString, QString> OrGate::getProperties() const {
    QMap<QString, QString> props;
    props["Propagation Delay"] = propagationDelay;
    props["Logic HIGH (V)"] = highVoltage;
    return props;
}

void OrGate::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
    if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
}

// ============================================================================
// پیاده‌سازی گیت NOT
// ============================================================================
NotGate::NotGate() {
    inA = new Terminal(this); inA->setPos(-25, 0);
    outY = new Terminal(this); outY->setPos(25, 0);
}

QRectF NotGate::boundingRect() const { return QRectF(-30, -25, 60, 65); }

void NotGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-25, 0, -15, 0);
    painter->drawLine(15, 0, 25, 0);

    painter->setBrush(QColor(240, 240, 240));
    QPainterPath path;
    path.moveTo(-15, -15);
    path.lineTo(7, 0);
    path.lineTo(-15, 15);
    path.closeSubpath();
    painter->drawPath(path);
    painter->drawEllipse(7, -4, 8, 8); // دایره نات خروجی

    painter->setFont(QFont("Consolas", 6));
    painter->setPen(Qt::darkGray);
    drawReadableText(painter,QRectF(-25, 25, 50, 15), Qt::AlignCenter, propagationDelay);
}

void NotGate::process() {
    if (inA->isFloating() || inA->isUndefinedState()) {
        outY->setUndefined();
        return;
    }

    bool result = ! inA->getLogicState();

    // 🛠️ سیستم تأخیر انتشار (Propagation Delay Event Queue)
    double delayNs = propagationDelay.replace("ns", "").toDouble();
    int requiredTicks = qMax(1, (int)(delayNs / 10.0)); // هر تیک شبیه‌ساز را ۱۰ نانوثانیه فرض می‌کنیم

    if (result != targetState) {
        targetState = result;
        delayTicks = requiredTicks; // استارت تایمر تأخیر
    }

    if (delayTicks > 0) {
        delayTicks--; // شمارش معکوس تا اعمال تغییر
    }

    if (delayTicks == 0) {
        // زمان تأخیر تمام شد، ولتاژ جدید اعمال می‌شود
        outY->setVoltage(targetState ? highVoltage.replace("V", "").toDouble() : 0.0);
        delayTicks = -1;
    }
}

QMap<QString, QString> NotGate::getProperties() const {
    QMap<QString, QString> props;
    props["Propagation Delay"] = propagationDelay;
    props["Logic HIGH (V)"] = highVoltage;
    return props;
}

void NotGate::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
    if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
}

// ============================================================================
// پیاده‌سازی گیت XOR
// ============================================================================
XorGate::XorGate() {
    inA = new Terminal(this); inA->setPos(-35, -10);
    inB = new Terminal(this); inB->setPos(-35, 10);
    outY = new Terminal(this); outY->setPos(35, 0);
}

QRectF XorGate::boundingRect() const { return QRectF(-40, -25, 80, 65); }

void XorGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-35, -10, -12, -10);
    painter->drawLine(-35, 10, -12, 10);
    painter->drawLine(23, 0, 35, 0);

    // کمان پشت گیت XOR
    QPainterPath backArc;
    backArc.moveTo(-22, -20);
    backArc.quadTo(QPointF(-12, 0), QPointF(-22, 20));
    painter->drawPath(backArc);

    painter->setBrush(QColor(240, 240, 240));
    QPainterPath path;
    path.moveTo(-17, -20);
    path.quadTo(QPointF(-7, -20), QPointF(23, 0));
    path.quadTo(QPointF(-7, 20), QPointF(-17, 20));
    path.quadTo(QPointF(-7, 0), QPointF(-17, -20));
    painter->drawPath(path);

    painter->setFont(QFont("Consolas", 6));
    painter->setPen(Qt::darkGray);
    drawReadableText(painter,QRectF(-35, 25, 70, 15), Qt::AlignCenter, propagationDelay);
}

void XorGate::process() {
    if (inA->isFloating() || inB->isFloating() || inA->isUndefinedState() || inB->isUndefinedState()) {
        outY->setUndefined();
        return;
    }

    bool result = inA->getLogicState() ^ inB->getLogicState();

    // 🛠️ سیستم تأخیر انتشار (Propagation Delay Event Queue)
    double delayNs = propagationDelay.replace("ns", "").toDouble();
    int requiredTicks = qMax(1, (int)(delayNs / 10.0)); // هر تیک شبیه‌ساز را ۱۰ نانوثانیه فرض می‌کنیم

    if (result != targetState) {
        targetState = result;
        delayTicks = requiredTicks; // استارت تایمر تأخیر
    }

    if (delayTicks > 0) {
        delayTicks--; // شمارش معکوس تا اعمال تغییر
    }

    if (delayTicks == 0) {
        // زمان تأخیر تمام شد، ولتاژ جدید اعمال می‌شود
        outY->setVoltage(targetState ? highVoltage.replace("V", "").toDouble() : 0.0);
        delayTicks = -1;
    }
}

QMap<QString, QString> XorGate::getProperties() const {
    QMap<QString, QString> props;
    props["Propagation Delay"] = propagationDelay;
    props["Logic HIGH (V)"] = highVoltage;
    return props;
}

void XorGate::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
    if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
}

// ============================================================================
// پیاده‌سازی گیت NAND
// ============================================================================
NandGate::NandGate() {
    inA = new Terminal(this); inA->setPos(-35, -10);
    inB = new Terminal(this); inB->setPos(-35, 10);
    outY = new Terminal(this); outY->setPos(35, 0);
}

QRectF NandGate::boundingRect() const { return QRectF(-40, -25, 80, 65); }

void NandGate::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawLine(-35, -10, -20, -10);
    painter->drawLine(-35, 10, -20, 10);
    painter->drawLine(25, 0, 35, 0);

    painter->setBrush(QColor(240, 240, 240));
    QPainterPath path;
    path.moveTo(-20, -20);
    path.lineTo(-5, -20);
    path.arcTo(QRectF(-25, -20, 40, 40), 90, -180);
    path.lineTo(-20, 20);
    path.closeSubpath();
    painter->drawPath(path);
    painter->drawEllipse(15, -4, 8, 8); // حبّاب نات خروجی NAND

    painter->setFont(QFont("Consolas", 6));
    painter->setPen(Qt::darkGray);
    drawReadableText(painter,QRectF(-35, 25, 70, 15), Qt::AlignCenter, propagationDelay);
}

void NandGate::process() {
    if (inA->isFloating() || inB->isFloating() || inA->isUndefinedState() || inB->isUndefinedState()) {
        outY->setUndefined();
        return;
    }

    bool result = !(inA->getLogicState() && inB->getLogicState());

    // 🛠️ سیستم تأخیر انتشار (Propagation Delay Event Queue)
    double delayNs = propagationDelay.replace("ns", "").toDouble();
    int requiredTicks = qMax(1, (int)(delayNs / 10.0)); // هر تیک شبیه‌ساز را ۱۰ نانوثانیه فرض می‌کنیم

    if (result != targetState) {
        targetState = result;
        delayTicks = requiredTicks; // استارت تایمر تأخیر
    }

    if (delayTicks > 0) {
        delayTicks--; // شمارش معکوس تا اعمال تغییر
    }

    if (delayTicks == 0) {
        // زمان تأخیر تمام شد، ولتاژ جدید اعمال می‌شود
        outY->setVoltage(targetState ? highVoltage.replace("V", "").toDouble() : 0.0);
        delayTicks = -1;
    }
}

QMap<QString, QString> NandGate::getProperties() const {
    QMap<QString, QString> props;
    props["Propagation Delay"] = propagationDelay;
    props["Logic HIGH (V)"] = highVoltage;
    return props;
}

void NandGate::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
    if (props.contains("Logic HIGH (V)")) highVoltage = props["Logic HIGH (V)"];
}

// ============================================================================
// پیاده‌سازی فلیپ‌فلاپ D
// ============================================================================
DFlipFlop::DFlipFlop() {
    inD = new Terminal(this); inD->setPos(-40, -15);
    clk = new Terminal(this); clk->setPos(-40, 15);
    outQ = new Terminal(this); outQ->setPos(40, -15);
    outQBar = new Terminal(this); outQBar->setPos(40, 15);
}

QRectF DFlipFlop::boundingRect() const { return QRectF(-45, -50, 90, 100); }

void DFlipFlop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->drawRect(-30, -35, 60, 70);

    // سیم‌های اتصالی پایه‌ها
    painter->drawLine(-40, -15, -30, -15);
    painter->drawLine(-40, 15, -30, 15);
    painter->drawLine(30, -15, 40, -15);
    painter->drawLine(30, 15, 40, 15);

    // لیبل‌های داخلی پایه‌ها
    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    drawReadableText(painter,QRectF(-25, -23, 20, 15), Qt::AlignLeft, "D");
    drawReadableText(painter,QRectF(10, -23, 20, 15), Qt::AlignRight, "Q");
    drawReadableText(painter,QRectF(5, 7, 25, 15), Qt::AlignRight, "Q'");

    // رسم نماد کلاک (مثلث لبه)
    painter->drawLine(-30, 7, -23, 15);
    painter->drawLine(-23, 15, -30, 23);

    painter->setFont(QFont("Consolas", 6));
    painter->setPen(Qt::darkGray);
    drawReadableText(painter,QRectF(-40, 40, 80, 15), Qt::AlignCenter, propagationDelay);
}

void DFlipFlop::process() {
    if (inD->isFloating() || clk->isFloating()) {
        outQ->setUndefined();
        outQBar->setUndefined();
        qWarning() << "DRC Warning: Floating input detected on D-FlipFlop!";
        return;
    }
    if (inD->isUndefinedState() || clk->isUndefinedState()) {
        outQ->setUndefined();
        outQBar->setUndefined();
        return;
    }

    bool currentClockState = clk->getLogicState();
    // تشخیص لبه بالارونده کلاک
    if (currentClockState && !lastClockState) {
        currentInternalQ = inD->getLogicState();
    }
    lastClockState = currentClockState;

    outQ->setVoltage(currentInternalQ ? 5.0 : 0.0);
    outQBar->setVoltage(!currentInternalQ ? 5.0 : 0.0);
}

QMap<QString, QString> DFlipFlop::getProperties() const {
    QMap<QString, QString> props;
    props["Propagation Delay"] = propagationDelay;
    props["Initial Q State (0/1)"] = initialQState;
    return props;
}

void DFlipFlop::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Propagation Delay")) propagationDelay = props["Propagation Delay"];
    if (props.contains("Initial Q State (0/1)")) initialQState = props["Initial Q State (0/1)"];
}
QJsonObject DFlipFlop::getDynamicState() const {
    QJsonObject state;
    state["q"] = currentInternalQ;
    state["last_clock"] = lastClockState;
    state["target"] = targetState;
    state["delay_ticks"] = delayTicks;
    return state;
}
void DFlipFlop::setDynamicState(const QJsonObject& state) {
    currentInternalQ = state["q"].toBool();
    lastClockState = state["last_clock"].toBool();
    targetState = state["target"].toBool();
    delayTicks = state["delay_ticks"].toInt(-1);
}
void DFlipFlop::resetSimulationState() {
    Element::resetSimulationState();
    currentInternalQ = initialQState == "1";
    lastClockState = false;
    targetState = currentInternalQ;
    delayTicks = -1;
}
