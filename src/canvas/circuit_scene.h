#pragma once
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMimeData>
#include "../core/terminal.h"
#include "../core/probe_item.h"
// Forward declarations
class Wire;

class CircuitScene : public QGraphicsScene {
Q_OBJECT
public:
    explicit CircuitScene(QObject *parent = nullptr);
    ProbeItem *voltageProbe;
    bool isProbeEnabled = false;
protected:
    // تابع رسم پس‌زمینه شطرنجی
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    // توابع رهگیری موس (برای سیم‌کشی تعاملی)
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    // تابع رهگیری کیبورد (برای حذف ایمن قطعات)
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