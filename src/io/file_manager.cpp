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

// =========================================
// توابع کمکی برای پیدا کردن شماره (Index) پایه‌ها
// =========================================
static int getTerminalIndex(Element* el, Terminal* target)
{
    int index = 0;
    for (QGraphicsItem* child : el->childItems())
    {
        if (Terminal* term = dynamic_cast<Terminal*>(child))
        {
            if (term == target) return index;
            index++;
        }
    }
    return -1;
}

static Terminal* getTerminalByIndex(Element* el, int index)
{
    int currentIndex = 0;
    for (QGraphicsItem* child : el->childItems())
    {
        if (Terminal* term = dynamic_cast<Terminal*>(child))
        {
            if (currentIndex == index) return term;
            currentIndex++;
        }
    }
    return nullptr;
}

// =========================================
// تابع کارخانه (Factory)
// =========================================
static Element* createComponent(const QString &type) {
    if (type == "Microcontroller (MCU)") return new MCUChip();
    if (type == "Resistor") return new Resistor();
    if (type == "Capacitor") return new Capacitor();
    if (type == "LED") return new LED();
    if (type == "7-Segment Display") return new SevenSegment();
    if (type == "AND Gate") return new AndGate();
    if (type == "OR Gate") return new OrGate();
    if (type == "External Memory Chip") return new MemoryChip();
    return nullptr;
}

// =========================================
// توابع ذخیره‌سازی (Save)
// =========================================
bool FileManager::saveCircuit(const QString &filePath, QGraphicsScene *scene)
{
    if (!scene || filePath.isEmpty())
        return false;

    // ۱. استخراج تمام قطعات به یک لیست مشخص تا ترتیبشان حفظ شود (برای ID گذاری)
    QList<Element*> elementsList;
    for (QGraphicsItem *item : scene->items())
    {
        if (Element *el = dynamic_cast<Element*>(item))
        {
            elementsList.append(el);
        }
    }

    QJsonObject rootObject;
    rootObject["elements"] = serializeElements(elementsList);
    rootObject["wires"] = serializeWires(scene, elementsList);

    QJsonDocument doc(rootObject);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for saving:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out << doc.toJson(QJsonDocument::Indented);
    file.close();
    qDebug() << "Circuit successfully saved to" << filePath;
    return true;
}

QJsonArray FileManager::serializeElements(const QList<Element*> &elementsList)
{
    QJsonArray elementsArray;
    for (Element *element : elementsList)
    {
        QJsonObject elementObj;
        elementObj["type"] = element->getComponentName();

        QJsonObject posObj;
        posObj["x"] = element->scenePos().x();
        posObj["y"] = element->scenePos().y();
        elementObj["position"] = posObj;

        QJsonObject propsObj;
        QMap<QString, QString> props = element->getProperties();
        for (auto it = props.constBegin(); it != props.constEnd(); ++it)
        {
            propsObj[it.key()] = it.value();
        }
        elementObj["properties"] = propsObj;
        elementsArray.append(elementObj);
    }
    return elementsArray;
}

QJsonArray FileManager::serializeWires(QGraphicsScene *scene, const QList<Element*> &elementsList)
{
    QJsonArray wiresArray;
    for (QGraphicsItem *item : scene->items())
    {
        if (Wire *wire = dynamic_cast<Wire*>(item))
        {
            Terminal *startTerm = wire->getStartTerminal();
            Terminal *endTerm = wire->getEndTerminal();

            if (!startTerm || !endTerm) continue; // سیم‌های ناقص را ذخیره نمی‌کنیم

            Element *startEl = dynamic_cast<Element*>(startTerm->parentItem());
            Element *endEl = dynamic_cast<Element*>(endTerm->parentItem());
            if (!startEl || !endEl)
                continue;

            int startElIndex = elementsList.indexOf(startEl);
            int endElIndex = elementsList.indexOf(endEl);

            if (startElIndex == -1 || endElIndex == -1)
                continue;

            QJsonObject wireObj;

            QJsonObject startObj;
            startObj["element_index"] = startElIndex;
            startObj["terminal_index"] = getTerminalIndex(startEl, startTerm);

            QJsonObject endObj;
            endObj["element_index"] = endElIndex;
            endObj["terminal_index"] = getTerminalIndex(endEl, endTerm);

            wireObj["start"] = startObj;
            wireObj["end"] = endObj;

            wiresArray.append(wireObj);
        }
    }
    return wiresArray;
}

// =========================================
// توابع بازیابی (Load)
// =========================================
bool FileManager::loadCircuit(const QString &filePath, QGraphicsScene *scene)
{
    if (!scene || filePath.isEmpty())
        return false;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for loading:" << filePath;
        return false;
    }
    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isObject()) return false;

    scene->clear(); // پاک کردن بوم فعلی
    QJsonObject rootObject = doc.object();

    QList<Element*> loadedElements; // لیستی برای نگهداری قطعاتِ لود شده جهت سیم‌کشی

    // ۱. ساخت قطعات
    QJsonArray elementsArray = rootObject["elements"].toArray();
    for (const QJsonValue &value : elementsArray)
    {
        QJsonObject elObj = value.toObject();
        QString type = elObj["type"].toString();

        Element *newElement = createComponent(type);
        if (newElement)
        {
            QJsonObject posObj = elObj["position"].toObject();
            newElement->setPos(posObj["x"].toDouble(), posObj["y"].toDouble());

            QJsonObject propsObj = elObj["properties"].toObject();
            QMap<QString, QString> props;
            for (auto it = propsObj.constBegin(); it != propsObj.constEnd(); ++it)
            {
                props[it.key()] = it.value().toString();
            }
            newElement->setProperties(props);

            scene->addItem(newElement);
            loadedElements.append(newElement); // ثبت قطعه در لیست برای اتصال سیم‌ها
        }
        else
        {
            loadedElements.append(nullptr); // برای حفظ نظم Index ها
        }
    }

    // ۲. ساخت سیم‌ها
    QJsonArray wiresArray = rootObject["wires"].toArray();
    for (const QJsonValue &value : wiresArray) {
        QJsonObject wireObj = value.toObject();
        QJsonObject startObj = wireObj["start"].toObject();
        QJsonObject endObj = wireObj["end"].toObject();
        int startElIdx = startObj["element_index"].toInt(-1);
        int startTermIdx = startObj["terminal_index"].toInt(-1);
        int endElIdx = endObj["element_index"].toInt(-1);
        int endTermIdx = endObj["terminal_index"].toInt(-1);
        if (startElIdx >= 0 && startElIdx < loadedElements.size() &&
            endElIdx >= 0 && endElIdx < loadedElements.size())
        {

            Element *startEl = loadedElements[startElIdx];
            Element *endEl = loadedElements[endElIdx];

            if (startEl && endEl)
            {
                Terminal *startTerm = getTerminalByIndex(startEl, startTermIdx);
                Terminal *endTerm = getTerminalByIndex(endEl, endTermIdx);

                if (startTerm && endTerm)
                {
                    Wire *newWire = new Wire(startTerm, startTerm->scenePos());
                    newWire->confirmConnection(endTerm);
                    scene->addItem(newWire);

                    // شبیه‌سازی زنده: فرمان می‌دهیم تا هوش مصنوعی مسیر را با زاویه ۹۰ درجه رسم کند
                    newWire->updateRoute();
                }
            }
        }
    }

    qDebug() << "Circuit successfully loaded from" << filePath;
    return true;
}