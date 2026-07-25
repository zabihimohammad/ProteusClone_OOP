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
#include "element.h"

// 🛠️ حل قطعی باگ تلرانس اعشاری در هش‌مپ
QString pointToString(const QPointF &p) {
    return QString("%1,%2").arg(qRound(p.x())).arg(qRound(p.y()));
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

QPointF getPinDirection(Terminal* term) {
    if (!term || !term->parentItem()) return QPointF(20, 0);

    Element *el = dynamic_cast<Element*>(term->parentItem());
    if (el && el->getComponentName() == "Junction Node") {
        return QPointF(0, 0);
    }

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

QVector<QPointF> AutoRouter::findPath(QGraphicsScene *scene, QPointF startPos, QPointF endPos, Terminal *startTerm, Terminal *endTerm, Wire *currentWire) {
    double stepSize = 10.0;
    int maxIterations = 10000;

    QPointF startDir = getPinDirection(startTerm);
    QPointF endDir = getPinDirection(endTerm);
    QPointF startStub = startPos + startDir;
    QPointF endStub = endPos + endDir;

    if (qAbs(startStub.x() - endStub.x()) < 1.0 && qAbs(startStub.y() - endStub.y()) < 1.0) {
        return {startPos, endPos};
    }

    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    QMap<QString, QPointF> cameFrom;
    QMap<QString, double> gScore;
    QSet<QString> closedSet;

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

        // 🛠️ تضمین رسیدن به مقصد با بررسی تلرانس (حل مشکل متوقف شدن زودتر از موعد سیم)
        if (qAbs(current.pos.x() - endStub.x()) < 1.0 && qAbs(current.pos.y() - endStub.y()) < 1.0) {
            pathFound = true;
            finalPoint = current.pos;
            break;
        }

        QVector<QPointF> nextPoints = {
                current.pos + QPointF(0, -stepSize), current.pos + QPointF(0, stepSize),
                current.pos + QPointF(-stepSize, 0), current.pos + QPointF(stepSize, 0)
        };

        if (qAbs(current.pos.x() - endStub.x()) > 0.1 && qAbs(current.pos.x() - endStub.x()) <= stepSize) {
            nextPoints.append(QPointF(endStub.x(), current.pos.y()));
        }
        if (qAbs(current.pos.y() - endStub.y()) > 0.1 && qAbs(current.pos.y() - endStub.y()) <= stepSize) {
            nextPoints.append(QPointF(current.pos.x(), endStub.y()));
        }

        for (const QPointF &next : nextPoints) {
            QPointF moveDir = next - current.pos;
            QPointF normDir(0, 0);
            if (qAbs(moveDir.x()) > 0) normDir.setX(moveDir.x() > 0 ? 1 : -1);
            if (qAbs(moveDir.y()) > 0) normDir.setY(moveDir.y() > 0 ? 1 : -1);

            if (current.dir != QPointF(0,0) && normDir == QPointF(-current.dir.x(), -current.dir.y())) {
                continue;
            }

            QString nextStr = pointToString(next);
            if (closedSet.contains(nextStr)) continue;

            QRectF checkRect(next.x() - 1, next.y() - 1, 2, 2);
            QList<QGraphicsItem*> itemsAtNext = scene->items(checkRect, Qt::IntersectsItemShape);

            bool hitObstacle = false;
            double wirePenalty = 0.0;

            // اگر نقطه بعدی دقیقاً همان مقصد ماست، آن را مجاز می‌شماریم
            bool isTarget = (qAbs(next.x() - endStub.x()) < 1.0 && qAbs(next.y() - endStub.y()) < 1.0);

            for (QGraphicsItem *item : itemsAtNext) {
                Wire *wireItem = dynamic_cast<Wire*>(item);
                if (wireItem) {
                    if (wireItem == currentWire) continue;
                    wirePenalty += 15.0;
                    continue;
                }

                if (dynamic_cast<Terminal*>(item)) continue;

                QGraphicsItem *parent = item->parentItem() ? item->parentItem() : item;

                if (parent == startTerm->parentItem() || parent == endTerm->parentItem()) {
                    if (isTarget) continue; // نقطه مقصد را مانع در نظر نگیر!
                    if (QLineF(next, startStub).length() <= 15.0 || QLineF(next, endStub).length() <= 15.0) {
                        continue;
                    }
                }

                hitObstacle = true;
                break;
            }

            if (hitObstacle) continue;

            double turnPenalty = (current.dir != QPointF(0,0) && current.dir != normDir) ? 10.0 : 0.0;
            double stepCost = qAbs(moveDir.x()) + qAbs(moveDir.y());
            double tentative_gScore = gScore[currentStr] + stepCost + turnPenalty + wirePenalty;

            if (!gScore.contains(nextStr) || tentative_gScore < gScore[nextStr]) {
                cameFrom[nextStr] = current.pos;
                gScore[nextStr] = tentative_gScore;

                double hScore = std::abs(next.x() - endStub.x()) + std::abs(next.y() - endStub.y());
                hScore *= 1.001;

                openSet.push({next, tentative_gScore, tentative_gScore + hScore, normDir});
            }
        }
    }

    QVector<QPointF> rawPath;
    if (pathFound) {
        QPointF curr = finalPoint;
        while (curr != startStub && cameFrom.contains(pointToString(curr))) {
            rawPath.prepend(curr);
            curr = cameFrom[pointToString(curr)];
        }
        rawPath.prepend(startStub);
        rawPath.prepend(startPos);

        // اضافه کردن مستقیمِ نقطه پایانی برای حذف قطعی باگ "فاصله داشتن سیم"
        if (qAbs(rawPath.last().x() - endStub.x()) > 0.1 || qAbs(rawPath.last().y() - endStub.y()) > 0.1) {
            rawPath.append(endStub);
        }
        if (qAbs(rawPath.last().x() - endPos.x()) > 0.1 || qAbs(rawPath.last().y() - endPos.y()) > 0.1) {
            rawPath.append(endPos);
        }

        for (int i = 0; i < rawPath.size() - 1; ) {
            if (qAbs(rawPath[i].x() - rawPath[i+1].x()) < 0.5 && qAbs(rawPath[i].y() - rawPath[i+1].y()) < 0.5) {
                rawPath.removeAt(i+1);
            } else {
                i++;
            }
        }

        // 🛠️ ایجاد زاویه ۹۰ درجه استاندارد در اتصال نهایی به گره
        if (rawPath.size() >= 2) {
            QPointF pLast = rawPath.last();
            QPointF pPrev = rawPath[rawPath.size() - 2];

            if (qAbs(pPrev.x() - pLast.x()) > 0.5 && qAbs(pPrev.y() - pLast.y()) > 0.5) {
                QPointF corner;
                if (rawPath.size() >= 3) {
                    QPointF pPrevPrev = rawPath[rawPath.size() - 3];
                    if (qAbs(pPrevPrev.x() - pPrev.x()) < 0.5) {
                        corner = QPointF(pPrev.x(), pLast.y());
                    } else {
                        corner = QPointF(pLast.x(), pPrev.y());
                    }
                } else {
                    corner = QPointF(pPrev.x(), pLast.y());
                }
                rawPath.insert(rawPath.size() - 1, corner);
            }
        }
    } else {
        // در صورت عدم یافتن مسیر، سیم را به صورت مستقیم متصل کن تا معلق نماند
        rawPath = {startPos, startStub, endStub, endPos};
    }

    QVector<QPointF> optimizedPath;
    if (rawPath.size() > 2) {
        optimizedPath.append(rawPath[0]);
        for (int i = 1; i < rawPath.size() - 1; i++) {
            QPointF prev = optimizedPath.last();
            QPointF curr = rawPath[i];
            QPointF next = rawPath[i+1];

            bool sameX = (qAbs(prev.x() - curr.x()) < 0.5) && (qAbs(curr.x() - next.x()) < 0.5);
            bool sameY = (qAbs(prev.y() - curr.y()) < 0.5) && (qAbs(curr.y() - next.y()) < 0.5);

            if (!sameX && !sameY) {
                optimizedPath.append(curr);
            }
        }
        optimizedPath.append(rawPath.last());
        return optimizedPath;
    }

    return rawPath;
}