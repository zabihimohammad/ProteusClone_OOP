#pragma once

#include <QGraphicsView>

class CircuitView final : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CircuitView(QWidget *parent = nullptr);

public slots:
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitCanvas();

signals:
    void zoomChanged(int percent);
    void cursorPositionChanged(QPointF scenePosition);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setZoom(qreal zoom);
    QRectF navigatorRect() const;
    QRectF navigatorCanvasRect() const;
    QRectF navigatorToggleRect() const;
    void navigateFromNavigator(const QPointF &position);

    bool m_isPanning = false;
    bool m_isNavigating = false;
    bool m_navigatorCollapsed = false;
    bool m_initialFitDone = false;
    QPoint m_lastMousePos;
};
