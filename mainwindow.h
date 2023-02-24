#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QTime>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <cstdlib>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<QString> print_elements();
private slots:
    void btnAddClicked();
    void btnRemoveClicked();
    void btnNewChildClicked();
    void treeSelectionChanged();
    void treeItemDoubleClicked(QTreeWidgetItem* item, int count);
    void actionSaveTriggered();
    void actionOpenAsTemplateTriggered();
    void actionNewTriggered();

};
#endif // MAINWINDOW_H
