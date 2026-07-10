#include "file_manager.h"
#include "../core/element.h"
#include "../core/wire.h"
#include "../core/terminal.h"
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
    // نکته: برای ذخیره سیم‌ها، باید به هر ترمینال یک ID اختصاص دهیم.
    // در گام بعدی وقتی ساختار لود را تکمیل کردیم، این قسمت را با مکانیزم ID گذاری کامل می‌کنم.
    return wiresArray;
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
    // پیش از بارگذاری مدار جدید، بوم فعلی را پاک می‌کنیم
    scene->clear();
    QJsonObject rootObject = doc.object();
    // خواندن قطعات و تزریق به مدار
    // (منطق این بخش را در پیام بعدی پس از تایید شما برای ساختن Factory قطعات اضافه می‌کنیم)
    return true;
}