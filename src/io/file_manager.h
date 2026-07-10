#pragma once
#include <QString>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QList>

// پیش‌تعریف کلاس‌ها
class Element;

class FileManager {
public:
    static bool saveCircuit(const QString &filePath, QGraphicsScene *scene);
    static bool loadCircuit(const QString &filePath, QGraphicsScene *scene);

private:
    static QJsonArray serializeElements(const QList<Element*> &elementsList);
    static QJsonArray serializeWires(QGraphicsScene *scene, const QList<Element*> &elementsList);
};