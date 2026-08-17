#include "hex_upload_dialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>

HexUploadDialog::HexUploadDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Firmware Uploader (HEX)");
    resize(400, 120);

    // رابط انتخاب فایل
    label = new QLabel("Select Intel HEX File for MCU:", this);
    pathLineEdit = new QLineEdit(this);
    pathLineEdit->setReadOnly(true); // مسیر فقط خواندنی است.

    browseButton = new QPushButton("Browse...", this);
    okButton = new QPushButton("Load Firmware", this);

    // ردیف انتخاب فایل
    QHBoxLayout *fileSelectionLayout = new QHBoxLayout();
    fileSelectionLayout->addWidget(pathLineEdit);
    fileSelectionLayout->addWidget(browseButton);

    // چیدمان اصلی
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addLayout(fileSelectionLayout);
    mainLayout->addWidget(okButton);

    // رویدادهای پنجره
    connect(browseButton, &QPushButton::clicked, this, &HexUploadDialog::onBrowseClicked);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept); // تایید پنجره
}

void HexUploadDialog::onBrowseClicked() {
    // انتخاب فایل HEX
    QString filePath = QFileDialog::getOpenFileName(
            this,
            "Open Hex File",
            "",
            "Intel Hex Files (*.hex);;All Files (*.*)"
    );

    // مسیر معتبر را نگه دار.
    if (!filePath.isEmpty()) {
        pathLineEdit->setText(filePath);
    }
}

QString HexUploadDialog::getSelectedFilePath() const {
    return pathLineEdit->text();
}