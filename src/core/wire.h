#pragma once
#include <QGraphicsItem>
#include <QVector>
#include <QPointF>
#include <QString>

class Terminal;

class Wire : public QGraphicsItem {
public:
    QPainterPath shape() const override;
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

    // این متغیر بعداً توسط موتور شبیه‌ساز آپدیت می‌شود (مثلاً "5.0V" یا "0.0V")
    QString voltageLevel = "Undefined";

    // توابع کمکی برای خواندن پایه‌های متصل به سیم در سیستم ذخیره‌سازی
    Terminal* getStartTerminal() const { return startTerminal; }
    Terminal* getEndTerminal() const { return endTerminal; }

    // قطع اتصال ترمینال (اضافه شده از سمت رابط کاربری)
    void disconnectTerminal(Terminal *term);

private:
    Terminal *startTerminal;
    Terminal *endTerminal;
    QVector<QPointF> points; // جایگزین p1 و p2 شد تا بتواند چندین شکستگی را ذخیره کند

protected:
    //void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    // void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};