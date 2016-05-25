#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QLineEdit>

namespace Ui {
class DocumentView;
}

class DocumentView : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentView(QWidget *parent = 0);
    ~DocumentView();

public slots:
    void addColumn();
    void addRow();

private:
    Ui::DocumentView *ui;
    QStandardItemModel *model;
};

#endif // DOCUMENTVIEW_H
