#include "mcu.h"
#include <QPainter>
#include <QFont> // برای تنظیم فونت نوشته‌ها
#include <QFile>
#include <QJsonArray>
#include "../core/terminal.h"

// ==========================================
// سازنده (Constructor) میکروکنترلر
// ==========================================
MCUChip::MCUChip() {
    // ایجاد پایه‌ها (پین‌ها) در ۴ طرف میکروکنترلر (شبیه پکیج‌های QFP یا DIP)
    for (int i = -20; i <= 20; i += 20) {
        (new Terminal(this))->setPos(-50, i); // پایه‌های سمت چپ
        (new Terminal(this))->setPos(50, i);  // پایه‌های سمت راست
        (new Terminal(this))->setPos(i, -50); // پایه‌های بالا
        (new Terminal(this))->setPos(i, 50);  // پایه‌های پایین
    }
}

// ==========================================
// تعیین محدوده کلیک و رسم قطعه
// ==========================================
QRectF MCUChip::boundingRect() const {
    return QRectF(-60, -60, 120, 120);
}
void MCUChip::setProperties(const QMap<QString, QString>& props) {
    if (props.contains("Clock Frequency")) clockFrequency = props["Clock Frequency"];
    if (props.contains("Hex File Path")) {
        QString newPath = props["Hex File Path"];
        if (newPath != hexFilePath && !newPath.isEmpty() && newPath != "Not Loaded") {
            hexFilePath = newPath;
            loadHexFile(hexFilePath); // 🛠️ لود کردن اتوماتیک فایل هنگام تغییر آدرس
        }
    }
}

// 🛠️ مفسر استاندارد فایل Intel HEX
void MCUChip::loadHexFile(const QString& path) {
    rom.clear();
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith(":")) {
                int byteCount = line.mid(1, 2).toInt(nullptr, 16);
                int addr = line.mid(3, 4).toInt(nullptr, 16);
                int type = line.mid(7, 2).toInt(nullptr, 16);
                if (type == 0) { // رکورد داده (Data Record)
                    for (int i = 0; i < byteCount; i++) {
                        rom[addr + i] = line.mid(9 + i * 2, 2).toInt(nullptr, 16);
                    }
                }
            }
        }
    }
}

void MCUChip::process() {
    if (rom.isEmpty()) return; // اگر فریمور لود نشده بود کاری نکن

    // 🛠️ واکشی (Fetch) کد باینری از حافظه رام
    uint8_t opcode = rom.value(PC, 0x00);

    // 🛠️ رمزگشایی و اجرا (Decode & Execute) - دستورات پایه‌ای ساده‌سازی شده
    if (opcode == 0x74) { // MOV A, #data
        accumulator = rom.value(PC + 1, 0);
        PC += 2;
    } else if (opcode == 0x24) { // ADD A, #data
        accumulator += rom.value(PC + 1, 0);
        PC += 2;
    } else if (opcode == 0x02) { // JMP address
        PC = (rom.value(PC + 1, 0) << 8) | rom.value(PC + 2, 0);
    } else if (opcode == 0xD2) { // SETB P1.x (شبیه‌سازی روشن کردن پایه)
        int bit = rom.value(PC + 1, 0);
        if (bit < childItems().size()) {
            if (auto* t = dynamic_cast<Terminal*>(childItems()[bit])) t->setVoltage(5.0);
        }
        PC += 2;
    } else if (opcode == 0xC2) { // CLR P1.x (شبیه‌سازی خاموش کردن پایه)
        int bit = rom.value(PC + 1, 0);
        if (bit < childItems().size()) {
            if (auto* t = dynamic_cast<Terminal*>(childItems()[bit])) t->setVoltage(0.0);
        }
        PC += 2;
    } else {
        PC++; // NOP یا رد شدن از دستور ناشناخته
    }
}

QJsonObject MCUChip::getDynamicState() const {
    QJsonObject state;
    state["program_counter"] = PC;
    state["accumulator"] = accumulator;

    QJsonArray ramData;
    for (auto it = RAM.constBegin(); it != RAM.constEnd(); ++it) {
        QJsonObject cell;
        cell["address"] = it.key();
        cell["value"] = it.value();
        ramData.append(cell);
    }
    state["ram"] = ramData;
    return state;
}

void MCUChip::setDynamicState(const QJsonObject& state) {
    PC = state["program_counter"].toInt();
    accumulator = state["accumulator"].toInt();
    RAM.clear();
    for (const QJsonValue& value : state["ram"].toArray()) {
        QJsonObject cell = value.toObject();
        RAM[cell["address"].toInt()] = cell["value"].toInt();
    }
}

void MCUChip::resetSimulationState() {
    Element::resetSimulationState();
    PC = 0;
    accumulator = 0;
    RAM.clear();
}
// ==========================================
// رسم گرافیکی میکروکنترلر روی بوم
// ==========================================
void MCUChip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(Qt::black, 2);
    if (isSelected()) pen.setColor(Qt::red);
    painter->setPen(pen);

    // ۱. رسم بدنه اصلی IC
    painter->setBrush(QColor(40, 40, 40)); // رنگ خاکستری تیره (شبیه پلاستیک IC)
    painter->drawRect(-40, -40, 80, 80);

    // ۲. رسم نقطه راهنمای پایه شماره 1 (بالا سمت چپ)
    painter->setBrush(Qt::white);
    painter->drawEllipse(-30, -30, 5, 5);

    // ۳. رسم پایه‌های فلزی (Pins)
    painter->setPen(pen);
    for (int i = -20; i <= 20; i += 20) {
        painter->drawLine(-50, i, -40, i); // پایه‌های چپ
        painter->drawLine(40, i, 50, i);   // پایه‌های راست
        painter->drawLine(i, -50, i, -40); // پایه‌های بالا
        painter->drawLine(i, 40, i, 50);   // پایه‌های پایین
    }

    // ==========================================
    // ۴. نمایش اطلاعات داینامیک روی بدنه تراشه
    // ==========================================

    // چاپ نام تراشه
    painter->setPen(Qt::white);
    painter->setFont(QFont("Consolas", 10, QFont::Bold));
    drawReadableText(painter,QRectF(-40, -30, 80, 20), Qt::AlignCenter, "MCU");

    // چاپ فرکانس کاری
    painter->setFont(QFont("Consolas", 7));
    painter->setPen(QColor(200, 200, 200)); // خاکستری روشن
    painter->drawText(QRectF(-40, -10, 80, 20), Qt::AlignCenter, clockFrequency);

    // بررسی وضعیت فایل هگز و نمایش آن با رنگ مناسب
    QString hexStatusText;
    if (hexFilePath == "Not Loaded" || hexFilePath.isEmpty()) {
        painter->setPen(QColor(255, 100, 100)); // رنگ قرمز (هشدار: کد ندارد)
        hexStatusText = "NO HEX";
    } else {
        painter->setPen(QColor(100, 255, 100)); // رنگ سبز (آماده اجرا)
        hexStatusText = "HEX OK";
    }
    painter->drawText(QRectF(-40, 10, 80, 20), Qt::AlignCenter, hexStatusText);
}
