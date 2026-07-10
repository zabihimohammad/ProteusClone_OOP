#pragma once
#include <QGraphicsItem>
#include <QList>

class Wire;

// کلاس ترمینال (پایه اتصال) که روی قطعات قرار می‌گیرد
class Terminal : public QGraphicsItem {
public:
    explicit Terminal(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void addWire(Wire *wire);
    void removeWire(Wire *wire);
    QList<Wire*> getConnectedWires() const;

    // ==========================================
    // خواص الکتریکی و منطقی (برای شبیه‌سازی و DRC)
    // ==========================================
    void setVoltage(double v);       // اعمال ولتاژ به پایه
    double getVoltage() const;       // خواندن ولتاژ
    void setUndefined();             // نامشخص کردن وضعیت (برای خطاهای فلوتینگ)

    bool isFloating() const;         // بررسی رها بودن پایه
    bool getLogicState() const;      // تبدیل ولتاژ به صفر و یک منطقی
    bool isUndefinedState() const;   // بررسی خطای اتصال کوتاه

    void resetState();               // پاک کردن وضعیت در ابتدای هر سیکل پردازش

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    bool isHovered;
    QList<Wire*> connectedWires;

    // متغیرهای شبیه‌سازی بک‌اند
    double _voltage;
    bool _isDriven;      // آیا در این سیکل ولتاژی به این پایه تزریق شده است؟
    bool _isUndefined;   // آیا پایه دچار تناقض (مثل اتصال کوتاه) شده است؟
};