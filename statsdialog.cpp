#include "statsDialog.h"

StatsDialog::StatsDialog(const FiberTracts &fibers, const QList<quint32> *selectedFibers,
                         const cube &volume, bool isLabel, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "" << "All Fibers" << "Selected");

    if (fibers.isEmpty())
        return;
    // Fill the table
    // get min, max, mean points
    int minTpoints=10000, maxTpoints=0;
    double sumTpoints=0, meanTpoints;
    for (int i=0; i<fibers.size();i++) {
        if (minTpoints > fibers.getFiber(i).size())
            minTpoints = fibers.getFiber(i).size();
        if(maxTpoints < fibers.getFiber(i).size())
            maxTpoints = fibers.getFiber(i).size();
        sumTpoints += fibers.getFiber(i).size();
    }
    meanTpoints = sumTpoints/fibers.size();
    // get min, max, mean, std for the scalar
    vec scalar = Col<double>(sumTpoints,1);
    quint32 c = 0, x, y, z;
    int     rows = volume.n_rows,
            cols = volume.n_cols,
            slices = volume.n_slices;
    for (int i=0; i<fibers.size();i++) {
        for (int j=0; j<fibers.getFiber(i).size();j++) {
            x = qMax(qMin(qRound(fibers.getFiber(i).at(j).x()),rows-1),0);
            y = qMax(qMin(qRound(fibers.getFiber(i).at(j).y()),cols-1),0);
            z = qMax(qMin(qRound(fibers.getFiber(i).at(j).z()),slices-1),0);
            scalar(c) = volume(x,y,z);
            c++;
        }
    }
    // fill All fibers stats
    tableWidget->item(0,1)->setText(QString::number(fibers.size(),'g',6));
    tableWidget->item(1,1)->setText(QString::number(minTpoints,'g',6));
    tableWidget->item(2,1)->setText(QString::number(maxTpoints,'g',6));
    tableWidget->item(3,1)->setText(QString::number(meanTpoints,'g',6));
    tableWidget->item(4,1)->setText(QString::number(fibers.minLength(),'g',6));
    tableWidget->item(5,1)->setText(QString::number(fibers.maxLength(),'g',6));
    tableWidget->item(6,1)->setText(QString::number(fibers.meanLength(),'g',6));
    if (!isLabel) {
        tableWidget->item(7,1)->setText(QString::number(scalar.min(),'g',6));
        tableWidget->item(8,1)->setText(QString::number(scalar.max(),'g',6));
        tableWidget->item(9,1)->setText(QString::number(arma::mean(scalar),'g',6));
        tableWidget->item(10,1)->setText(QString::number(arma::stddev(scalar),'g',6));
    }
    // fill selected fibers stats
    if (selectedFibers) {
        // get min, max, mean points, min, max, mean length
        minTpoints=10000;       maxTpoints=0;   sumTpoints=0;
        double minL = 100000,   maxL=0,         sumL = 0,       meanL;
        int idx;
        for (int i=0; i<selectedFibers->size();i++) {
            idx = selectedFibers->at(i);
            // points
            if (minTpoints > fibers.getFiber(idx).size())
                minTpoints = fibers.getFiber(idx).size();
            if(maxTpoints < fibers.getFiber(idx).size())
                maxTpoints = fibers.getFiber(idx).size();
            sumTpoints += fibers.getFiber(idx).size();
            // length
            if (minL > fibers.getLength(i))
                minL = fibers.getLength(i);
            if (maxL < fibers.getLength(i))
                maxL = fibers.getLength(i);
            sumL += fibers.getLength(i);
        }
        meanTpoints = sumTpoints/selectedFibers->size();
        meanL = sumL/selectedFibers->size();
        // get min, max, mean, std for the scalar
        scalar = Col<double>(sumTpoints,1);
        c = 0;
        for (int i=0; i<selectedFibers->size();i++) {
            idx = selectedFibers->at(i);
            for (int j=0; j<fibers.getFiber(idx).size();j++) {
                x = qMax(qMin(qRound(fibers.getFiber(idx).at(j).x()),rows-1),0);
                y = qMax(qMin(qRound(fibers.getFiber(idx).at(j).y()),cols-1),0);
                z = qMax(qMin(qRound(fibers.getFiber(idx).at(j).z()),slices-1),0);
                scalar(c) = volume(x,y,z);
                c++;
            }
        }
        tableWidget->item(0,2)->setText(QString::number(selectedFibers->size(),'g',6));
        tableWidget->item(1,2)->setText(QString::number(minTpoints,'g',6));
        tableWidget->item(2,2)->setText(QString::number(maxTpoints,'g',6));
        tableWidget->item(3,2)->setText(QString::number(meanTpoints,'g',6));
        tableWidget->item(4,2)->setText(QString::number(minL,'g',6));
        tableWidget->item(5,2)->setText(QString::number(maxL,'g',6));
        tableWidget->item(6,2)->setText(QString::number(meanL,'g',6));
        if (!isLabel) {
            tableWidget->item(7,2)->setText(QString::number(scalar.min(),'g',6));
            tableWidget->item(8,2)->setText(QString::number(scalar.max(),'g',6));
            tableWidget->item(9,2)->setText(QString::number(arma::mean(scalar),'g',6));
            tableWidget->item(10,2)->setText(QString::number(arma::stddev(scalar),'g',6));
        }
    }
    tableWidget->resizeColumnsToContents();
    updateGeometry();
    setFixedSize(this->size());
}

void StatsDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->type() == QKeyEvent::KeyPress)
        if(e->matches(QKeySequence::Copy)) {
            onCopyKeysPressed(tableWidget);
            e->accept();
        }
}

