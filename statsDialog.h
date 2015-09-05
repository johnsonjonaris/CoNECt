#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include "ui_statsdialog.h"
#include "fibertracts.h"
#include "misc_functions.h"
#include "armadillo"

using namespace arma;


class StatsDialog : public QDialog, public Ui::StatsDialog
{
    Q_OBJECT
public:
    StatsDialog(const FiberTracts &fibers,
                const QList<quint32> *currentSelectedFibers,
                const cube &volume, bool isLabel, QWidget *parent = 0);
    ~StatsDialog() {qDeleteAll(this->children());}

protected:
    void keyPressEvent (QKeyEvent *e);
};

#endif // STATSDIALOG_H
