#include "traceabilityview.h"
#include "ui_traceabilityview.h"
#include <QDebug>
#include <QHeaderView>

TraceabilityView::TraceabilityView(DocumentView* requirements,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TraceabilityView)
{
    ui->setupUi(this);
    this->requirements = requirements;
    QObject::connect(requirements, SIGNAL(addRowToDocument(DocumentView*, int)), this, SLOT(addRowToDocument(DocumentView*, int)));
    QObject::connect(requirements, SIGNAL(deleteRowOfDocument(DocumentView*,int,int)), this, SLOT(deleteRowOfDocument(DocumentView*,int,int)));
    model = nullptr;

    ui->tableView->setModel(model);
    //ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::);

    del = new CheckBoxDelegate(this);
    ui->tableView->setItemDelegate(del);


    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::AnyKeyPressed);
    ui->tableView->horizontalHeader()->setSectionsClickable(false);
    ui->tableView->verticalHeader()->setSectionsClickable(false);
    ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->tableView->selectAll();

    ui->comboBox->setModel(&otherDocsModel);
    QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedDocumentChanged(int)));

    ui->reqList->setModel(&reqListModel);
    ui->preReq->setModel(&preReqModel);
    ui->postReq->setModel(&postReqModel);

    QObject::connect(ui->reqList, SIGNAL(clicked(QModelIndex)), this, SLOT(updateTrace(QModelIndex)));
    QObject::connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

}

void TraceabilityView::tabChanged(int i){

    if(i == 1)
        updateTrace(QModelIndex());

}

void TraceabilityView::updateTrace(QModelIndex index){

    int row = ui->reqList->currentIndex().row();
    qDebug() << "update trace row: " << row;
    if(row < 0)
        return;

    QList<QStandardItemModel*> list = traceModelList.values();
    preReqModel.clear();
    postReqModel.clear();

    for(int i = 0; i < list.size(); i++){

            QStringList listheader = traceModelList.keys().at(i)->getHeader();
            int column = list[i]->columnCount();
            for(int j = 0; j < column; j++){
                QStandardItem* item = list.at(i)->item(row, j);
                if(!item)
                    continue;
                if(item->data(Qt::EditRole).toInt() == 1)
                   preReqModel.insertRow(preReqModel.rowCount(), new QStandardItem(traceModelList.keys().at(i)->text() + " - " + traceModelList.keys().at(i)->getHeader().at(j).trimmed()));
                else if(item->data(Qt::EditRole).toInt() == 2)
                   postReqModel.insertRow(postReqModel.rowCount(),new QStandardItem(traceModelList.keys().at(i)->text() + " - " + traceModelList.keys().at(i)->getHeader().at(j).trimmed()));
            }
    }

}

void TraceabilityView::addModels(DocumentView *item){

    otherDocsModel.setItem(otherDocsModel.rowCount(), item);
    QStandardItemModel *model = new QStandardItemModel();
    model->setVerticalHeaderLabels(requirements->getHeader());
    traceModelList.insert(item, model);
    QObject::connect(item, SIGNAL(addRowToDocument(DocumentView*, int)), this, SLOT(addRowToDocument(DocumentView*, int)));
    QObject::connect(item, SIGNAL(deleteRowOfDocument(DocumentView*,int, int)), this, SLOT(deleteRowOfDocument(DocumentView*,int, int)));
    if(this->model == nullptr){
        this->model = model;
        ui->tableView->setModel(model);
        ui->comboBox->setCurrentIndex(0);

    }
}

void TraceabilityView::addRowToDocument(DocumentView *item, int row){

    if(item == requirements){
        QList<QStandardItemModel*> list = traceModelList.values();
        for(int i = 0; i < list.size(); i++){
                if(row >= 0)
                    list[i]->insertRow(row);
                list[i]->setVerticalHeaderLabels(requirements->getHeader());
        }
          reqListModel.clear();
          for(QString str : requirements->getHeader()){
              QStandardItem *tempitem = new QStandardItem(str.trimmed());
              tempitem->setEditable(false);
              reqListModel.insertRow(reqListModel.rowCount(), tempitem);
          }
        return;
    }

    QStandardItemModel *model = traceModelList.value(item);
    if(row >= 0)
        model->insertColumn(row);
    model->setHorizontalHeaderLabels(item->getHeader());
}

void TraceabilityView::deleteRowOfDocument(DocumentView *item, int row, int count){

    if(item == requirements){
        QList<QStandardItemModel*> list = traceModelList.values();
        for(int i = 0; i < list.size(); i++)
                list[i]->removeRows(row, count);
        reqListModel.removeRows(row, count);
        return;
    }

    QStandardItemModel *model = traceModelList.value(item);
    model->removeColumns(row, count);

}

void TraceabilityView::selectedDocumentChanged(int index){

    DocumentView *item = static_cast<DocumentView*> (otherDocsModel.item(index));
    QStandardItemModel *model = traceModelList.value(item);

    this->model = model;
    ui->tableView->setModel(model);
}

void TraceabilityView::removeDocument(int index){

    QStandardItem *item = otherDocsModel.item(index);
    otherDocsModel.removeRow(index);

    traceModelList.remove(static_cast<DocumentView*>(item));
}

TraceabilityView::~TraceabilityView()
{
    delete ui;
}
