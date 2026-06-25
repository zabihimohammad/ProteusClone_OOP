#include "auto_router.h"
#include "terminal.h"
#include "wire.h"
#include <QMap>
#include <QSet>
#include <QGraphicsItem>
#include <QLineF>
#include <queue>
#include <vector>

QString pointToString(const QPointF &p) {
    return QString("%1,%2").arg(qRound(p.x())).arg(qRound(p.y()));
}

struct AStarNode {
    QPointF pos;
    double gCost;
    double fCost;
    QPointF dir;
    bool operator>(const AStarNode& other) const {
        return fCost > other.fCost;
    }
};

// ==========================================
// تابع فوق‌هوشمند برای تشخیص جهتِ خروج سیم از قطعه
// ==========================================
QPointF getPinDirection(Terminal* term) {
    if (!term || !term->parentItem()) return QPointF(20, 0); // پیش‌فرض

    QPointF local = term->pos(); // مختصات پایه نسبت به مرکز قطعه

    // استثنا برای قطعه زمین (GND) که در مرکز است اما باید سیم از بالا واردش شود
    if (local.x() == 0 && local.y() == 0) return QPointF(0, -20);

    // بررسی اینکه پایه در کدام لبه قطعه قرار دارد تا جهت خروج را تعیین کند
    if (std::abs(local.x()) >= std::abs(local.y())) {
        return local.x() > 0 ? QPointF(20, 0) : QPointF(-20, 0);
    } else {
        return local.y() > 0 ? QPointF(0, 20) : QPointF(0, -20);
    }
}

QVector<QPointF> AutoRouter::findPath(QGraphicsScene *scene, QPointF startPos, QPointF endPos, Terminal *startTerm, Terminal *endTerm) {
    int stepSize = 10;
    int maxIterations = 5000;

    // ۱. ایجاد نقاط استاب (Stubs): نقاطی در ۲۰ پیکسلی روبروی پایه‌ها
    QPointF startDir = getPinDirection(startTerm);
    QPointF endDir = getPinDirection(endTerm);
    QPointF startStub = startPos + startDir;
    QPointF endStub = endPos + endDir;

    // اگر نقاط استاب خیلی به هم نزدیک بودند، مستقیما وصلشان کن
    if (startStub == endStub) return {startPos, startStub, endPos};

    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    QMap<QString, QPointF> cameFrom;
    QMap<QString, double> gScore;
    QSet<QString> closedSet;

    // هوش مصنوعی حالا جستجو را از "استاب مبدا" شروع می‌کند
    openSet.push({startStub, 0, 0, QPointF(0,0)});
    gScore[pointToString(startStub)] = 0;

    bool pathFound = false;
    QPointF finalPoint;

    QVector<QPointF> directions = {
            QPointF(0, -stepSize), QPointF(0, stepSize),
            QPointF(-stepSize, 0), QPointF(stepSize, 0)
    };

    int iterations = 0;
    while (!openSet.empty() && iterations < maxIterations) {
        AStarNode current = openSet.top();
        openSet.pop();
        iterations++;

        QString currentStr = pointToString(current.pos);
        if (closedSet.contains(currentStr)) continue;
        closedSet.insert(currentStr);

        // هدف هوش مصنوعی رسیدن به "استاب مقصد" است
        if (QLineF(current.pos, endStub).length() < 5.0) {
            pathFound = true;
            finalPoint = current.pos;
            break;
        }

        for (const QPointF &dir : directions) {
            QPointF next = current.pos + dir;
            QString nextStr = pointToString(next);

            if (closedSet.contains(nextStr)) continue;

            QRectF checkRect(next.x() - 2, next.y() - 2, 4, 4);
            QList<QGraphicsItem*> itemsAtNext = scene->items(checkRect);

            bool hitObstacle = false;
            double wirePenalty = 0.0;

            for (QGraphicsItem *item : itemsAtNext) {
                if (dynamic_cast<Wire*>(item)) {
                    wirePenalty += 400.0; // جریمه سنگین برای دور ماندن از سیم‌های دیگر
                    continue;
                }
                if (dynamic_cast<Terminal*>(item)) continue;

                QGraphicsItem *parent = item->parentItem() ? item->parentItem() : item;

                // چون سیم بین استاب‌ها کشیده می‌شود، برخورد با هر قطعه‌ای (حتی مبدا و مقصد) ممنوع است!
                // این خط باعث می‌شود سیم هرگز وارد شکم قطعات نشود.
                if (parent == startTerm->parentItem() || parent == endTerm->parentItem()) {
                    if (QLineF(next, startStub).length() <= 15.0 || QLineF(next, endStub).length() <= 15.0) {
                        continue; // فقط اطراف خودِ استاب‌ها مجاز است
                    }
                }

                hitObstacle = true;
                break;
            }

            if (hitObstacle) continue;

            // جریمه تغییر زاویه برای داشتن خطوط صاف
            double turnPenalty = (current.dir != QPointF(0,0) && current.dir != dir) ? 80.0 : 0.0;
            double tentative_gScore = gScore[currentStr] + stepSize + turnPenalty + wirePenalty;

            if (!gScore.contains(nextStr) || tentative_gScore < gScore[nextStr]) {
                cameFrom[nextStr] = current.pos;
                gScore[nextStr] = tentative_gScore;

                double hScore = std::abs(next.x() - endStub.x()) + std::abs(next.y() - endStub.y());

                // فرمول جادویی Tie-breaker
                double dx1 = next.x() - endStub.x();
                double dy1 = next.y() - endStub.y();
                double dx2 = startStub.x() - endStub.x();
                double dy2 = startStub.y() - endStub.y();
                hScore += std::abs(dx1 * dy2 - dx2 * dy1) * 0.005;

                double fScore = tentative_gScore + hScore;
                openSet.push({next, tentative_gScore, fScore, dir});
            }
        }
    }

    QVector<QPointF> rawPath;
    if (pathFound) {
        QPointF curr = finalPoint;
        rawPath.prepend(endPos);   // اتصال نهایی خط کش‌کشی شده به پایه مقصد
        if (finalPoint != endStub) rawPath.prepend(endStub);

        while (curr != startStub && cameFrom.contains(pointToString(curr))) {
            rawPath.prepend(curr);
            curr = cameFrom[pointToString(curr)];
        }

        rawPath.prepend(startStub);
        rawPath.prepend(startPos); // اتصال خط کش‌کشی شده مبدا
    } else {
        rawPath.append(endPos);
    }

    // ==========================================
    // ۲. فیلتر بهینه‌ساز برای حذف نقاط اضافی و جلوگیری از زیگ‌زاگ‌های ریز
    // ==========================================
    QVector<QPointF> optimizedPath;
    if(rawPath.size() > 2) {
        optimizedPath.append(rawPath[0]);
        for(int i = 1; i < rawPath.size() - 1; i++) {
            QPointF prev = optimizedPath.last();
            QPointF curr = rawPath[i];
            QPointF next = rawPath[i+1];

            // اگر سه نقطه در یک خط صاف هستند، نقطه وسطی را حذف کن
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