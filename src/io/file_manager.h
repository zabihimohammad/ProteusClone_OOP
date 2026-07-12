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
    // توابع ذخیره و بازیابی در فایل (بخش ۱۰.۱)
    static bool saveCircuit(const QString &filePath, QGraphicsScene *scene);
    static bool loadCircuit(const QString &filePath, QGraphicsScene *scene);

    // ==========================================
    // توابع سیستم پیشرفته Undo/Redo (بخش ۱۰.۲)
    // ==========================================
    static void recordState(QGraphicsScene *scene); // ثبت وضعیت جدید مدار
    static void undo(QGraphicsScene *scene);        // بازگشت به عقب
    static void redo(QGraphicsScene *scene);        // رفتن به جلو

private:
    // توابع هسته برای استخراج داده
    static QJsonArray serializeElements(const QList<Element*> &elementsList);
    static QJsonArray serializeWires(QGraphicsScene *scene, const QList<Element*> &elementsList);

    // توابع کمکی برای تبدیل بوم به حافظه (Memory) و بالعکس
    static QByteArray captureSceneState(QGraphicsScene *scene);
    static void restoreSceneState(const QByteArray &stateData, QGraphicsScene *scene);

    // پشته‌های حافظه برای نگهداری تاریخچه (History Stacks)
    static QStack<QByteArray> undoStack;
    static QStack<QByteArray> redoStack;
    static QByteArray currentState;
};