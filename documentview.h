#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QLineEdit>
#include <QMouseEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QItemSelection>

namespace Ui {
class DocumentView;
}

class DocumentView : public QWidget
{
    Q_OBJECT
    QMessageBox* msgBox;

public:
    explicit DocumentView(QWidget *parent = 0);
    ~DocumentView();

public slots:
    void addColumn();
    void addRow();
    void deleteColumn();
    void deleteRow();
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::DocumentView *ui;
    QStandardItemModel *model;
};

#endif // DOCUMENTVIEW_H
