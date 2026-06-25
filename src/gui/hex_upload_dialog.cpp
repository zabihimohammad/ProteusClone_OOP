#include "hex_upload_dialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

HexUploadDialog::HexUploadDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Firmware Uploader (HEX)");
    resize(400, 120);

    // ساخت اجزای گرافیکی پنجره
    label = new QLabel("Select Intel HEX File for MCU:", this);
    pathLineEdit = new QLineEdit(this);
    pathLineEdit->setReadOnly(true); // کاربر نتواند دستی مسیر را دستکاری کند

    browseButton = new QPushButton("Browse...", this);
    okButton = new QPushButton("Load Firmware", this);

    // چیدمان افقی برای کادر متنی و دکمه Browse
    QHBoxLayout *fileSelectionLayout = new QHBoxLayout();
    fileSelectionLayout->addWidget(pathLineEdit);
    fileSelectionLayout->addWidget(browseButton);

    // چیدمان عمودی اصلی کل پنجره
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addLayout(fileSelectionLayout);
    mainLayout->addWidget(okButton);

    // متصل کردن سیگنال‌ها به اسلات‌ها (مکانیزم رویدادها در Qt)
    connect(browseButton, &QPushButton::clicked, this, &HexUploadDialog::onBrowseClicked);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept); // بستن پنجره با موفقیت
}

void HexUploadDialog::onBrowseClicked() {
    // باز کردن پنجره استاندارد ویندوز برای انتخاب فایل با فیلتر مخصوص .hex
    QString filePath = QFileDialog::getOpenFileName(
            this,
            "Open Hex File",
            "",
            "Intel Hex Files (*.hex);;All Files (*.*)"
    );

    // اگر کاربر فایلی را انتخاب کرد (پنجره را کنسل نکرد)
    if (!filePath.isEmpty()) {
        pathLineEdit->setText(filePath);
    }
}

QString HexUploadDialog::getSelectedFilePath() const {
    return pathLineEdit->text();
}