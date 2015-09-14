#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include "ui_statsdialog.h"
#include "fibertracts.h"
#include "misc_functions.h"
#include "armadillo"

using namespace arma;

/**
  * \class StatsDialog
  *
  * Display statistics dialog
  */
class StatsDialog : public QDialog, public Ui::StatsDialog
{
    Q_OBJECT
public:
    /// constructor populates the dialog
    StatsDialog(const FiberTracts &fibers,
                const QList<quint32> *currentSelectedFibers,
                const cube &volume,
                bool isLabel,
                QWidget *parent = 0);
    ~StatsDialog() { qDeleteAll(this->children()); }

protected:
    /// allows copy of data into clipboard
    void keyPressEvent (QKeyEvent *e);
};

#endif // STATSDIALOG_H
