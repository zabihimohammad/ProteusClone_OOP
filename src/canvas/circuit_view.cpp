#include "circuit_view.h"

#include "circuit_scene.h"

#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QWheelEvent>

namespace {
constexpr qreal kMinimumZoom = 0.10;
constexpr qreal kMaximumZoom = 4.0;
}

CircuitView::CircuitView(QWidget *parent) : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAcceptDrops(true);
    setMouseTracking(true);
    setFrameShape(QFrame::NoFrame);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setDragMode(QGraphicsView::RubberBandDrag);
}

void CircuitView::setZoom(qreal zoom)
{
    zoom = qBound(kMinimumZoom, zoom, kMaximumZoom);
    const qreal current = transform().m11();
    if (qFuzzyIsNull(current)) return;
    scale(zoom / current, zoom / current);
    emit zoomChanged(qRound(zoom * 100));
    viewport()->update();
}

void CircuitView::zoomIn() { setZoom(transform().m11() * 1.15); }
void CircuitView::zoomOut() { setZoom(transform().m11() / 1.15); }
void CircuitView::resetZoom() { setZoom(1.0); }

void CircuitView::fitCanvas()
{
    auto *circuitScene = qobject_cast<CircuitScene *>(scene());
    if (!circuitScene) return;
    fitInView(circuitScene->canvasRect().adjusted(-50, -50, 50, 50), Qt::KeepAspectRatio);
    const qreal fittedZoom = qBound(kMinimumZoom, transform().m11(), kMaximumZoom);
    if (!qFuzzyCompare(fittedZoom, transform().m11())) setZoom(fittedZoom);
    centerOn(circuitScene->canvasRect().center());
    m_initialFitDone = true;
    emit zoomChanged(qRound(transform().m11() * 100));
}

void CircuitView::wheelEvent(QWheelEvent *event)
{
    setZoom(transform().m11() * (event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15));
    event->accept();
}

void CircuitView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && navigatorToggleRect().contains(event->pos())) {
        m_navigatorCollapsed = !m_navigatorCollapsed;
        viewport()->update();
        event->accept();
        return;
    }
    if (!m_navigatorCollapsed && event->button() == Qt::LeftButton &&
        navigatorCanvasRect().contains(event->pos())) {
        m_isNavigating = true;
        navigateFromNavigator(event->pos());
        setCursor(Qt::PointingHandCursor);
        event->accept();
        return;
    }

    // 🛠️ مدیریت Panning: اگر دکمه وسط فشرده شد یا Shift+Click چپ انجام شد
    if (event->button() == Qt::MiddleButton ||
        (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier))) {

        // موقتاً درگ مود انتخابی را خاموش می‌کنیم تا کادر مستطیلی مزاحم حرکت روی بوم نشود
        setDragMode(QGraphicsView::NoDrag);
        m_isPanning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    // در حالت عادی کلیک چپ روی فضای خالی، کادر انتخاب مستطیلی باز می‌کند
    setDragMode(QGraphicsView::RubberBandDrag);
    QGraphicsView::mousePressEvent(event);
}

void CircuitView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isNavigating) {
        navigateFromNavigator(event->pos());
    } else if (m_isPanning) {
        const QPoint delta = event->pos() - m_lastMousePos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastMousePos = event->pos();
        viewport()->update();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
    emit cursorPositionChanged(mapToScene(event->pos()));
}

void CircuitView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isNavigating && event->button() == Qt::LeftButton) {
        m_isNavigating = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    if (m_isPanning && (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton)) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);

        // 🛠️ پس از اتمام حرکت روی بوم، دوباره درگ مود را روی کادر انتخابی تنظیم می‌کنیم
        setDragMode(QGraphicsView::RubberBandDrag);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

QRectF CircuitView::navigatorRect() const
{
    if (m_navigatorCollapsed)
        return {viewport()->width() - 58.0, viewport()->height() - 58.0, 42.0, 42.0};
    return {viewport()->width() - 198.0, viewport()->height() - 142.0, 182.0, 126.0};
}

QRectF CircuitView::navigatorToggleRect() const
{
    if (m_navigatorCollapsed) return navigatorRect();
    const QRectF nav = navigatorRect();
    return {nav.right() - 31.0, nav.top() + 6.0, 23.0, 23.0};
}

QRectF CircuitView::navigatorCanvasRect() const
{
    auto *circuitScene = qobject_cast<CircuitScene *>(scene());
    if (!circuitScene) return {};
    const QRectF area = navigatorRect().adjusted(11, 31, -11, -11);
    const QRectF canvas = circuitScene->canvasRect();
    const qreal mapScale = qMin(area.width() / canvas.width(), area.height() / canvas.height());
    const QSizeF size = canvas.size() * mapScale;
    return {area.center().x() - size.width() / 2.0,
            area.center().y() - size.height() / 2.0,
            size.width(), size.height()};
}

void CircuitView::navigateFromNavigator(const QPointF &position)
{
    auto *circuitScene = qobject_cast<CircuitScene *>(scene());
    if (!circuitScene) return;
    const QRectF map = navigatorCanvasRect();
    const QRectF canvas = circuitScene->canvasRect();
    const QPointF point(qBound(map.left(), position.x(), map.right()),
                        qBound(map.top(), position.y(), map.bottom()));
    const qreal x = (point.x() - map.left()) / map.width();
    const qreal y = (point.y() - map.top()) / map.height();
    centerOn(canvas.left() + x * canvas.width(), canvas.top() + y * canvas.height());
    viewport()->update();
}

void CircuitView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    const QRectF nav = navigatorRect();
    painter.setPen(QPen(QColor("#D7DEE7"), 1));
    painter.setBrush(QColor(255, 255, 255, 242));
    painter.drawRoundedRect(nav, 12, 12);
    if (m_navigatorCollapsed) {
        painter.setPen(QColor("#1473E6"));
        QFont font = painter.font();
        font.setPointSize(17);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(nav, Qt::AlignCenter, QString::fromUtf8("⌖"));
        return;
    }

    QFont titleFont = painter.font();
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(QColor("#354152"));
    painter.drawText(nav.adjusted(11, 7, -11, -nav.height() + 28),
                     Qt::AlignLeft | Qt::AlignVCenter, tr("Navigator"));
    painter.setPen(QColor("#667180"));
    painter.setBrush(QColor("#F0F3F7"));
    painter.drawRoundedRect(navigatorToggleRect(), 6, 6);
    painter.drawText(navigatorToggleRect(), Qt::AlignCenter, QString::fromUtf8("−"));

    auto *circuitScene = qobject_cast<CircuitScene *>(scene());
    if (!circuitScene) return;
    const QRectF map = navigatorCanvasRect();
    painter.setPen(QPen(QColor("#9AA6B5"), 1));
    painter.setBrush(QColor("#F5F7FA"));
    painter.drawRect(map);

    const QRectF canvas = circuitScene->canvasRect();
    const QRectF visible = mapToScene(viewport()->rect()).boundingRect().intersected(canvas);
    if (!visible.isEmpty()) {
        const qreal mapScale = map.width() / canvas.width();
        QRectF viewportRect(map.left() + (visible.left() - canvas.left()) * mapScale,
                            map.top() + (visible.top() - canvas.top()) * mapScale,
                            visible.width() * mapScale, visible.height() * mapScale);
        painter.setPen(QPen(QColor("#1473E6"), 1.5));
        painter.setBrush(QColor(20, 115, 230, 45));
        painter.drawRect(viewportRect.intersected(map));
    }
}

void CircuitView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (!m_initialFitDone) fitCanvas();
}
