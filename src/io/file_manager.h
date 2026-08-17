#pragma once
#include <QString>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QList>
#include <QStack>
#include <QByteArray>

class Element;

class FileManager {
public:
    // ذخیره و بازیابی فایل
    static bool saveCircuit(const QString &filePath, QGraphicsScene *scene);
    static bool loadCircuit(const QString &filePath, QGraphicsScene *scene);

    // تاریخچه ویرایش
    static void recordState(QGraphicsScene *scene);
    static void undo(QGraphicsScene *scene);
    static void redo(QGraphicsScene *scene);

private:
    // تبدیل مدار و JSON
    static QJsonArray serializeElements(const QList<Element*> &elementsList);
    static QJsonArray serializeWires(QGraphicsScene *scene, const QList<Element*> &elementsList);

    // انتقال مدار به حافظه و بوم
    static QByteArray captureSceneState(QGraphicsScene *scene);
    static void restoreSceneState(const QByteArray &stateData, QGraphicsScene *scene);

    // پشته‌های تاریخچه
    static QStack<QByteArray> undoStack;
    static QStack<QByteArray> redoStack;
    static QByteArray currentState;
};
