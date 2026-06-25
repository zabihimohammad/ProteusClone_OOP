#include "circuit_scene.h"
#include <QPen>
#include <QColor>

CircuitScene::CircuitScene(QObject *parent) : QGraphicsScene(parent) {
    gridSize = 20; // هر مربع شطرنجی ۲۰ در ۲۰ پیکسل خواهد بود

    // تنظیم ابعاد بی‌نهایت برای بوم (از مختصات -5000 تا +5000)
    // این کار باعث می‌شود کاربر بتواند مدارش را تا حد دلخواه گسترش دهد
    setSceneRect(-5000, -5000, 10000, 10000);
}

void CircuitScene::drawBackground(QPainter *painter, const QRectF &rect) {
    // ۱. ابتدا کل پس‌زمینه را با یک رنگ ملایم (مثلاً خاکستری بسیار روشن) پر می‌کنیم
    painter->fillRect(rect, QColor(245, 245, 245));

    // ۲. تنظیم قلم برای رسم نقاط گرید
    QPen pen;
    pen.setColor(QColor(200, 200, 200)); // رنگ خاکستری تیره برای نقاط
    pen.setWidth(1); // ضخامت نقاط
    painter->setPen(pen);

    // ۳. الگوریتم بهینه‌سازی رسم:
    // به جای اینکه کل 10000 پیکسل بوم را رسم کنیم، مختصات را محدود به ناحیه‌ای می‌کنیم
    // که الان جلوی چشم کاربر است (پارامتر rect ابعاد کادر دید فعلی را به ما می‌دهد).

    int left = int(rect.left()) - (int(rect.left()) % gridSize);
    int top = int(rect.top()) - (int(rect.top()) % gridSize);

    // ۴. رسم نقاط (Dots) در فواصل منظم
    // برای ساخت شبکه شطرنجی خطی، می‌توانید از ()painter->drawLine استفاده کنید،
    // اما نقطه (Point) برای نرم‌افزار مدار خلوت‌تر و زیباتر است.
    for (int x = left; x < rect.right(); x += gridSize) {
        for (int y = top; y < rect.bottom(); y += gridSize) {
            painter->drawPoint(x, y);
        }
    }
}