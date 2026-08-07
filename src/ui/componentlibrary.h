#pragma once

#include <QFrame>
#include <QString>
#include <QVector>
#include <QPoint>
class QLabel;
class QLineEdit;
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;

struct ComponentInfo
{
    QString id;
    QString name;
    QString category;
    QString description;
    QString symbol;
};

class ComponentLibrary final : public QFrame
{
    Q_OBJECT

public:
    explicit ComponentLibrary(QWidget *parent = nullptr);

signals:
    void componentActivated(QString componentId);

private slots:
    void rebuildTree(const QString &query = {});
    void updatePreview(QTreeWidgetItem *current, QTreeWidgetItem *previous);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    QVector<ComponentInfo> m_components;
    QLineEdit *m_search = nullptr;
    QTreeWidget *m_tree = nullptr;
    QLabel *m_symbol = nullptr;
    QLabel *m_name = nullptr;
    QLabel *m_description = nullptr;
    QLabel *m_empty = nullptr;
    QListWidget *m_active = nullptr;
    QPoint m_dragStartPos; // ذخیره مختصات اولیه کلیک موس
};
