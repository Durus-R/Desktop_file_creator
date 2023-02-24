#include "mainwindow.h"
#include "./ui_mainwindow.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{



    ui->setupUi(this);
    connect(ui->btnAdd, SIGNAL(clicked()), SLOT(btnAddClicked()));
    connect(ui->btnRemove, SIGNAL(clicked()), SLOT(btnRemoveClicked()));
    connect(ui->btnNewChild, SIGNAL(clicked()), SLOT(btnNewChildClicked()));
    connect(ui->tree, SIGNAL(itemSelectionChanged()), SLOT(treeSelectionChanged()));
    connect(ui->actionSave, SIGNAL(triggered()), SLOT(actionSaveTriggered()));
    connect(ui->tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(treeItemDoubleClicked(QTreeWidgetItem*,int)));
    connect(ui->actionOpen_as_Template, SIGNAL(triggered()), SLOT(actionOpenAsTemplateTriggered()));
    connect(ui->actionNew, SIGNAL(triggered()), SLOT(actionNewTriggered()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnAddClicked(){
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->tree);
    item->setText(0, tr("Category"));
    ui->tree->addTopLevelItem(item);
}

void MainWindow::btnRemoveClicked(){
    for ( QTreeWidgetItem *item: ui->tree->selectedItems()) {
        delete item;
    }
}

void MainWindow::btnNewChildClicked() {

        QTreeWidgetItem *child = new QTreeWidgetItem();
        child->setText(0, tr("Left"));
        child->setText(1, tr(""));
        if (ui->tree->currentItem()->parent() == nullptr) {
            ui->tree->currentItem()->addChild(child);
            ui->tree->currentItem()->setExpanded(true);
        } else {
            ui->tree->currentItem()->parent()->addChild(child);
        }

}

void MainWindow::treeSelectionChanged() {
    if (ui->tree->selectedItems().length() == 0) {
        ui->btnNewChild->setEnabled(false);
        ui->lblName->setText(QString(""));
    } else {
    ui->btnNewChild->setEnabled(true);
    if (ui->tree->selectedItems().length() == 1 && ui->tree->selectedItems()[0]->text(0) != "(null)") {
        ui->lblName->setText(ui->tree->selectedItems()[0]->text(0));}
    else {
        ui->lblName->setText(QString(""));
    }}
}

void MainWindow::treeItemDoubleClicked(QTreeWidgetItem* item, int count) {

    if (count == 1 && item->parent() == nullptr) {
        return;}
    else {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->tree->editItem(item, count);}
}

void MainWindow::actionSaveTriggered() {
    QList<QString> output = print_elements();
    for (int i; i<output.length(); i++) {
        qDebug() << output[i];
    }
    qDebug() << output;
    QString filename = QFileDialog::getSaveFileName(this, tr("Save As..."), QString(getenv("HOME")),
                                                                  tr("Configuration Files (*.conf *.desktop);;All Files (*.*)"));
    if (filename == QString(""))
            return;
    qDebug() << filename;
    QFile output_file(filename);
    if (output_file.open(QIODevice::WriteOnly)) {
         QTextStream stream(&output_file);
    for (QString s : output) {
        if (s == QString("[(null)]") || s == QString("") || s == QString("[]")) {
            continue;}
        qDebug() << s;
        stream << s << Qt::endl;
    }
    output_file.close();
    } else {
        QMessageBox::critical(this, "Desktop File Creator", tr("No permission for %1").arg(filename), QMessageBox::Ok);
    }


}

QList<QString> MainWindow::print_elements() {
    QList<QString> output = QList<QString>();
    for (int i = 0; i<ui->tree->topLevelItemCount(); i++) {
        output.append(QString("[%1]").arg(ui->tree->topLevelItem(i)->text(0)));

        for (int j = 0; j<ui->tree->topLevelItem(i)->childCount(); j++) {
            QTreeWidgetItem *child = ui->tree->topLevelItem(i)->child(j);
            if (child->text(1) == QString("")) {
                output.append(child->text(0));
            } else {
                output.append(child->text(0)+"="+child->text(1));
            }
           }
    }
    return output;
}

void MainWindow::actionOpenAsTemplateTriggered() {
    QList<QString> output;
    QString filename, template_filename;
    if (ui->tree->topLevelItemCount()>0) {
        int ret = QMessageBox::warning(this, tr("Desktop File Creator"),
                                       tr("The document has been modified.\n"
                                          "Do you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard
                                       | QMessageBox::Cancel,
                                       QMessageBox::Save);
        if (ret == QMessageBox::Save) {
            QList<QString> output = print_elements();
            for (int i; i<output.length(); i++) {
                qDebug() << output[i];
            }
            qDebug() << output;
            QString filename = QFileDialog::getSaveFileName(this, tr("Save As..."), QString(getenv("HOME")),
                                                                          tr("Configuration Files (*.conf *.desktop);;All Files (*.*)"));
            if (filename == QString(""))
                    return;
            qDebug() << filename;
            QFile output_file(filename);
            if (output_file.open(QIODevice::WriteOnly)) {
                 QTextStream stream(&output_file);
            for (QString s : output) {
                if (s == QString("[(null)]") || s == QString("") || s == QString("[]")) {
                    continue;}
                qDebug() << s;
                stream << s << Qt::endl;
            }
            output_file.close();
            } else {
                QMessageBox::critical(this, "Desktop File Creator", tr("No permission for %1").arg(filename), QMessageBox::Ok);
            }

           }
        if (ret == QMessageBox::Cancel)
            return;

    }
    ui->tree->clear();
    template_filename = QFileDialog::getOpenFileName(this, tr("Open As Template..."), "/etc/", tr("Configuration Files (*.conf *.desktop);;All Files (*.*)"));
    if (template_filename == QString("")) return;
    qDebug() << template_filename;
    QFile input_file(template_filename);
    QList<QString> input;
    input.append("[(null)]");
    if (input_file.open(QIODevice::ReadOnly))
    {
       QTextStream in(&input_file);
       while (!in.atEnd())
       {
          QString i = in.readLine();
          if (i.startsWith("#") || i.startsWith(";") || i.trimmed() == QString(""))
              continue;
          input.append(i);

       }
       input_file.close();

    if (input[1].startsWith("["))
            input.pop_front();}
    else {
        QMessageBox::critical(this, "Desktop File Creator", tr("No permission for %1").arg(filename), QMessageBox::Ok);
        return;
    }

    int header = -1;

    for (int i = 0; i<input.length(); i++) {
        if (!(input[i].startsWith("[")) && header == -1)
                continue; //skip comments and invalid lines
        if (input[i].startsWith("["))
        {
            header++;
            QTreeWidgetItem *item = new QTreeWidgetItem();
            input[i].chop(1);
            input[i].remove(0, 1);
            item->setText(0, input[i]);
            ui->tree->addTopLevelItem(item);
            item->setExpanded(true);
        } else {
            QTreeWidgetItem *child = new QTreeWidgetItem(ui->tree->topLevelItem(header));
            if (input[i].contains("=")) {
                QStringList parts = input[i].split("=");
                child->setText(0, parts[0]);
                child->setText(1, parts[1]);
            } else {
                child->setText(0, input[i]);
            }
            ui->tree->topLevelItem(header)->addChild(child);

        }
    }

}

void MainWindow::actionNewTriggered() {
    QList<QString> output;
    QString filename;
    if (ui->tree->topLevelItemCount()>0) {
        int ret = QMessageBox::warning(this, tr("Desktop File Creator"),
                                       tr("The document has been modified.\n"
                                          "Do you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard
                                       | QMessageBox::Cancel,
                                       QMessageBox::Save);
        if (ret == QMessageBox::Save) {
            QList<QString> output = print_elements();
            for (int i; i<output.length(); i++) {
                qDebug() << output[i];
            }
            qDebug() << output;
            QString filename = QFileDialog::getSaveFileName(this, tr("Save As..."), QString(getenv("HOME")),
                                                                          tr("Configuration Files (*.conf *.desktop);;All Files (*.*)"));
            if (filename == QString(""))
                    return;
            qDebug() << filename;
            QFile output_file(filename);
            if (output_file.open(QIODevice::WriteOnly)) {
                 QTextStream stream(&output_file);
            for (QString s : output) {
                if (s == QString("[(null)]") || s == QString("") || s == QString("[]")) {
                    continue;}
                qDebug() << s;
                stream << s << Qt::endl;
            }
            output_file.close();
            } else {
                QMessageBox::critical(this, "Desktop File Creator", tr("No permission for %1").arg(filename), QMessageBox::Ok);
            }
            ui->tree->clear();
        }
        if (ret == QMessageBox::Cancel)
            return;

    } else {
    ui->tree->clear();}
}
