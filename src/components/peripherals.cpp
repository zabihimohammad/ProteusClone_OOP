#include "peripherals.h"
#include <QPainter>
#include <QFont>
#include <QJsonArray>
#include "../core/terminal.h"
#include "../core/wire.h"
#include <qmath.h>

// ۱. حافظه خارجی (RAM/EEPROM)
MemoryChip::MemoryChip() {
    // سمت چپ: ۸ پین آدرس (A0 تا A7) و ۱ پین کنترل (WE)
    for (int i = 0; i < 9; i++) {
        (new Terminal(this))->setPos(-50, -40 + (i * 10));
    }
    // سمت راست: ۸ پین دیتا (D0 تا D7)
    for (int i = 0; i < 8; i++) {
        (new Terminal(this))->setPos(50, -35 + (i * 10));
    }
}

QRectF MemoryChip::boundingRect() const { return QRectF(-60, -75, 120, 150); }

void MemoryChip::process() {
    if (childItems().size() < 17) return;

    // استخراج آدرس (A0 تا A7)
    int addr = 0;
    for (int i = 0; i < 8; i++) {
        if (dynamic_cast<Terminal*>(childItems()[i])->getLogicState()) addr |= (1 << i);
    }

    // پین WE (Write Enable)
    Terminal* we = dynamic_cast<Terminal*>(childItems()[8]);
    bool isWrite = we->getLogicState();

    if (isWrite) {
        // خواندن باس ورودی داده و ذخیره در RAM
        int data = 0;
        for (int i = 0; i < 8; i++) {
            if (dynamic_cast<Terminal*>(childItems()[9 + i])->getLogicState()) data |= (1 << i);
        }
        memoryData[addr] = data;
    } else {
        // قرار دادن دیتای RAM روی باس خروجی
        int data = memoryData.value(addr, 0);
        for (int i = 0; i < 8; i++) {
            Terminal* dPin = dynamic_cast<Terminal*>(childItems()[9 + i]);
            dPin->setVoltage((data & (1 << i)) ? 5.0 : 0.0);
        }
    }
}

void MemoryChip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2); if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->setBrush(QColor(50, 50, 50));
    painter->drawRect(-40, -50, 80, 100);

    painter->setPen(Qt::white);
    drawReadableText(painter, QRectF(-40, -50, 80, 100), Qt::AlignCenter, "RAM");

    painter->setPen(pen);
    for (int i = 0; i < 9; i++) painter->drawLine(-50, -40 + (i * 10), -40, -40 + (i * 10));
    for (int i = 0; i < 8; i++) painter->drawLine(40, -35 + (i * 10), 50, -35 + (i * 10));

    painter->setFont(QFont("Consolas", 8, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    painter->drawText(QRectF(-60, -70, 120, 15), Qt::AlignCenter, memorySize);
}

QMap<QString, QString> MemoryChip::getProperties() const {
    QMap<QString, QString> props; props["Memory Size"] = memorySize; props["Storage Firmware (.hex)"] = initialHexPath; return props;
}
void MemoryChip::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Memory Size")) memorySize = props["Memory Size"];
    if (props.contains("Storage Firmware (.hex)")) initialHexPath = props["Storage Firmware (.hex)"];
}
QJsonObject MemoryChip::getDynamicState() const {
    QJsonObject state;
    QJsonArray dataArr;
    for (auto it = memoryData.begin(); it != memoryData.end(); ++it) {
        QJsonObject cell; cell["addr"] = it.key(); cell["val"] = it.value();
        dataArr.append(cell);
    }
    state["ram"] = dataArr;
    return state;
}
void MemoryChip::setDynamicState(const QJsonObject& state) {
    memoryData.clear();
    QJsonArray dataArr = state["ram"].toArray();
    for (int i = 0; i < dataArr.size(); ++i) {
        QJsonObject cell = dataArr[i].toObject();
        memoryData[cell["addr"].toInt()] = cell["val"].toInt();
    }
}
void MemoryChip::resetSimulationState() {
    Element::resetSimulationState();
    memoryData.clear();
}

// ۲. نمایشگر کاراکتری (LCD 16x2)
LCD16x2::LCD16x2() {
    int startX = -55;
    // ایجاد ۱۱ پین: 0=RS, 1=RW, 2=E, 3تا10=D0-D7
    for(int i = 0; i < 11; i++) {
        (new Terminal(this))->setPos(startX + (i * 10), -35);
    }
}

QRectF LCD16x2::boundingRect() const { return QRectF(-70, -55, 140, 90); }

void LCD16x2::process() {
    if (childItems().size() < 11) return;

    Terminal* rs = dynamic_cast<Terminal*>(childItems()[0]);
    Terminal* e = dynamic_cast<Terminal*>(childItems()[2]);
    bool currentE = e->getLogicState();

    // تشخیص لبه بالارونده Enable برای خواندن داده‌ها
    if (currentE && !lastE) {
        int data = 0;
        for (int i = 0; i < 8; i++) {
            if (dynamic_cast<Terminal*>(childItems()[3 + i])->getLogicState()) data |= (1 << i);
        }

        if (rs->getLogicState() == false) { // Instruction Command
            if (data == 0x01) { // Clear Display
                displayText[0] = ""; displayText[1] = ""; cursorCol = 0; cursorRow = 0;
            }
            else if (data == 0xC0) { cursorRow = 1; cursorCol = 0; } // Line 2
        } else { // Data (Character)
            if (cursorRow < 2 && cursorCol < 16) {
                if (displayText[cursorRow].length() <= cursorCol) {
                    displayText[cursorRow] = displayText[cursorRow].leftJustified(cursorCol, ' ') + QChar(data);
                } else {
                    displayText[cursorRow][cursorCol] = QChar(data);
                }
                cursorCol++;
            }
        }
        update(); // رفرش گرافیکی برای رندر کاراکتر جدید
    }
    lastE = currentE;
}

void LCD16x2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2); if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    painter->setBrush(QColor(34, 139, 34)); // قاب دور
    painter->drawRect(-60, -25, 120, 50);

    painter->setBrush(QColor(152, 251, 152)); // نمایشگر
    painter->drawRect(-50, -15, 100, 30);

    int startX = -55;
    for(int i=0; i<11; i++) painter->drawLine(startX + (i*10), -25, startX + (i*10), -35);

    // متن LCD را رسم کن.
    painter->setFont(QFont("Consolas", 10, QFont::Bold));
    painter->setPen(Qt::black);
    drawReadableText(painter, QRectF(-48, -13, 96, 14), Qt::AlignLeft | Qt::AlignVCenter, displayText[0]);
    drawReadableText(painter, QRectF(-48, 1, 96, 14), Qt::AlignLeft | Qt::AlignVCenter, displayText[1]);

    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    drawReadableText(painter, QRectF(-70, 30, 140, 15), Qt::AlignCenter, "LCD 16x2");
}

QMap<QString, QString> LCD16x2::getProperties() const {
    QMap<QString, QString> props; props["Bus Interface Mode"] = busMode; return props;
}
void LCD16x2::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Bus Interface Mode")) busMode = props["Bus Interface Mode"];
}
QJsonObject LCD16x2::getDynamicState() const {
    QJsonObject state;
    state["line_1"] = displayText[0];
    state["line_2"] = displayText[1];
    state["cursor_row"] = cursorRow;
    state["cursor_column"] = cursorCol;
    state["last_enable"] = lastE;
    return state;
}
void LCD16x2::setDynamicState(const QJsonObject& state) {
    displayText[0] = state["line_1"].toString();
    displayText[1] = state["line_2"].toString();
    cursorRow = state["cursor_row"].toInt();
    cursorCol = state["cursor_column"].toInt();
    lastE = state["last_enable"].toBool();
    update();
}
void LCD16x2::resetSimulationState() {
    Element::resetSimulationState();
    displayText[0].clear();
    displayText[1].clear();
    cursorRow = 0;
    cursorCol = 0;
    lastE = false;
    update();
}

// ۳. صفحه کلید ماتریسی (Keypad 4x4)
Keypad::Keypad() {
    // ۴ پین سطر (راست)
    for(int i = 0; i < 4; i++) (new Terminal(this))->setPos(45, -25 + (i * 15));
    // ۴ پین ستون (پایین)
    for(int i = 0; i < 4; i++) (new Terminal(this))->setPos(-25 + (i * 15), 45);
}

QRectF Keypad::boundingRect() const { return QRectF(-45, -50, 90, 120); }

void Keypad::process() {
    if (childItems().size() < 8) return;

    // سطر و ستون دکمه فشرده را وصل کن.
    if (pressedRow != -1 && pressedCol != -1) {
        Terminal* rowPin = dynamic_cast<Terminal*>(childItems()[pressedRow]);
        Terminal* colPin = dynamic_cast<Terminal*>(childItems()[4 + pressedCol]);
        if (rowPin && colPin) {
            if (rowPin->getVoltage() > 2.5) colPin->setVoltage(rowPin->getVoltage());
            else if (colPin->getVoltage() > 2.5) rowPin->setVoltage(colPin->getVoltage());
        }
    }
}

void Keypad::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF p = event->pos();
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                QRectF keyRect(-30 + (c * 15), -32 + (r * 15), 12, 12);
                if (keyRect.contains(p)) {
                    pressedRow = r; pressedCol = c;
                    update();
                    return;
                }
            }
        }
    }
    Element::mousePressEvent(event);
}

void Keypad::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        pressedRow = -1; pressedCol = -1;
        update();
    }
    Element::mouseReleaseEvent(event);
}

void Keypad::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2); if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->setBrush(QColor(200, 200, 200));
    painter->drawRect(-35, -45, 70, 75);

    QString keys[4][4] = { {"7","8","9","/"}, {"4","5","6","*"}, {"1","2","3","-"}, {"C","0","=","+"} };

    for(int r = 0; r < 4; r++) {
        for(int c = 0; c < 4; c++) {
            QRectF keyRect(-30 + (c * 15), -32 + (r * 15), 12, 12);
            painter->setBrush((pressedRow == r && pressedCol == c) ? Qt::gray : Qt::white);
            painter->drawRect(keyRect);
            painter->setPen(Qt::black);
            painter->setFont(QFont("Consolas", 6, QFont::Bold));
            drawReadableText(painter, keyRect, Qt::AlignCenter, keys[r][c]);
        }
    }

    painter->setPen(pen);
    for(int i = 0; i < 4; i++) {
        painter->drawLine(35, -25 + (i * 15), 45, -25 + (i * 15)); // Row Pins
        painter->drawLine(-25 + (i * 15), 30, -25 + (i * 15), 45); // Col Pins
    }
}

QMap<QString, QString> Keypad::getProperties() const {
    QMap<QString, QString> props; props["Debounce Delay"] = debounceTimeMs; return props;
}
void Keypad::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Debounce Delay")) debounceTimeMs = props["Debounce Delay"];
}
// ۴. مبدل آنالوگ به دیجیتال (ADC)
ADC_Chip::ADC_Chip() {
    (new Terminal(this))->setPos(-40, 0); // Vin
    for(int i = -20; i <= 20; i += 10) {
        (new Terminal(this))->setPos(40, i); // Digital Out
    }
}

QRectF ADC_Chip::boundingRect() const {
    return QRectF(-65, -55, 120, 110);
}

void ADC_Chip::process() {
    Terminal *vinTerm = dynamic_cast<Terminal*>(childItems()[0]); // اولین فرزند پین ورودی است
    if (!vinTerm) return;

    double vin = vinTerm->getVoltage();
    double vref = referenceVoltage.replace("V", "").toDouble();
    int bits = resolutionBits.split("-")[0].toInt(); // استخراج عدد 10 یا 8 از رشته "10-Bit"

    // کلمپ کردن ولتاژ بین 0 و Vref
    vin = qBound(0.0, vin, vref);

    // محاسبه مقدار دیجیتال
    int maxDigitalValue = (1 << bits) - 1; // 2^N - 1
    int digitalOut = qRound((vin / vref) * maxDigitalValue);

    // ارسال بیت‌ها به پین‌های خروجی (پین‌های ایندکس 1 به بعد)
    for (int i = 0; i < bits && (i + 1) < childItems().size(); ++i) {
        Terminal *outTerm = dynamic_cast<Terminal*>(childItems()[i + 1]);
        if (outTerm) {
            bool bitHigh = (digitalOut >> i) & 1;
            outTerm->setVoltage(bitHigh ? 5.0 : 0.0);
        }
    }
}

void ADC_Chip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // بدنه مبدل
    painter->setBrush(QColor(70, 130, 180));
    painter->drawRect(-30, -30, 60, 60);
    painter->setPen(Qt::white);
    drawReadableText(painter, QRectF(-30, -30, 60, 60), Qt::AlignCenter, "ADC");

    // پایه ورودی آنالوگ
    painter->setPen(pen);
    painter->drawLine(-40, 0, -30, 0);
    drawReadableText(painter, -55, 5, "Vin");

    // پایه‌های خروجی دیجیتال
    for(int i = -20; i <= 20; i += 10) {
        painter->drawLine(30, i, 40, i);
    }

    // مشخصات ADC
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    drawReadableText(painter, QRectF(-65, -50, 120, 15), Qt::AlignCenter, resolutionBits);
    drawReadableText(painter, QRectF(-65, 35, 120, 15), Qt::AlignCenter, "Vref: " + referenceVoltage);
}

// ۵. مبدل دیجیتال به آنالوگ (DAC)
DAC_Chip::DAC_Chip() {
    (new Terminal(this))->setPos(40, 0); // Vout
    for(int i = -20; i <= 20; i += 10) {
        (new Terminal(this))->setPos(-40, i); // Digital In
    }
}

QRectF DAC_Chip::boundingRect() const {
    return QRectF(-65, -55, 120, 110);
}

void DAC_Chip::process() {
    double vref = maxOutputVoltage.replace("V", "").toDouble();
    int bits = resolutionBits.split("-")[0].toInt();
    int digitalIn = 0;

    // خواندن بیت‌های ورودی (پین‌های ایندکس 1 به بعد)
    for (int i = 0; i < bits && (i + 1) < childItems().size(); ++i) {
        Terminal *inTerm = dynamic_cast<Terminal*>(childItems()[i + 1]);
        if (inTerm && inTerm->getLogicState()) {
            digitalIn |= (1 << i);
        }
    }

    // محاسبه ولتاژ آنالوگ خروجی
    int maxDigitalValue = (1 << bits) - 1;
    double vout = ((double)digitalIn / maxDigitalValue) * vref;

    // اعمال به پین خروجی (اولین فرزند)
    Terminal *voutTerm = dynamic_cast<Terminal*>(childItems()[0]);
    if (voutTerm) {
        voutTerm->setVoltage(vout);
    }
}

void DAC_Chip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // بدنه مبدل
    painter->setBrush(QColor(210, 105, 30));
    painter->drawRect(-30, -30, 60, 60);
    painter->setPen(Qt::white);
    drawReadableText(painter, QRectF(-30, -30, 60, 60), Qt::AlignCenter, "DAC");

    // پایه خروجی آنالوگ
    painter->setPen(pen);
    painter->drawLine(30, 0, 40, 0);
    drawReadableText(painter, 45, 5, "Vout");

    // پایه‌های ورودی دیجیتال
    for(int i = -20; i <= 20; i += 10) {
        painter->drawLine(-40, i, -30, i);
    }

    // مشخصات DAC
    painter->setFont(QFont("Consolas", 7, QFont::Bold));
    painter->setPen(Qt::darkBlue);
    drawReadableText(painter, QRectF(-65, -50, 120, 15), Qt::AlignCenter, resolutionBits);
    drawReadableText(painter, QRectF(-65, 35, 120, 15), Qt::AlignCenter, "Max: " + maxOutputVoltage);
}
