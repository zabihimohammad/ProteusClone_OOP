#pragma once
#include <QVector>
#include <QPointF>
#include <QGraphicsScene>

class Terminal;

class AutoRouter {
public:
    // این تابع نقطه شروع و پایان را می‌گیرد و لیستی از نقاط (مسیر ایمن) را برمی‌گرداند
    static QVector<QPointF> findPath(QGraphicsScene *scene, QPointF startPos, QPointF endPos, Terminal *startTerm, Terminal *endTerm);
};