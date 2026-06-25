#include "auto_router.h"
#include "terminal.h"
#include "wire.h"
#include <QMap>
#include <QSet>
#include <QGraphicsItem>
#include <QLineF>
#include <queue>
#include <vector>
#include <algorithm>

QString pointToString(const QPointF &p) {
    return QString("%1,%2").arg(p.x()).arg(p.y()); // ذخیره دقیق اعشار برای گام‌های پویا
}

struct AStarNode {
    QPointF pos;
    double gCost;
    double fCost;
    QPointF dir;
    bool operator>(const AStarNode& other) const {
        if (qAbs(fCost - other.fCost) < 0.1) return gCost < other.gCost;
        return fCost > other.fCost;
    }
};

// ==========================================
// ۱. سیستم پیشرفته تشخیص جهت پایه
// ==========================================
QPointF getPinDirection(Terminal* term) {
    if (!term || !term->parentItem()) return QPointF(20, 0);

    QPointF local = term->pos();
    QRectF bounds = term->parentItem()->boundingRect();

    double distLeft   = qAbs(local.x() - bounds.left());
    double distRight  = qAbs(local.x() - bounds.right());
    double distTop    = qAbs(local.y() - bounds.top());
    double distBottom = qAbs(local.y() - bounds.bottom());

    double minDist = std::min({distLeft, distRight, distTop, distBottom});

    if (minDist == distLeft)   return QPointF(-20, 0);
    if (minDist == distRight)  return QPointF(20, 0);
    if (minDist == distTop)    return QPointF(0, -20);
    return QPointF(0, 20);
}

// ==========================================
// ۲. هسته اصلی مسیریاب منهتن با گام‌های پویا
// ==========================================
QVector<QPointF> AutoRouter::findPath(QGraphicsScene *scene, QPointF startPos, QPointF endPos, Terminal *startTerm, Terminal *endTerm, Wire *currentWire) {
    double stepSize = 10.0;
    int maxIterations = 10000;

    QPointF startDir = getPinDirection(startTerm);
    QPointF endDir = getPinDirection(endTerm);
    QPointF startStub = startPos + startDir;
    QPointF endStub = endPos + endDir;

    if (startStub == endStub) return {startPos, startStub, endPos};

    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    QMap<QString, QPointF> cameFrom;
    QMap<QString, double> gScore;
    QSet<QString> closedSet;

    // نرمال‌سازی جهت اولیه برای جلوگیری از خطاهای اعشاری
    QPointF normStartDir(0, 0);
    if (qAbs(startDir.x()) > 0) normStartDir.setX(startDir.x() > 0 ? 1 : -1);
    if (qAbs(startDir.y()) > 0) normStartDir.setY(startDir.y() > 0 ? 1 : -1);

    openSet.push({startStub, 0, 0, normStartDir});
    gScore[pointToString(startStub)] = 0;

    bool pathFound = false;
    QPointF finalPoint;

    int iterations = 0;
    while (!openSet.empty() && iterations < maxIterations) {
        AStarNode current = openSet.top();
        openSet.pop();
        iterations++;

        QString currentStr = pointToString(current.pos);
        if (closedSet.contains(currentStr)) continue;
        closedSet.insert(currentStr);

        // رسیدن به هدف با دقت ۱۰۰٪ (دیگر نیازی به شعاع ۵ پیکسلی نیست)
        if (current.pos == endStub) {
            pathFound = true;
            finalPoint = current.pos;
            break;
        }

        // تولید گام‌های پویا: هم گام‌های ۱۰ پیکسلی ثابت و هم گام‌های ریز برای رسیدن دقیق به هدف
        QVector<QPointF> nextPoints = {
                current.pos + QPointF(0, -stepSize), current.pos + QPointF(0, stepSize),
                current.pos + QPointF(-stepSize, 0), current.pos + QPointF(stepSize, 0)
        };

        // تزریق گام‌های دقیق برای جلوگیری از زیگ‌زاگ
        if (current.pos.x() != endStub.x() && qAbs(current.pos.x() - endStub.x()) < stepSize) {
            nextPoints.append(QPointF(endStub.x(), current.pos.y()));
        }
        if (current.pos.y() != endStub.y() && qAbs(current.pos.y() - endStub.y()) < stepSize) {
            nextPoints.append(QPointF(current.pos.x(), endStub.y()));
        }

        for (const QPointF &next : nextPoints) {
            QPointF moveDir = next - current.pos;
            QPointF normDir(0, 0);
            if (qAbs(moveDir.x()) > 0) normDir.setX(moveDir.x() > 0 ? 1 : -1);
            if (qAbs(moveDir.y()) > 0) normDir.setY(moveDir.y() > 0 ? 1 : -1);

            // ممنوعیت برگشت به عقب
            if (current.dir != QPointF(0,0) && normDir == QPointF(-current.dir.x(), -current.dir.y())) {
                continue;
            }

            QString nextStr = pointToString(next);
            if (closedSet.contains(nextStr)) continue;

            QRectF checkRect(next.x() - 1, next.y() - 1, 2, 2);
            QList<QGraphicsItem*> itemsAtNext = scene->items(checkRect, Qt::IntersectsItemShape);

            bool hitObstacle = false;
            double wirePenalty = 0.0;

            for (QGraphicsItem *item : itemsAtNext) {
                Wire *wireItem = dynamic_cast<Wire*>(item);
                if (wireItem) {
                    if (wireItem == currentWire) continue;

                    // تنظیم طلایی: جریمه بسیار کم برای تقاطع (۱۵)
                    // باعث می‌شود سیم بدون ترس از دیگران عبور کند اما همچنان موازی نیفتد
                    wirePenalty += 15.0;
                    continue;
                }

                if (dynamic_cast<Terminal*>(item)) continue;

                QGraphicsItem *parent = item->parentItem() ? item->parentItem() : item;

                // شعاع فرار از قطعه مبدا و مقصد به ۱۵ کاهش یافت تا در کنار قطعه پله نسازد
                if (parent == startTerm->parentItem() || parent == endTerm->parentItem()) {
                    if (QLineF(next, startStub).length() <= 15.0 || QLineF(next, endStub).length() <= 15.0) {
                        continue;
                    }
                }

                hitObstacle = true;
                break;
            }

            if (hitObstacle) continue;

            // جریمه چرخش ۱۰ امتیاز است تا خطوط مستقیم بمانند
            double turnPenalty = (current.dir != QPointF(0,0) && current.dir != normDir) ? 10.0 : 0.0;
            double stepCost = qAbs(moveDir.x()) + qAbs(moveDir.y());
            double tentative_gScore = gScore[currentStr] + stepCost + turnPenalty + wirePenalty;

            if (!gScore.contains(nextStr) || tentative_gScore < gScore[nextStr]) {
                cameFrom[nextStr] = current.pos;
                gScore[nextStr] = tentative_gScore;

                double hScore = std::abs(next.x() - endStub.x()) + std::abs(next.y() - endStub.y());
                hScore *= 1.001;

                double fScore = tentative_gScore + hScore;
                openSet.push({next, tentative_gScore, fScore, normDir});
            }
        }
    }

    // ==========================================
    // ۳. بازسازی و فیلتر نقاط
    // ==========================================
    QVector<QPointF> rawPath;
    if (pathFound) {
        QPointF curr = finalPoint;
        rawPath.prepend(endPos);
        if (finalPoint != endStub) rawPath.prepend(endStub);

        while (curr != startStub && cameFrom.contains(pointToString(curr))) {
            rawPath.prepend(curr);
            curr = cameFrom[pointToString(curr)];
        }

        rawPath.prepend(startStub);
        rawPath.prepend(startPos);
    } else {
        rawPath.append(endPos);
    }

    QVector<QPointF> optimizedPath;
    if(rawPath.size() > 2) {
        optimizedPath.append(rawPath[0]);
        for(int i = 1; i < rawPath.size() - 1; i++) {
            QPointF prev = optimizedPath.last();
            QPointF curr = rawPath[i];
            QPointF next = rawPath[i+1];

            bool sameX = (qAbs(prev.x() - curr.x()) < 1) && (qAbs(curr.x() - next.x()) < 1);
            bool sameY = (qAbs(prev.y() - curr.y()) < 1) && (qAbs(curr.y() - next.y()) < 1);

            if (!sameX && !sameY) {
                optimizedPath.append(curr);
            }
        }
        optimizedPath.append(rawPath.last());
        return optimizedPath;
    }

    return rawPath;
}