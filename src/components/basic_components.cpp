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

    // نمایش فرکانس روی قطعه
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    drawReadableText(painter,QRectF(-25, -35, 50, 15), Qt::AlignCenter, frequency);
}

// 🛠️ بخش اصلاح شده: موتور شبیه‌ساز هر 0.1 ثانیه این تابع را صدا می‌زند
void PulseGenerator::process() {
    tickCount++;
    if (tickCount >= 10) { // هر 1 ثانیه (10 تا 100 میلی‌ثانیه)
        tickCount = 0;

        // تغییر وضعیت (Toggle)
        if (currentMockVoltage == "0.0V") {
            currentMockVoltage = "5.0V";
        } else {
            currentMockVoltage = "0.0V";
        }
    }

    // تزریق ولتاژ به ترمینال خروجی
    for (QGraphicsItem *child : childItems()) {
        Terminal *term = dynamic_cast<Terminal*>(child);
        if (term) {
            term->voltageLevel = currentMockVoltage;
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
    drawReadableText(painter,QRectF(-35, -35, 70, 15), Qt::AlignCenter, type);
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
    drawReadableText(painter,QRectF(-35, -35, 70, 15), Qt::AlignCenter, color);
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
    drawReadableText(painter,QRectF(-25, -45, 50, 15), Qt::AlignCenter, color);
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
// ============================================================================
// ۱۲. پیاده‌سازی گره اتصال (Junction Node)
// ============================================================================
JunctionNode::JunctionNode() {
    term = new Terminal(this);
    term->setPos(0, 0); // ترمینال دقیقاً در مرکز گره قرار می‌گیرد
}
JunctionNode::~JunctionNode() {
    if (term) {
        // گرفتن کپی از لیست سیم‌ها چون در حین حذف، لیست اصلی تغییر می‌کند
        QList<Wire*> wiresToDelete = term->getConnectedWires();
        for (Wire* wire : wiresToDelete) {
            if (wire && wire->scene()) {
                wire->scene()->removeItem(wire);
                delete wire; // حذف زنجیره‌ای تکه‌سیم‌های متصل به این گره
            }
        }
    }
}
QRectF JunctionNode::boundingRect() const {
    return QRectF(-6, -6, 12, 12);
}
// 🛠️ مخرب هوشمند جدید برای پاکسازی سیم‌های متصل به گره

void JunctionNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // رسم یک نقطه توپر سیاه رنگ (استاندارد شماتیک برای گره‌ها)
    painter->setBrush(isSelected() ? Qt::red : Qt::black);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-4, -4, 8, 8);
}

void JunctionNode::process() {
    // گره‌ها پردازش منطقی خاصی ندارند، فقط ولتاژ را از سیم‌ها عبور می‌دهند
}
// ==========================================
// ۱۳. پیاده‌سازی اسیلوسکوپ گرافیکی پیشرفته
// ==========================================
Oscilloscope::Oscilloscope() {
    // ایجاد یک پین ورودی در سمت چپ دستگاه برای اتصال سیم مدار
    inChannel = new Terminal(this);
    inChannel->setPos(-80, 0);

    // پر کردن بافر اولیه با ولتاژ صفر
    voltageHistory.fill(0.0, maxSamples);
}

QRectF Oscilloscope::boundingRect() const {
    // کادر کلی اسیلوسکوپ (یک مستطیل عریض آزمایشگاهی)
    return QRectF(-75, -55, 150, 110);
}

void Oscilloscope::process() {
    // خواندن ولتاژ زنده از روی رشته متنی یا متغیر الکتریکی پین ورودی
    double currentV = 0.0;
    if (inChannel) {
        // تبدیل رشته "5.0V" یا مشابه به عدد double برای رسم ریاضی
        QString vStr = inChannel->voltageLevel;
        currentV = vStr.replace("V", "").toDouble();
    }

    // شیفت دادن بافر به چپ و اضافه کردن نمونه جدید از سمت راست (حرکت زمانی موج)
    voltageHistory.pop_front();
    voltageHistory.append(currentV);
}

void Oscilloscope::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // ─── ۱. بدنه فیزیکی دستگاه (تم تیره و مدرن) ───
    QLinearGradient bgGrad(-70, -50, 70, 50);
    bgGrad.setColorAt(0.0, QColor("#2D3540"));
    bgGrad.setColorAt(1.0, QColor("#1A1F26"));
    painter->setBrush(bgGrad);

    QPen bodyPen(isSelected() ? Qt::red : QColor("#4F5D6B"), 2.5);
    painter->setPen(bodyPen);
    painter->drawRoundedRect(-70, -50, 140, 100, 10, 10);

    // ─── ۲. اسکرین تیره دیجیتال (CRT Screen) ───
    QRectF screenRect(-60, -42, 120, 74);
    painter->setBrush(QColor("#0D1117"));
    painter->setPen(QPen(QColor("#30363D"), 1.5));
    painter->drawRoundedRect(screenRect, 4, 4);

    // ─── ۳. رسم گرید شطرنجی فسفری (مختصات اسیلوسکوپ) ───
    QColor gridColor("#00FFCC");
    gridColor.setAlpha(25);
    painter->setPen(QPen(gridColor, 1, Qt::DotLine));
    // خطوط عمودی گرید
    for (int x = -50; x <= 50; x += 15) {
        painter->drawLine(x, -42, x, 32);
    }
    // خطوط افقی گرید
    for (int y = -30; y <= 30; y += 12) {
        painter->drawLine(-60, y, 60, y);
    }

    // ─── ۴. رسم شکل موج ولتاژ به صورت نئونی (Glow Waveform) ───
    QPainterPath wavePath;
    double stepX = 120.0 / (maxSamples - 1);
    double startX = -60.0;

    // تبدیل ولتاژ به مختصات Y صفحه (فرض می‌کنیم محدوده ولتاژ ۰ تا ۵ ولت است)
    auto voltageToY = [&](double v) {
        // ۵ ولت در بالای صفحه و ۰ ولت در پایین صفحه گرید
        double clampedV = qBound(0.0, v, 5.0);
        return 26.0 - (clampedV / 5.0) * 56.0;
    };

    wavePath.moveTo(startX, voltageToY(voltageHistory[0]));
    for (int i = 1; i < maxSamples; ++i) {
        wavePath.lineTo(startX + (i * stepX), voltageToY(voltageHistory[i]));
    }

    // لایه اول: رسم هاله درخشان پشتی (شفافیت بالا و ضخامت بیشتر)
    QColor glowColor("#00FFCC");
    glowColor.setAlpha(60);
    painter->setPen(QPen(glowColor, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(wavePath);

    // لایه دوم: هسته اصلی موج فسفری تیز (ضخامت کمتر و تمام رنگ)
    painter->setPen(QPen(QColor("#FFFFFF"), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(wavePath);

    // ─── ۵. لِیبل‌ها و مقادیر متنی وضعیت ولتاژ زنده ───
    painter->setFont(QFont("Consolas", 7, QFont::Bold));

    // نمایش ولتاژ لحظه‌ای کانال در گوشه بالا سمت چپ اسکرین
    QString liveVText = QString::number(voltageHistory.last(), 'f', 1) + "V";
    painter->setPen(QColor("#00FFCC"));
    painter->drawText(QRectF(-55, -38, 50, 15), Qt::AlignLeft, liveVText);

    // چاپ نام مدل اسیلوسکوپ در پایین بدنه دستگاه
    painter->setPen(QColor("#9AA6B5"));
    painter->setFont(QFont("Segoe UI", 7, QFont::DemiBold));
    drawReadableText(painter, QRectF(-70, 37, 140, 12), Qt::AlignCenter, "DIGITAL OSCILLOSCOPE");

    painter->restore();
}