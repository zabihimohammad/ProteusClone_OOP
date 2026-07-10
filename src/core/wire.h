#pragma once
#include <QGraphicsItem>
#include <QVector>
#include <QPointF>

class Terminal;

class Wire : public QGraphicsItem {
public:
    Wire(Terminal *startTerm, QPointF startPos);

    void setEndPoint(QPointF endPos);
    void addWaypoint(QPointF point); // برای گره‌گذاری دستی
    void setFullRoute(const QVector<QPointF> &route); // برای مسیریاب هوشمند
    void confirmConnection(Terminal *endTerm);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
// در بخش public اضافه کنید:
    void updateRoute();
    ~Wire() override; // مخرب برای پاک کردن تمیز از حافظه
    // توابع کمکی برای خواندن پایه‌های متصل به سیم در سیستم ذخیره‌سازی
    Terminal* getStartTerminal() const { return startTerminal; }
    Terminal* getEndTerminal() const { return endTerminal; }
private:
    Terminal *startTerminal;
    Terminal *endTerminal;
    QVector<QPointF> points; // جایگزین p1 و p2 شد تا بتواند چندین شکستگی را ذخیره کند
};