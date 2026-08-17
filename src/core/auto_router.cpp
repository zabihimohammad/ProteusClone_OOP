#include "auto_router.h"

#include "element.h"
#include "terminal.h"
#include "wire.h"

#include <QGraphicsItem>
#include <QLineF>
#include <QMap>
#include <QSet>

#include <algorithm>
#include <cmath>
#include <queue>
#include <vector>

namespace {
// تنظیمات مسیریابی
constexpr qreal kStepSize = 10.0;
constexpr qreal kStubLength = 20.0;
constexpr qreal kComponentClearance = 6.0;
constexpr int kMaximumIterations = 20000;

struct RouteNode {
    QPointF position;
    qreal pathCost;
    qreal totalCost;
    QPointF direction;

    bool operator>(const RouteNode &other) const
    {
        if (qAbs(totalCost - other.totalCost) < 0.1)
            return pathCost < other.pathCost;
        return totalCost > other.totalCost;
    }
};

QString pointKey(const QPointF &point)
{
    return QString::number(qRound(point.x())) + ',' + QString::number(qRound(point.y()));
}

QPointF normalizedDirection(const QPointF &direction)
{
    if (qAbs(direction.x()) > qAbs(direction.y()))
        return {direction.x() >= 0.0 ? 1.0 : -1.0, 0.0};
    if (!qFuzzyIsNull(direction.y()))
        return {0.0, direction.y() >= 0.0 ? 1.0 : -1.0};
    return {};
}

QPointF pinDirection(Terminal *terminal)
{
    // جهت خروج سیم از پایه
    if (!terminal || !terminal->parentItem())
        return {kStubLength, 0.0};

    auto *element = dynamic_cast<Element *>(terminal->parentItem());
    if (element && element->getComponentName() == "Junction Node")
        return {};

    QGraphicsItem *parent = terminal->parentItem();
    const QPointF localPosition = terminal->pos();
    const QRectF bounds = parent->boundingRect();

    const qreal left = qAbs(localPosition.x() - bounds.left());
    const qreal right = qAbs(localPosition.x() - bounds.right());
    const qreal top = qAbs(localPosition.y() - bounds.top());
    const qreal bottom = qAbs(localPosition.y() - bounds.bottom());
    const qreal closest = std::min({left, right, top, bottom});

    QPointF localDirection;
    if (closest == left)
        localDirection = {-1.0, 0.0};
    else if (closest == right)
        localDirection = {1.0, 0.0};
    else if (closest == top)
        localDirection = {0.0, -1.0};
    else
        localDirection = {0.0, 1.0};

    const qreal requiredLength = qMax(kStubLength, closest + kComponentClearance + 4.0);
    const QPointF sceneOrigin = parent->mapToScene(localPosition);
    const QPointF sceneDirection = parent->mapToScene(localPosition + localDirection) - sceneOrigin;
    return normalizedDirection(sceneDirection) * requiredLength;
}

QVector<QRectF> componentObstacles(QGraphicsScene *scene)
{
    // محدوده امن دور قطعات
    QVector<QRectF> obstacles;
    if (!scene)
        return obstacles;

    for (QGraphicsItem *item : scene->items()) {
        auto *element = dynamic_cast<Element *>(item);
        if (!element || !element->isVisible())
            continue;
        if (element->getComponentName() == "Junction Node")
            continue;

        obstacles.append(element->sceneBoundingRect().adjusted(
            -kComponentClearance, -kComponentClearance,
             kComponentClearance,  kComponentClearance));
    }
    return obstacles;
}

QRectF segmentBounds(const QPointF &first, const QPointF &second)
{
    return QRectF(first, second).normalized().adjusted(-1.0, -1.0, 1.0, 1.0);
}

bool hitsComponent(const QPointF &first, const QPointF &second,
                   const QVector<QRectF> &obstacles)
{
    const QRectF segment = segmentBounds(first, second);
    for (const QRectF &obstacle : obstacles) {
        if (obstacle.intersects(segment))
            return true;
    }
    return false;
}

qreal wireCrossingPenalty(QGraphicsScene *scene, const QPointF &first,
                          const QPointF &second, Wire *currentWire)
{
    // عبور از سیم‌های دیگر هزینه دارد.
    qreal penalty = 0.0;
    const QList<QGraphicsItem *> nearbyItems =
        scene->items(segmentBounds(first, second), Qt::IntersectsItemShape);

    for (QGraphicsItem *item : nearbyItems) {
        auto *wire = dynamic_cast<Wire *>(item);
        if (wire && wire != currentWire)
            penalty += 15.0;
    }
    return penalty;
}

void appendIfDifferent(QVector<QPointF> &path, const QPointF &point)
{
    if (path.isEmpty() || QLineF(path.last(), point).length() > 0.1)
        path.append(point);
}

QVector<QPointF> simplifyPath(const QVector<QPointF> &path)
{
    // نقاط میانی اضافی را حذف کن.
    if (path.size() < 3)
        return path;

    QVector<QPointF> result;
    result.append(path.first());
    for (int i = 1; i < path.size() - 1; ++i) {
        const QPointF previous = result.last();
        const QPointF current = path.at(i);
        const QPointF next = path.at(i + 1);
        const bool sameX = qAbs(previous.x() - current.x()) < 0.5 &&
                           qAbs(current.x() - next.x()) < 0.5;
        const bool sameY = qAbs(previous.y() - current.y()) < 0.5 &&
                           qAbs(current.y() - next.y()) < 0.5;
        if (!sameX && !sameY)
            appendIfDifferent(result, current);
    }
    appendIfDifferent(result, path.last());
    return result;
}

QVector<QPointF> fallbackPath(const QPointF &startPosition, const QPointF &startStub,
                              const QPointF &endStub, const QPointF &endPosition,
                              const QVector<QRectF> &obstacles)
{
    // مسیر ساده برای حالت بدون جواب
    const QPointF horizontalCorner(endStub.x(), startStub.y());
    const QPointF verticalCorner(startStub.x(), endStub.y());

    const bool horizontalClear = !hitsComponent(startStub, horizontalCorner, obstacles) &&
                                 !hitsComponent(horizontalCorner, endStub, obstacles);
    const bool verticalClear = !hitsComponent(startStub, verticalCorner, obstacles) &&
                               !hitsComponent(verticalCorner, endStub, obstacles);

    QVector<QPointF> path;
    appendIfDifferent(path, startPosition);
    appendIfDifferent(path, startStub);
    if (horizontalClear || !verticalClear)
        appendIfDifferent(path, horizontalCorner);
    else
        appendIfDifferent(path, verticalCorner);
    appendIfDifferent(path, endStub);
    appendIfDifferent(path, endPosition);
    return simplifyPath(path);
}
}

QVector<QPointF> AutoRouter::findPath(QGraphicsScene *scene, QPointF startPosition,
                                      QPointF endPosition, Terminal *startTerminal,
                                      Terminal *endTerminal, Wire *currentWire)
{
    // مسیر عمودی و افقی بین دو پایه
    if (!scene || !startTerminal || !endTerminal)
        return {startPosition, endPosition};

    const QPointF startDirection = pinDirection(startTerminal);
    const QPointF endDirection = pinDirection(endTerminal);
    const QPointF startStub = startPosition + startDirection;
    const QPointF endStub = endPosition + endDirection;
    const QVector<QRectF> obstacles = componentObstacles(scene);

    if (QLineF(startStub, endStub).length() < 0.5)
        return simplifyPath({startPosition, startStub, endPosition});

    QRectF searchArea(startStub, endStub);
    searchArea = searchArea.normalized().adjusted(-400.0, -400.0, 400.0, 400.0);
    for (const QRectF &obstacle : obstacles)
        searchArea = searchArea.united(obstacle.adjusted(-80.0, -80.0, 80.0, 80.0));

    std::priority_queue<RouteNode, std::vector<RouteNode>, std::greater<RouteNode>> openSet;
    QMap<QString, QPointF> cameFrom;
    QMap<QString, qreal> pathScores;
    QSet<QString> closedSet;

    const QPointF initialDirection = normalizedDirection(startDirection);
    openSet.push({startStub, 0.0, 0.0, initialDirection});
    pathScores.insert(pointKey(startStub), 0.0);

    bool found = false;
    QPointF finalPoint;
    int iterations = 0;

    while (!openSet.empty() && iterations < kMaximumIterations) {
        const RouteNode current = openSet.top();
        openSet.pop();
        ++iterations;

        const QString currentKey = pointKey(current.position);
        if (closedSet.contains(currentKey))
            continue;
        closedSet.insert(currentKey);

        if (QLineF(current.position, endStub).length() < 0.5) {
            found = true;
            finalPoint = current.position;
            break;
        }

        QVector<QPointF> nextPoints{
            current.position + QPointF(0.0, -kStepSize),
            current.position + QPointF(0.0,  kStepSize),
            current.position + QPointF(-kStepSize, 0.0),
            current.position + QPointF( kStepSize, 0.0)
        };

        const qreal xDistance = qAbs(current.position.x() - endStub.x());
        const qreal yDistance = qAbs(current.position.y() - endStub.y());
        if (xDistance > 0.1 && xDistance <= kStepSize)
            nextPoints.append({endStub.x(), current.position.y()});
        if (yDistance > 0.1 && yDistance <= kStepSize)
            nextPoints.append({current.position.x(), endStub.y()});

        for (const QPointF &next : nextPoints) {
            if (!searchArea.contains(next))
                continue;

            const QPointF moveDirection = normalizedDirection(next - current.position);
            if (!current.direction.isNull() &&
                moveDirection == QPointF(-current.direction.x(), -current.direction.y()))
                continue;

            const QString nextKey = pointKey(next);
            if (closedSet.contains(nextKey))
                continue;
            if (hitsComponent(current.position, next, obstacles))
                continue;

            const qreal distance = qAbs(next.x() - current.position.x()) +
                                   qAbs(next.y() - current.position.y());
            const qreal turnPenalty = !current.direction.isNull() &&
                                      current.direction != moveDirection ? 12.0 : 0.0;
            const qreal crossingPenalty =
                wireCrossingPenalty(scene, current.position, next, currentWire);
            const qreal newScore = pathScores.value(currentKey) + distance +
                                   turnPenalty + crossingPenalty;

            if (!pathScores.contains(nextKey) || newScore < pathScores.value(nextKey)) {
                cameFrom.insert(nextKey, current.position);
                pathScores.insert(nextKey, newScore);
                const qreal remaining = qAbs(next.x() - endStub.x()) +
                                        qAbs(next.y() - endStub.y());
                openSet.push({next, newScore, newScore + remaining * 1.001, moveDirection});
            }
        }
    }

    if (!found)
        return fallbackPath(startPosition, startStub, endStub, endPosition, obstacles);

    QVector<QPointF> path;
    QPointF current = finalPoint;
    path.prepend(current);
    while (QLineF(current, startStub).length() >= 0.5) {
        const QString key = pointKey(current);
        if (!cameFrom.contains(key))
            return fallbackPath(startPosition, startStub, endStub, endPosition, obstacles);
        current = cameFrom.value(key);
        path.prepend(current);
    }

    path.prepend(startPosition);
    appendIfDifferent(path, endStub);
    appendIfDifferent(path, endPosition);
    return simplifyPath(path);
}
