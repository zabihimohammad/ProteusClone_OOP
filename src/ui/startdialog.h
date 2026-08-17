#pragma once

#include <QDialog>
#include <QSize>

class QLineEdit;
class QListWidget;
class QComboBox;
class QSpinBox;
class QPushButton;

// صفحه شروع و پروژه‌های اخیر
class StartDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = nullptr);
    QString projectName() const;
    QString projectFilePath() const;
    QSize canvasSize() const;
    void refreshRecentProjects();
    void setBackAvailable(bool available);

signals:
    void backRequested();

private:
    void createProject();
    void openProject();
    void applyProjectMetadata(const QString &path);
    void loadRecentProjects();
    void rememberProject(const QString &path);

    QLineEdit *m_name = nullptr;
    QLineEdit *m_location = nullptr;
    QComboBox *m_sizePreset = nullptr;
    QSpinBox *m_width = nullptr;
    QSpinBox *m_height = nullptr;
    QListWidget *m_recent = nullptr;
    QPushButton *m_back = nullptr;
    QString m_projectFilePath;
};
