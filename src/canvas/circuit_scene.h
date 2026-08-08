#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMimeData>

#include "../core/probe_item.h"
#include "../core/terminal.h"

class Wire;
class Element;
class CircuitScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum class GridStyle { Dots, Lines };

    explicit CircuitScene(QObject *parent = nullptr);

    ProbeItem *voltageProbe = nullptr;
    bool isProbeEnabled = false;

    int gridSpacing() const { return m_gridSize; }
    bool isGridVisible() const { return m_gridVisible; }
    bool isSnapEnabled() const { return m_snapEnabled; }
    GridStyle gridStyle() const { return m_gridStyle; }
    QRectF canvasRect() const { return m_canvasRect; }
    void copySelectedComponents();
    void pasteCopiedComponents(const QPointF &targetScenePos);

public slots:
    void setWiringMode(bool mode); // 🛠️ این متد اضافه شد
    void setGridSize(int size);
    void setGridVisible(bool visible);
    void setSnapEnabled(bool enabled);
    void setGridStyle(GridStyle style);
    void setCanvasSize(const QSizeF &size);
    QGraphicsItem *addComponent(const QString &componentType, const QPointF &position);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    // 🛠️ ساختار پیشرفته کلیپ‌بورد برای ذخیره همزمان قطعات و اتصالات بین آن‌ها
    struct CopiedComponent {
        QString type;
        QMap<QString, QString> properties;
        QPointF relativePos;
        Element* originalAddress; // برای ردیابی اتصالات در زمان کپی
    };

    struct CopiedWire {
        int startComponentIndex;
        int startTerminalIndex;
        int endComponentIndex;
        int endTerminalIndex;
    };

    QVector<CopiedComponent> m_clipboardComponents;
    QVector<CopiedWire> m_clipboardWires;
    int m_gridSize = 20;
    bool m_gridVisible = true;
    bool m_snapEnabled = true;
    GridStyle m_gridStyle = GridStyle::Dots;
    QRectF m_canvasRect{-800, -500, 1600, 1000};
    bool isWiring = false;
    Wire *tempWire = nullptr;
    Terminal *startTerminal = nullptr;

    bool m_wiringMode = false;
    QPointF m_crosshairPos;
};
