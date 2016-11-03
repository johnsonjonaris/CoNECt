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

/**
  * \class gTableDialog
  *
  * This class creates the gradient table dialog.
  * The dialog interrogates the user about the gradient table,
  * it allows the user to add\save\delete new gradient tables.
  *
  */

class gTableDialog: public QDialog, public Ui::gTableDialog
{
    Q_OBJECT
public:
    gTableDialog(QList<mat> *list, QWidget *parent = 0);
    QList<mat> *gTableList;     ///< list of gradient tables

private:
    ///
    bool scangTableEdit(mat &gTable);
    /// read gradient table file
    bool readGradientTable(QString fileName, mat &gTable);
    /// store gradient tables
    bool storeCurrentTables();
    /// invoked when
    QString getTableName();


private slots:
    /// invoked when chosen gradient table is changed
    void ongTableListBoxChange(int index);
    /// invoked when open button is pressed
    void onImportTabelButtonPress();
    /// invoked when add button is pressed
    void onAddTableButtonPress();
    /// invoked when save button is pressed
    void onSaveButtonPress();
    /// invoked when delete button is pressed
    void onDeleteTableButtonPress();
    /// invoked when text is changed
    void onTableEditChange() {SaveButton->setEnabled(true);}
};

#endif // GTABLEDIALOG_H
