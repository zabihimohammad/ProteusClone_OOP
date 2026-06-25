#include "mainwindow.h"
#include "../components/basic_components.h"
// هدرهای مربوط به بخش بوم گرافیکی (کدهایی که در گام دوم ساختیم) را این‌جا اینکلود می‌کنیم
#include "../canvas/circuit_scene.h"
#include "../canvas/circuit_view.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // ۱. تنظیم ویژگی‌های ظاهری پنجره اصلی نرم‌افزار
    setWindowTitle("Proteus Clone - OOP Project");
    resize(1024, 768); // ابعاد پیش‌فرض استاندارد برای نمایش مناسب قطعات مداری

                                                                        // ۲. ساخت نمونه (Instance) از بوم طراحی مدار (Scene)
                                                                                                                //فرستادن پارامتر `this` به سازنده، به Qt می‌گوید که پنجره اصلی مالک این شیء است
                                                                                                                                                                                             // و با بسته شدن برنامه، حافظه آن را به طور خودکار آزاد می‌کند (جلوگیری از Memory Leak).
    scene = new CircuitScene(this);

    //۳. ساخت نمونه از دوربین و کنترل‌کننده تعاملات بوم (View)
    view = new CircuitView(this);

    // ۴. متصل کردن بوم به دوربین (قلب تفکیک معماری گرافیکی)
    // با این کار، به شیء view فرمان می‌دهیم که اطلاعات، گریدها و قطعاتِ موجود در scene را نمایش دهد
    view->setScene(scene);
    Resistor *r1 = new Resistor();
    scene->addItem(r1);
    //۵. تنظیم دوربین به عنوان ویجت مرکزی (Central Widget) پنجره
                                                           //کلاس QMainWindow فضاهای مختلفی دارد (مثل نوار ابزار و وضعیت).
    // دستور setCentralWidget باعث می‌شود بوم شطرنجی شما تمام فضای اصلی و وسط برنامه را تصاحب کند.
                                                                                             // مزیت بزرگ این دستور این است که با تغییر سایز پنجره توسط کاربر، بوم هم به طور خودکار فیت می‌شود.
            setCentralWidget(view);
}

MainWindow::~MainWindow() {
    // به دلیل ساختار درختی مدیریت حافظه در Qt (روابط Parent-Child)،
    //اشیایی که در بالا با کلمه کلیدی new ساختیم و تفویض مالکیت کردیم،
    // موقع تخریب این کلاس به صورت خودکار و امن از حافظه پاک خواهند شد.
}