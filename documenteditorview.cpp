#include "documenteditorview.h"
#include "ui_documenteditorview.h"
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QLineEdit>
#include <QDir>
#include <xmlhandler.h>
#include <QXmlStreamWriter>
#include <iostream>
#include <QDomDocument>

DocumentEditorView::DocumentEditorView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DocumentEditorView)
{
    ui->setupUi(this);
    QObject::connect(ui->addDocButton, SIGNAL(pressed()), this, SLOT(addNewTab()));
    QObject::connect(ui->deleteButton, SIGNAL(pressed()), this, SLOT(deleteTab()));
    QObject::connect(ui->loadDocButton, SIGNAL(pressed()), this, SLOT(loadTab()));
    QObject::connect(ui->saveButton, SIGNAL(pressed()), this, SLOT(saveTab()));
}

DocumentEditorView::~DocumentEditorView()
{
    delete ui;
}

void DocumentEditorView::saveTab(){

   if(ui->tabWidget->currentIndex() < 0)
       return;

   QString fileName =  QFileDialog::getSaveFileName(this);

   std::list<DocumentView*>::iterator it = documentlist.begin();
   std::advance(it, ui->tabWidget->currentIndex());
   QStandardItemModel *model = (*it)->getModel();
   QStringList header = (*it)->getColumnNames();

   QDomDocument doc;

   QDomElement el = doc.createElement("document");
   el.appendChild(doc.createTextNode((*it)->getName()));
   doc.appendChild(el);

   QStandardItem *root = model->invisibleRootItem();

   for(int i = 0; i < root->rowCount(); i++){  

       QDomElement subel = doc.createElement("row");
       el.appendChild(subel);

       for(int j = 0; j < root->columnCount(); j++){
            QStandardItem *item = root->child(i, j);
            QDomElement subsub =  doc.createElement(header.at(j));
            if(item)
                subsub.appendChild(doc.createTextNode(item->text()));
            else
                subsub.appendChild(doc.createTextNode(""));
            subel.appendChild(subsub);
       }

       QStandardItem *item = root->child(i);


       for(int j = 0; j < item->rowCount(); j++){

           QDomElement subsubel = doc.createElement("subrow");
           subel.appendChild(subsubel);

           for(int k = 0; k < root->columnCount(); k++){

                QStandardItem *subitem = item->child(j, k);
                QDomElement subsub =  doc.createElement(header.at(k));
                if(subitem)
                    subsub.appendChild(doc.createTextNode(subitem->text()));
                else
                    subsub.appendChild(doc.createTextNode(""));
                subsubel.appendChild(subsub);
           }
      }
   }


   QFile file(fileName);
   if( file.exists())
      file.remove();

   if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
          return;

    file.write(doc.toByteArray());
    file.close();
   //readXml(fileName);
}

void DocumentEditorView::loadTab(){

   QDomDocument doc;
    QString filename = QFileDialog::getOpenFileName(this);
    QFile file(filename); //filename

    if (!file.open(QIODevice::ReadOnly | QFile::Text)){
        std::cerr<< "ERROR: file openning"<< std::endl;
        return;
    }

    QString message;

    if (!doc.setContent(&file ,false, &message)) {
        std::cerr << "ERROR: dom document open: " << message.toStdString() << std::endl;
        file.close();
        return;
    }

    file.close();

    QDomElement docEl = doc.documentElement();
    if (docEl.tagName() != "document") {
            std::cerr << "Error: Not a bookindex file" << std::endl;
            return ;
    }


    //Document NAME - HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    qDebug(docEl.toElement().firstChild().nodeValue().toLatin1());

    QString docName = docEl.toElement().firstChild().nodeValue();
    DocumentView *newdoc = new DocumentView(docName, this);
    ui->tabWidget->addTab(newdoc, docName);
    documentlist.push_back(newdoc);
    docAdded(newdoc);
    QStandardItemModel *model = newdoc->getModel();
    QStandardItem *root = model->invisibleRootItem();

    QStringList headlist;
    bool headlistdone = false;

    QDomNode child = docEl.firstChild().nextSibling();
    int rown = -1;
    while (!child.isNull())
    {
        //row element
        qDebug(" "+child.toElement().tagName().toLatin1());

        newdoc->addRow();
        rown++;

        QDomNode childNode = child.firstChild();
        int column = -1;
        int subrow = -1;
        while(!childNode.isNull())
        {

            //qDebug("  "+ childNode.toElement().tagName().toLatin1()+ ":" + childNode.toElement().firstChild().nodeValue().toLatin1());

            if(childNode.toElement().tagName() == "subrow")
            {
                newdoc->addSubRow(rown);
                subrow++;
                QDomNode childNodeSub = childNode.toElement().firstChild();
                int subcolumn = -1;
                while(!childNodeSub.isNull())
                {
                    subcolumn++;
                    if(subcolumn != 0)
                        model->item(rown)->setChild(subrow, subcolumn   ,new  QStandardItem(childNodeSub.toElement().firstChild().nodeValue()));
                    //qDebug("   "+childNodeSub.toElement().tagName().toLatin1()+":"+childNodeSub.toElement().firstChild().nodeValue().toLatin1());
                    childNodeSub = childNodeSub.nextSibling();

                }
            }
            else {

                column++;

                if(!headlistdone){
                    headlist <<  childNode.toElement().tagName();
                    if(column > 1)
                     newdoc->addEmptyColumn();
                 }

                 if(column != 0){
                     model->setItem(rown,column, new   QStandardItem(childNode.toElement().firstChild().nodeValue()));
                     qDebug() << "DATA CHANGED: " << childNode.toElement().nodeValue();
                 }

            }

            childNode = childNode.nextSibling();
        }

        if(!headlistdone){
            model->setHorizontalHeaderLabels(headlist);
            headlist.clear();
            headlistdone = true;
        }
        child = child.nextSibling();
    }

    newdoc->expand();
}

void DocumentEditorView::addNewTab(){

    bool ok;
    QString text = QInputDialog::getText(this, tr("Document Name"), tr("Document Name:"), QLineEdit::Normal, "Doc", &ok);

    if (ok && !text.isEmpty()){
        DocumentView *newdoc = new DocumentView(text, this);
        ui->tabWidget->addTab(newdoc, text);
        documentlist.push_back(newdoc);
        docAdded(newdoc);
   }

}

void DocumentEditorView::deleteTab(){
    if(ui->tabWidget->currentIndex() < 0)
        return;

    if(QMessageBox::question(this, "Delete Document", "Are you sure you want to delete this document?",  QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes){
        std::list<DocumentView*>::iterator it = documentlist.begin();
        std::advance(it, ui->tabWidget->currentIndex());
        documentlist.erase(it);
        removeDocument(ui->tabWidget->currentIndex());        
        //ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    }
}
