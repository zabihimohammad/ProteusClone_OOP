#include "file_manager.h"
#include "../core/element.h"
#include "../core/wire.h"
#include "../core/terminal.h"

// =========================================
// هدرهای قطعات (برای ساخته شدن در تابع Load)
// =========================================
#include "../components/basic_components.h"
#include "../components/logic_gates.h"
#include "../components/mcu.h"
#include "../components/peripherals.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMap>

bool FileManager::saveCircuit(const QString &filePath, QGraphicsScene *scene)
{
    if (!scene || filePath.isEmpty())
        return false;
    QJsonObject rootObject;
    // ۱. استخراج و ذخیره تمام قطعات (مقاومت، میکرو، گیت‌ها و ...)
    rootObject["elements"] = serializeElements(scene);
    // ۲. استخراج و ذخیره تمام اتصالات و سیم‌ها
    rootObject["wires"] = serializeWires(scene);
    // ۳. تبدیل به سند JSON و ذخیره در فایل
    QJsonDocument doc(rootObject);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for saving:" << filePath;
        return false;
    }
    QTextStream out(&file);
    out << doc.toJson(QJsonDocument::Indented); // ذخیره با فرمت مرتب و خوانا
    file.close();
    qDebug() << "Circuit successfully saved to" << filePath;
    return true;
}

QJsonArray FileManager::serializeElements(QGraphicsScene *scene)
{
    QJsonArray elementsArray;
    // پیمایش تمام اشیاء موجود روی بوم
    for (QGraphicsItem *item : scene->items())
    {
        // فیلتر کردن فقط قطعات اصلی (رد کردن سیم‌ها و ترمینال‌های خالی)
        Element *element = dynamic_cast<Element*>(item);
        if (element)
        {
            QJsonObject elementObj;
            // ذخیره نوع قطعه برای اینکه زمان لود بدانیم چه شیئی بسازیم
            elementObj["type"] = element->getComponentName();
            // ذخیره مختصات قطعه روی بوم
            QJsonObject posObj;
            posObj["x"] = element->scenePos().x();
            posObj["y"] = element->scenePos().y();
            elementObj["position"] = posObj;
            // ذخیره مقادیر داینامیک قطعه (مثل ولتاژ، مقدار مقاومت و غیره)
            QJsonObject propsObj;
            QMap<QString, QString> props = element->getProperties();
            for (auto it = props.constBegin(); it != props.constEnd(); ++it)
            {
                propsObj[it.key()] = it.value();
            }
            elementObj["properties"] = propsObj;
            elementsArray.append(elementObj);
        }
    }
    return elementsArray;
}

QJsonArray FileManager::serializeWires(QGraphicsScene *scene)
{
    QJsonArray wiresArray;
    // فعلاً آرایه سیم‌ها را خالی برمی‌گردانیم.
    // نکته: برای ذخیره سیم‌ها، در قدم بعدی به هر ترمینال یک ID اختصاص می‌دهیم.
    return wiresArray;
}

// =========================================
// تابع کارخانه (Factory): تولید شیء از روی نام
// =========================================
Element* createComponent(const QString &type) {
    if (type == "Microcontroller (MCU)") return new MCUChip();
    if (type == "Resistor") return new Resistor();
    if (type == "Capacitor") return new Capacitor();
    if (type == "LED") return new LED();
    if (type == "7-Segment Display") return new SevenSegment();
    if (type == "AND Gate") return new AndGate();
    if (type == "OR Gate") return new OrGate();
    if (type == "External Memory Chip") return new MemoryChip();
    // در صورت نیاز بقیه قطعات (مثل سلف، منابع تغذیه و ...) را می‌توانید اینجا اضافه کنید
    return nullptr;
}

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
    if (doc.isNull() || !doc.isObject())
    {
        qDebug() << "Invalid JSON file.";
        return false;
    }

    // پیش از بارگذاری مدار جدید، بوم فعلی را کاملاً پاک می‌کنیم
    scene->clear();
    QJsonObject rootObject = doc.object();

    // خواندن قطعات و تزریق به مدار
    QJsonArray elementsArray = rootObject["elements"].toArray();
    for (const QJsonValue &value : elementsArray) {
        QJsonObject elObj = value.toObject();
        QString type = elObj["type"].toString();

        // ۱. ساخت قطعه با استفاده از کارخانه
        Element *newElement = createComponent(type);
        if (newElement) {
            // ۲. تنظیم موقعیت (مختصات) قطعه
            QJsonObject posObj = elObj["position"].toObject();
            newElement->setPos(posObj["x"].toDouble(), posObj["y"].toDouble());

            // ۳. برگرداندن ویژگی‌ها (Properties) به قطعه
            QJsonObject propsObj = elObj["properties"].toObject();
            QMap<QString, QString> props;
            for (auto it = propsObj.constBegin(); it != propsObj.constEnd(); ++it) {
                props[it.key()] = it.value().toString();
            }
            newElement->setProperties(props);

            // ۴. اضافه کردن قطعه به بوم طراحی
            scene->addItem(newElement);
        } else {
            qDebug() << "Unknown component type in JSON:" << type;
        }
    }

    qDebug() << "Circuit successfully loaded from" << filePath;
    return true;
}