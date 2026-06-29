#include "properties_dialog.h"

PropertiesDialog::PropertiesDialog(const QString &title, const QMap<QString, QString> &properties, QWidget *parent)
        : QDialog(parent)
{
    setupUI(title, properties);
    applyStyles();
}

void PropertiesDialog::setupUI(const QString &title, const QMap<QString, QString> &properties) {
    setWindowTitle(title + " Properties");
    setMinimumWidth(350);

    QLabel *titleLabel = new QLabel(title + " Settings", this);
    titleLabel->setObjectName("titleLabel");

    QLabel *subTitleLabel = new QLabel("Adjust the technical parameters below:", this);
    subTitleLabel->setObjectName("subTitleLabel");

    // ==========================================
    // سیستم ساخت فرم داینامیک بر اساس ویژگی‌های قطعه
    // ==========================================
    QFormLayout *formLayout = new QFormLayout();

    // حلقه زدن روی تمام ویژگی‌های ارسالی از قطعه (مثلاً مقاومت، ولتاژ و...)
    for (auto it = properties.constBegin(); it != properties.constEnd(); ++it) {
        QLabel *fieldLabel = new QLabel(it.key() + ":", this);
        fieldLabel->setObjectName("fieldLabel");

        QLineEdit *edit = new QLineEdit(this);
        edit->setText(it.value()); // قرار دادن مقدار فعلی
        edit->setObjectName("valueInput");

        // 🛠️ رفع باگ گرافیکی: تضمین ارتفاع کافی برای فونت Consolas
        edit->setMinimumHeight(35);

        formLayout->addRow(fieldLabel, edit);
        inputFields.insert(it.key(), edit); // ذخیره اشاره‌گر برای خواندن مقدار در آینده
    }

    btnSave = new QPushButton("Save Changes", this);
    btnSave->setObjectName("btnSave");
    btnSave->setCursor(Qt::PointingHandCursor);

    btnCancel = new QPushButton("Cancel", this);
    btnCancel->setObjectName("btnCancel");
    btnCancel->setCursor(Qt::PointingHandCursor);

    connect(btnSave, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // هول دادن دکمه‌ها به سمت راست
    buttonLayout->addWidget(btnCancel);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(btnSave);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(15);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subTitleLabel);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(formLayout); // اضافه کردن فرم داینامیک
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

// تابع خواندن مقادیر جدید
QMap<QString, QString> PropertiesDialog::getUpdatedProperties() const {
    QMap<QString, QString> updatedProps;
    for (auto it = inputFields.constBegin(); it != inputFields.constEnd(); ++it) {
        updatedProps.insert(it.key(), it.value()->text());
    }
    return updatedProps;
}

void PropertiesDialog::applyStyles() {
    QString styleSheet = R"(
        QDialog { background-color: #1e1e2e; }
        QLabel#titleLabel { color: #ffffff; font-size: 18px; font-weight: bold; }
        QLabel#subTitleLabel { color: #a6accd; font-size: 12px; margin-bottom: 10px; }
        QLabel#fieldLabel { color: #ffffff; font-size: 13px; font-weight: bold; }

        /* استایل فیلدها با رفع باگ برش حروف */
        QLineEdit#valueInput {
            background-color: #2b2b40;
            color: #00ffcc;
            border: 2px solid #3a3a5a;
            border-radius: 6px;
            padding: 2px 10px; /* پدینگ بهینه شد */
            font-size: 14px;
            font-family: 'Consolas', monospace;
        }
        QLineEdit#valueInput:focus { border: 2px solid #00aaff; background-color: #32324a; }

        QPushButton#btnSave {
            background-color: #0077ff; color: white; border-radius: 6px; padding: 8px 15px; font-weight: bold;
        }
        QPushButton#btnSave:hover { background-color: #00aaff; }

        QPushButton#btnCancel {
            background-color: transparent; color: #a6accd; border: 2px solid #3a3a5a; border-radius: 6px; padding: 8px 15px; font-weight: bold;
        }
        QPushButton#btnCancel:hover { background-color: #3a3a5a; color: white; }
    )";
    this->setStyleSheet(styleSheet);
}