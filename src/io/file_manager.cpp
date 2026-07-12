#include "file_manager.h"
#include "../core/element.h"
#include "../core/wire.h"
#include "../core/terminal.h"
#include "../components/basic_components.h"
#include "../components/logic_gates.h"
#include "../components/mcu.h"
#include "../components/peripherals.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMap>

// مقداردهی اولیه متغیرهای استاتیک (تاریخچه مدار)
QStack<QByteArray> FileManager::undoStack;
QStack<QByteArray> FileManager::redoStack;
QByteArray FileManager::currentState;

// =========================================
// توابع کمکی پیدا کردن شماره (Index)
// =========================================
static int getTerminalIndex(Element* el, Terminal* target) {
    int index = 0;
    for (QGraphicsItem* child : el->childItems()) {
        if (Terminal* term = dynamic_cast<Terminal*>(child)) {
            if (term == target) return index;
            index++;
        }
    }
    return -1;
}

static Terminal* getTerminalByIndex(Element* el, int index) {
    int currentIndex = 0;
    for (QGraphicsItem* child : el->childItems()) {
        if (Terminal* term = dynamic_cast<Terminal*>(child)) {
            if (currentIndex == index) return term;
            currentIndex++;
        }
    }
    return nullptr;
}

// =========================================
// تابع کارخانه (Factory Pattern) - نسخه کامل
// =========================================
static Element* createComponent(const QString &type) {
    if (type == "Microcontroller (MCU)") return new MCUChip();

    // -- قطعات پایه --
    if (type == "Resistor") return new Resistor();
    if (type == "Capacitor") return new Capacitor();
    if (type == "Inductor") return new Inductor();
    if (type == "Pulse Generator") return new PulseGenerator();
    if (type == "Switch") return new Switch();
    if (type == "Push Button") return new PushButton();
    if (type == "LED") return new LED();
    if (type == "7-Segment Display") return new SevenSegment();
    if (type == "Ground (GND)") return new Ground();
    if (type == "DC Voltage Source") return new DCVoltageSource();
    if (type == "Clock Generator") return new ClockGenerator();

    // -- گیت‌های منطقی --
    if (type == "AND Gate") return new AndGate();
    if (type == "OR Gate") return new OrGate();
    if (type == "NOT Gate") return new NotGate();
    if (type == "XOR Gate") return new XorGate();
    if (type == "NAND Gate") return new NandGate();
    if (type == "D-Type Flip-Flop") return new DFlipFlop();

    // -- پریفرال‌ها و آی‌سی‌ها --
    if (type == "External Memory Chip") return new MemoryChip();
    if (type == "LCD 16x2 Display") return new LCD16x2();
    if (type == "Matrix Keypad 4x4") return new Keypad();
    if (type == "Analog to Digital Converter (ADC)") return new ADC_Chip();
    if (type == "Digital to Analog Converter (DAC)") return new DAC_Chip();

    // اگر نام قطعه‌ای در لیست نبود، ارور را در کنسول چاپ کن تا سریع پیدایش کنیم
    qWarning() << "[FileManager] ERROR: Unknown component type in Factory:" << type;
    return nullptr;
}
// ============================================================================
// موتور اصلی سریالایز و دی‌سریالایز (استفاده مشترک برای فایل و Undo/Redo)
// ============================================================================

QByteArray FileManager::captureSceneState(QGraphicsScene *scene) {
    QList<Element*> elementsList;
    for (QGraphicsItem *item : scene->items()) {
        if (Element *el = dynamic_cast<Element*>(item)) {
            elementsList.append(el);
        }
    }

    QJsonObject rootObject;
    rootObject["elements"] = serializeElements(elementsList);
    rootObject["wires"] = serializeWires(scene, elementsList);

    QJsonDocument doc(rootObject);
    return doc.toJson(QJsonDocument::Compact); // فشرده ذخیره می‌کنیم تا رم اشغال نشود
}

void FileManager::restoreSceneState(const QByteArray &stateData, QGraphicsScene *scene) {
    if (stateData.isEmpty()) return;

    QJsonDocument doc = QJsonDocument::fromJson(stateData);
    if (doc.isNull() || !doc.isObject()) return;

    scene->clear();
    QJsonObject rootObject = doc.object();
    QList<Element*> loadedElements;

    // بازیابی قطعات
    QJsonArray elementsArray = rootObject["elements"].toArray();
    for (const QJsonValue &value : elementsArray) {
        QJsonObject elObj = value.toObject();
        Element *newElement = createComponent(elObj["type"].toString());

        if (newElement) {
            QJsonObject posObj = elObj["position"].toObject();
            newElement->setPos(posObj["x"].toDouble(), posObj["y"].toDouble());

            QJsonObject propsObj = elObj["properties"].toObject();
            QMap<QString, QString> props;
            for (auto it = propsObj.constBegin(); it != propsObj.constEnd(); ++it) {
                props[it.key()] = it.value().toString();
            }
            newElement->setProperties(props);

            scene->addItem(newElement);
            loadedElements.append(newElement);
        } else {
            loadedElements.append(nullptr);
        }
    }

    // بازیابی سیم‌ها و مسیریابی خودکار
    QJsonArray wiresArray = rootObject["wires"].toArray();
    for (const QJsonValue &value : wiresArray) {
        QJsonObject wireObj = value.toObject();
        QJsonObject startObj = wireObj["start"].toObject();
        QJsonObject endObj = wireObj["end"].toObject();

        int startElIdx = startObj["element_index"].toInt(-1);
        int startTermIdx = startObj["terminal_index"].toInt(-1);
        int endElIdx = endObj["element_index"].toInt(-1);
        int endTermIdx = endObj["terminal_index"].toInt(-1);

        if (startElIdx >= 0 && endElIdx >= 0) {
            Element *startEl = loadedElements.value(startElIdx, nullptr);
            Element *endEl = loadedElements.value(endElIdx, nullptr);

            if (startEl && endEl) {
                Terminal *startTerm = getTerminalByIndex(startEl, startTermIdx);
                Terminal *endTerm = getTerminalByIndex(endEl, endTermIdx);

                if (startTerm && endTerm) {
                    Wire *newWire = new Wire(startTerm, startTerm->scenePos());
                    newWire->confirmConnection(endTerm);
                    scene->addItem(newWire);
                    newWire->updateRoute();
                }
            }
        }
    }
}

// =========================================
// سیستم کنترل فایل (Save / Load)
// =========================================
bool FileManager::saveCircuit(const QString &filePath, QGraphicsScene *scene) {
    if (!scene || filePath.isEmpty()) return false;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    // تبدیل بوم به JSON ولی این بار با فرمت خوانا (Indented) برای ذخیره در هارد
    QJsonDocument doc = QJsonDocument::fromJson(captureSceneState(scene));
    QTextStream out(&file);
    out << doc.toJson(QJsonDocument::Indented);
    file.close();
    return true;
}

bool FileManager::loadCircuit(const QString &filePath, QGraphicsScene *scene) {
    if (!scene || filePath.isEmpty()) return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QByteArray fileData = file.readAll();
    file.close();

    restoreSceneState(fileData, scene);

    // پاک کردن تاریخچه به خاطر لود شدن مدار کاملاً جدید
    undoStack.clear();
    redoStack.clear();
    currentState = captureSceneState(scene);

    return true;
}

// =========================================
// منطق اصلی Undo / Redo
// =========================================
void FileManager::recordState(QGraphicsScene *scene) {
    if (!currentState.isEmpty()) {
        undoStack.push(currentState);
    }
    currentState = captureSceneState(scene);
    redoStack.clear(); // با انجام یک کار جدید، تاریخچه‌ی آینده پاک می‌شود
    qDebug() << "[History] State recorded. Undo level:" << undoStack.size();
}

void FileManager::undo(QGraphicsScene *scene) {
    if (undoStack.isEmpty()) {
        qDebug() << "[History] Nothing to undo.";
        return;
    }
    redoStack.push(currentState);
    currentState = undoStack.pop();
    restoreSceneState(currentState, scene);
    qDebug() << "[History] Undo performed.";
}

void FileManager::redo(QGraphicsScene *scene) {
    if (redoStack.isEmpty()) {
        qDebug() << "[History] Nothing to redo.";
        return;
    }
    undoStack.push(currentState);
    currentState = redoStack.pop();
    restoreSceneState(currentState, scene);
    qDebug() << "[History] Redo performed.";
}

// =========================================
// استخراج قطعات و سیم‌ها
// =========================================
QJsonArray FileManager::serializeElements(const QList<Element*> &elementsList) {
    QJsonArray elementsArray;
    for (Element *element : elementsList) {
        QJsonObject elementObj;
        elementObj["type"] = element->getComponentName();

        QJsonObject posObj;
        posObj["x"] = element->scenePos().x();
        posObj["y"] = element->scenePos().y();
        elementObj["position"] = posObj;

        QJsonObject propsObj;
        QMap<QString, QString> props = element->getProperties();
        for (auto it = props.constBegin(); it != props.constEnd(); ++it) {
            propsObj[it.key()] = it.value();
        }
        elementObj["properties"] = propsObj;
        elementsArray.append(elementObj);
    }
    return elementsArray;
}

QJsonArray FileManager::serializeWires(QGraphicsScene *scene, const QList<Element*> &elementsList) {
    QJsonArray wiresArray;
    for (QGraphicsItem *item : scene->items()) {
        if (Wire *wire = dynamic_cast<Wire*>(item)) {
            Terminal *startTerm = wire->getStartTerminal();
            Terminal *endTerm = wire->getEndTerminal();
            if (!startTerm || !endTerm) continue;

            Element *startEl = dynamic_cast<Element*>(startTerm->parentItem());
            Element *endEl = dynamic_cast<Element*>(endTerm->parentItem());
            if (!startEl || !endEl) continue;

            int startElIndex = elementsList.indexOf(startEl);
            int endElIndex = elementsList.indexOf(endEl);
            if (startElIndex == -1 || endElIndex == -1) continue;

            QJsonObject wireObj;
            QJsonObject startObj, endObj;

            startObj["element_index"] = startElIndex;
            startObj["terminal_index"] = getTerminalIndex(startEl, startTerm);
            endObj["element_index"] = endElIndex;
            endObj["terminal_index"] = getTerminalIndex(endEl, endTerm);

            wireObj["start"] = startObj;
            wireObj["end"] = endObj;
            wiresArray.append(wireObj);
        }
    }
    return wiresArray;
}