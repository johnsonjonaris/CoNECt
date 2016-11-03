#include "showmetricsdialog.h"
#include "ui_showmetricsdialog.h"

ShowMetricsDialog::ShowMetricsDialog(const Connectome &cm, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    if (cm.isEmpty())
        return;
    uint n = cm.getHeader().nRegions;
    // prepare the number of columns
    LocalMetricsTable->setColumnCount(n+1);
    // fill the names of the labels, if existing
    if (!cm.getNames().isEmpty()) {
        QFont f; f.setBold(true);
        for (uint i=0;i<n;++i) {
            QTableWidgetItem *item = new QTableWidgetItem(cm.getNames().at(i));
            item->setFont(f);
            LocalMetricsTable->setItem(0,i+1,item);
        }
    }
    // fill the local metrics
    for (uint r=0;r<9;++r)
        for (uint c=0;c<n;++c) {
            LocalMetricsTable->setItem(r+1,c+1,new QTableWidgetItem(
                                           QString::number(cm.getLocalMetrics().at(r,c)
                                                           ,'g',6)));
        }
    // fill global metrics
    for (uint r=0;r<5;++r) {
        GlobalMetricsTable->setItem(r,1,new QTableWidgetItem(
                                        QString::number(cm.getGlobalMetrics().at(r)
                                                        ,'g',6)));
        GlobalMetricsTable->setItem(r,3,new QTableWidgetItem(
                                        QString::number(cm.getGlobalMetrics().at(r+5)
                                                        ,'g',6)));
    }
}

ShowMetricsDialog::~ShowMetricsDialog() { qDeleteAll(this->children()); }

void ShowMetricsDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->type() == QKeyEvent::KeyPress)
        if(e->matches(QKeySequence::Copy)) {
            if (LocalMetricsTable->hasFocus())
                onCopyKeysPressed(LocalMetricsTable);
            else
                onCopyKeysPressed(GlobalMetricsTable);
            e->accept();
        }
}
