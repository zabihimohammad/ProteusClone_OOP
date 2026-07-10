#pragma once
#include <QString>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class FileManager {
public:
    // ذخیره وضعیت فعلی بوم در یک مسیر مشخص
    static bool saveCircuit(const QString &filePath, QGraphicsScene *scene);

    // بارگذاری و بازسازی مدار از روی فایل ذخیره شده
    static bool loadCircuit(const QString &filePath, QGraphicsScene *scene);

private:
    // توابع کمکی (Helper) برای جلوگیری از شلوغ شدن توابع اصلی
    static QJsonArray serializeElements(QGraphicsScene *scene);
    static QJsonArray serializeWires(QGraphicsScene *scene);
};