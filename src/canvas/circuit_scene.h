#pragma once
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMimeData>
#include "../core/terminal.h"

// Forward declarations
class Wire;

class CircuitScene : public QGraphicsScene {
Q_OBJECT
public:
    explicit CircuitScene(QObject *parent = nullptr);

protected:
    // تابع رسم پس‌زمینه شطرنجی
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    // توابع رهگیری موس (برای سیم‌کشی تعاملی و ثبت تاریخچه)
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override; // <-- این خط اضافه شد

    // تابع رهگیری کیبورد (برای حذف ایمن قطعات و میانبرهای Undo/Redo/Save)
    void keyPressEvent(QKeyEvent *event) override;

    // توابع سیستم Drag & Drop (دریافت قطعات از منوی UI)
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    int gridSize;            // فاصله بین نقطه‌های شطرنجی
    bool isWiring;           // آیا کاربر در حال سیم‌کشی است؟
    Wire *tempWire;          // سیمی که در حال کشیده شدن است
    Terminal *startTerminal; // پایه‌ای که سیم از آن شروع شده
};