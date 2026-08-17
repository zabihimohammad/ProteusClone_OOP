#pragma once
#include <QVector>
#include <QPointF>
#include <QGraphicsScene>

class Terminal;
class Wire; // پیش‌تعریف کلاس سیم

class AutoRouter {
public:
    // سیم جاری برای جلوگیری از برخورد با خودش
    static QVector<QPointF> findPath(QGraphicsScene *scene, QPointF startPos, QPointF endPos, Terminal *startTerm, Terminal *endTerm, Wire *currentWire);
};