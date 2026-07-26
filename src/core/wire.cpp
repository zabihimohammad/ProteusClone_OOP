#include "wire.h"
#include "terminal.h"
#include <QPainter>
#include "auto_router.h" // برای دسترسی به هوش مصنوعی
#include <QToolTip>
#include <QGraphicsSceneHoverEvent>
#include "probe_item.h"
#include "../canvas/circuit_scene.h"
#include "../components/basic_components.h"
#include <QPainterPath>
Wire::Wire(Terminal *startTerm, QPointF startPos) {
    startTerminal = startTerm;
    endTerminal = nullptr;

    // نقطه شروع را دو بار به لیست اضافه می‌کنیم (دومی برای انتهای متحرک سیم است)
    points.append(startPos);
    points.append(startPos);
    setZValue(-1);
    // اضافه کردن این خط: اجازه انتخاب شدن با موس
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
}

void Wire::setEndPoint(QPointF endPos) {
    if (!points.isEmpty()) {
        points.last() = endPos; // همیشه آخرین نقطه به دنبال موس حرکت می‌کند
        prepareGeometryChange();
    }
}

void Wire::addWaypoint(QPointF point) {
    if (points.size() >= 2) {
        // نقطه جدید را درست قبل از نقطه متحرک پایانی وارد می‌کنیم
        points.insert(points.size() - 1, point);
    }
}

void Wire::setFullRoute(const QVector<QPointF> &route) {
    // 🛠️ باگ بزرگ اینجا بود! حالا مسیر هوش مصنوعی دقیقاً و بدون اضافات کپی می‌شود
    points = route;
    prepareGeometryChange();
}

void Wire::confirmConnection(Terminal *endTerm) {
    endTerminal = endTerm;
    // وقتی سیم وصل شد، خودش را در حافظه پایه‌های مبدا و مقصد ثبت می‌کند
    if (startTerminal) startTerminal->addWire(this);
    if (endTerminal) endTerminal->addWire(this);
}

QRectF Wire::boundingRect() const {
    // پیدا کردن کادری که تمام گره‌های سیم را در بر بگیرد
    if (points.isEmpty()) return QRectF();

    qreal minX = points[0].x(), maxX = points[0].x();
    qreal minY = points[0].y(), maxY = points[0].y();

    for (const QPointF &p : points) {
        if (p.x() < minX) minX = p.x();
        if (p.x() > maxX) maxX = p.x();
        if (p.y() < minY) minY = p.y();
        if (p.y() > maxY) maxY = p.y();
    }

    return QRectF(minX, minY, maxX - minX, maxY - minY).adjusted(-5, -5, 5, 5);
}


void Wire::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen pen(isSelected() ? Qt::red : Qt::blue, 2);
    painter->setPen(pen);

    if (points.size() < 2) return;

    // 🛠️ اصلاح طلایی: به جای سایز آرایه، چک می‌کنیم که سیم به مقصد رسیده است یا خیر
    if (!endTerminal) {
        // سیم در دست کاربر است (رسم ارتوگونال موقت)
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF pA = points[i];
            QPointF pB = points[i+1];
            qreal midX = (pA.x() + pB.x()) / 2.0;

            painter->drawLine(pA, QPointF(midX, pA.y()));
            painter->drawLine(QPointF(midX, pA.y()), QPointF(midX, pB.y()));
            painter->drawLine(QPointF(midX, pB.y()), pB);
        }
    } else {
        // سیم نهایی شده است (اتصال مستقیم نقاط هوش مصنوعی بدون کج شدن)
        for (int i = 0; i < points.size() - 1; ++i) {
            painter->drawLine(points[i], points[i+1]);
        }
    }
}
void Wire::updateRoute() {
    // اگر سیم هنوز کامل کشیده نشده (در دست کاربر است) نیازی به آپدیت ندارد
    if (!startTerminal || !endTerminal || !scene()) return;

    // فراخوانی مجدد هوش مصنوعی با مختصات جدید پایه‌ها
    QVector<QPointF> newRoute = AutoRouter::findPath(
            scene(),
            startTerminal->scenePos(),
            endTerminal->scenePos(),
            startTerminal,
            endTerminal,
            this
    );

    // رسم مجدد مسیر
    setFullRoute(newRoute);
}
Wire::~Wire() {
    // ۱. قطع اتصال از ترمینال‌ها
    if (startTerminal) startTerminal->removeWire(this);
    if (endTerminal) endTerminal->removeWire(this);

    // ۲. بررسی هوشمند گره‌های متصل پس از حذف سیم
    if (scene()) {
        QList<Terminal*> termsToCheck = {startTerminal, endTerminal};
        for (Terminal* term : termsToCheck) {
            if (!term) continue;

            // پیدا کردن المان مادری که این ترمینال متعلق به آن است
            QGraphicsItem* parent = term->parentItem();
            JunctionNode* junction = dynamic_cast<JunctionNode*>(parent);

            if (junction) {
                int wireCount = term->getConnectedWires().size();

                // الف) اگر هیچ سیمی به گره وصل نبود یا فقط ۱ سیم مانده بود (تکه‌سیم رها شده)، گره کاملاً پاک شود
                if (wireCount <= 1) {
                    scene()->removeItem(junction);
                    // حذف گره به صورت زنجیره‌ای باعث پاک شدن سیم‌های رها شده‌ی متصل به آن نیز می‌شود
                    delete junction;
                }
                    // ب) اگر دقیقاً ۲ سیم به گره مانده بود، گره حذف و دو سیم تبدیل به یک سیم مستقیم شوند (Healing)
                else if (wireCount == 2) {
                    // این بخش در گام سوم پیاده‌سازی می‌شود تا سیم‌ها به هم جوش بخورند
                }
            }
        }
    }
}
void Wire::disconnectTerminal(Terminal *term) {
    // اگر ترمینالی که دارد نابود می‌شود پایه شروع من است، آن را فراموش کن
    if (startTerminal == term) {
        startTerminal = nullptr;
    }
    // اگر پایه انتهایی من است، آن را فراموش کن
    if (endTerminal == term) {
        endTerminal = nullptr;
    }
}
// ==========================================================
// محاسبه دقیق خطوط برخورد سیم با موس (جلوگیری از باگ کادر بزرگ)
// ==========================================================
QPainterPath Wire::shape() const {
    QPainterPath path;
    if (points.size() < 2) return path;

    if (!endTerminal) {
        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF pA = points[i];
            QPointF pB = points[i+1];
            qreal midX = (pA.x() + pB.x()) / 2.0;

            if (i == 0) path.moveTo(pA);
            path.lineTo(QPointF(midX, pA.y()));
            path.lineTo(QPointF(midX, pB.y()));
            path.lineTo(pB);
        }
    } else {
        path.moveTo(points[0]);
        for (int i = 1; i < points.size(); ++i) {
            path.lineTo(points[i]);
        }
    }

    QPainterPathStroker stroker;
    stroker.setWidth(10);
    return stroker.createStroke(path);
}
// ==========================================================
// ۱. تشخیص دقیق خطی از سیم که کاربر روی آن کلیک کرده است
// ==========================================================
void Wire::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton && points.size() >= 2) {
        QPointF clickPos = event->scenePos();
        m_draggedSegmentIndex = -1;
        m_isDraggingHorizontal = false;
        m_isDraggingVertical = false;

        double minDistance = 10.0; // شعاع ۱۰ پیکسلی برای گرفتن سیم

        for (int i = 0; i < points.size() - 1; ++i) {
            QPointF a = points[i];
            QPointF b = points[i+1];

            double dx = b.x() - a.x();
            double dy = b.y() - a.y();
            double l2 = dx * dx + dy * dy;
            if (l2 < 0.1) continue;

            double t = ((clickPos.x() - a.x()) * dx + (clickPos.y() - a.y()) * dy) / l2;
            t = qBound(0.0, t, 1.0);
            QPointF proj(a.x() + t * dx, a.y() + t * dy);

            double dist = QLineF(clickPos, proj).length();
            if (dist < minDistance) {
                minDistance = dist;
                m_draggedSegmentIndex = i;
            }
        }

        if (m_draggedSegmentIndex != -1) {
            m_lastDragPos = clickPos;
            QPointF p1 = points[m_draggedSegmentIndex];
            QPointF p2 = points[m_draggedSegmentIndex + 1];

            if (qAbs(p1.y() - p2.y()) < 1.0) m_isDraggingHorizontal = true;
            else m_isDraggingVertical = true;

            if (m_draggedSegmentIndex == 0) {
                points.insert(1, points[0]);
                m_draggedSegmentIndex = 1;
            }
            if (m_draggedSegmentIndex == points.size() - 2) {
                points.insert(points.size() - 1, points.last());
            }

            // 🛠️ فیکس طلایی: به جای ریترن زودهنگام، اجازه می‌دهیم رویداد کلیک
            // به کلاس مادر برسد تا سیم به درستی متوجه وضعیت Selection (انتخاب) بشود.
            QGraphicsItem::mousePressEvent(event);
            return;
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

// ==========================================================
// ۲. جابجایی زنده خطوط سیم با حرکت موس
// ==========================================================
void Wire::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_draggedSegmentIndex != -1) {
        QPointF currentPos = event->scenePos();
        QPointF delta = currentPos - m_lastDragPos;
        m_lastDragPos = currentPos;

        prepareGeometryChange(); // به Qt اطلاع می‌دهیم که ابعاد گرافیکی در حال تغییر است

        if (m_isDraggingHorizontal) {
            points[m_draggedSegmentIndex].setY(points[m_draggedSegmentIndex].y() + delta.y());
            points[m_draggedSegmentIndex + 1].setY(points[m_draggedSegmentIndex + 1].y() + delta.y());
        } else if (m_isDraggingVertical) {
            points[m_draggedSegmentIndex].setX(points[m_draggedSegmentIndex].x() + delta.x());
            points[m_draggedSegmentIndex + 1].setX(points[m_draggedSegmentIndex + 1].x() + delta.x());
        }

        update();
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

// ==========================================================
// ۳. رها کردن سیم و قفل شدن اتوماتیک روی گرید (Snap)
// ==========================================================
void Wire::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_draggedSegmentIndex != -1) {
        int snapSize = 20; // گام‌های ۲۰ پیکسلی برای صاف ماندن مدار

        prepareGeometryChange();
        if (m_isDraggingHorizontal) {
            double snappedY = qRound(points[m_draggedSegmentIndex].y() / snapSize) * snapSize;
            points[m_draggedSegmentIndex].setY(snappedY);
            points[m_draggedSegmentIndex + 1].setY(snappedY);
        } else if (m_isDraggingVertical) {
            double snappedX = qRound(points[m_draggedSegmentIndex].x() / snapSize) * snapSize;
            points[m_draggedSegmentIndex].setX(snappedX);
            points[m_draggedSegmentIndex + 1].setX(snappedX);
        }

        m_draggedSegmentIndex = -1;
        m_isDraggingHorizontal = false;
        m_isDraggingVertical = false;
        update();

        event->accept();
        return;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}