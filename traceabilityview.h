#ifndef TRACEABILITYVIEW_H
#define TRACEABILITYVIEW_H

#include <QWidget>
#include <QStandardItemModel>
#include "checkboxdelegate.h"
#include "documentview.h"
#include <QPair>
#include <QHash>

namespace Ui {
class TraceabilityView;
}

class TraceabilityView : public QWidget
{
    Q_OBJECT


public:
    explicit TraceabilityView(DocumentView *requirements, QWidget *parent = 0);
    ~TraceabilityView();

public slots:
    void addModels(DocumentView *);
    void addRowToDocument(DocumentView *, int);
    //void addSubRowToDocument(DocumentView *,int);
    void deleteRowOfDocument(DocumentView*, int, int count);
    void selectedDocumentChanged(int);
    void removeDocument(int index);
    void updateTrace(QModelIndex index);
    void tabChanged(int i);

    void itemSelected(const QModelIndex & current);

private:
    Ui::TraceabilityView *ui;  
    QStandardItemModel *model;
    QHash<DocumentView*, QStandardItemModel*> traceModelList;
    QList<QStandardItemModel*> noteModel;
    DocumentView *requirements;
    QStandardItemModel otherDocsModel;

    QStandardItemModel reqListModel;
    QStandardItemModel preReqModel;
    QStandardItemModel postReqModel;

    CheckBoxDelegate *del;

    QStandardItem *previous;
};

#endif // TRACEABILITYVIEW_H
