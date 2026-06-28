#pragma once
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
class Wire;
class Terminal;

class CircuitScene : public QGraphicsScene {
Q_OBJECT
public:
    explicit CircuitScene(QObject *parent = nullptr);

protected:
    // تابع رسم پس‌زمینه شطرنجی
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    // توابع رهگیری موس برای سیم‌کشی
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
private:
    int gridSize;            // فاصله بین نقطه‌های شطرنجی
    bool isWiring;           // آیا کاربر در حال سیم‌کشی است؟
    Wire *tempWire;          // سیمی که در حال کشیده شدن است
    Terminal *startTerminal; // پایه‌ای که سیم از آن شروع شده
};