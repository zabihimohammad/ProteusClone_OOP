#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMimeData>

#include "../core/probe_item.h"
#include "../core/terminal.h"

class Wire;

class CircuitScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CircuitScene(QObject *parent = nullptr);

    ProbeItem *voltageProbe = nullptr;
    bool isProbeEnabled = false;

    int gridSpacing() const { return m_gridSize; }
    bool snapEnabled() const { return m_snapEnabled; }
    QRectF canvasRect() const { return m_canvasRect; }

public slots:
    void setGridSize(int size);
    void setSnapEnabled(bool enabled) { m_snapEnabled = enabled; }
    void setCanvasSize(const QSizeF &size);
    QGraphicsItem *addComponent(const QString &componentType, const QPointF &position);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    int m_gridSize = 20;
    bool m_snapEnabled = true;
    QRectF m_canvasRect{-800, -500, 1600, 1000};
    bool isWiring = false;
    Wire *tempWire = nullptr;
    Terminal *startTerminal = nullptr;
};
