#pragma once
#include <QVector>
#include <QPointF>
#include <QGraphicsScene>

class Terminal;
class Wire; // پیش‌تعریف کلاس سیم

class AutoRouter {
public:
    // اضافه شدن currentWire به انتهای تابع
    static QVector<QPointF> findPath(QGraphicsScene *scene, QPointF startPos, QPointF endPos, Terminal *startTerm, Terminal *endTerm, Wire *currentWire);
};