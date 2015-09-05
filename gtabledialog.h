#ifndef GTABLEDIALOG_H
#define GTABLEDIALOG_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include "ui_gtabledialog.h"
#include "armadillo"

using namespace arma;

class gTableDialog: public QDialog, public Ui::gTableDialog
{
    Q_OBJECT
public:
    gTableDialog(QList<mat> *list, QWidget *parent = 0);
    QList<mat> *gTableList;

private:
    bool scangTableEdit(mat &gTable);
    bool readGradientTable(QString fileName,mat &gTable);
    bool storeCurrentTables();
    QString getTableName();


private slots:
    void ongTableListBoxChange(int index);
    void onImportTabelButtonPress();
    void onAddTableButtonPress();
    void onSaveButtonPress();
    void onDeleteTableButtonPress();
    void onTableEditChange() {SaveButton->setEnabled(true);}
};

#endif // GTABLEDIALOG_H
