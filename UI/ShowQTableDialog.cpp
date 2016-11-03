#include "ShowQTableDialog.h"

ShowQTableDialog::ShowQTableDialog(const mat &q, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    QStringList l;
    l<<"X"<<"Y"<<"Z";
    tableWidget->setHorizontalHeaderLabels(l);
//    layout()->setSizeConstraint(QLayout::SetFixedSize);

    tableWidget->setRowCount(q.n_cols-1); // first row is zeros
    for (uint r = 0; r<q.n_cols-1; r++)
        for (int c=0;c<3;c++) {
            tableWidget->setItem(r,c,
                                 new QTableWidgetItem(QString::number(q(c,r+1)
                                                                      ,'g',6)));
        }
}

