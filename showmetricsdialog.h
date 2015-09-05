#ifndef SHOWMETRICSDIALOG_H
#define SHOWMETRICSDIALOG_H

#include <QDialog>
#include "ui_showmetricsdialog.h"
#include "connectome.h"
#include "misc_functions.h"
#include "armadillo"
using namespace arma;

class ShowMetricsDialog : public QDialog, private Ui::ShowMetricsDialog
{
    Q_OBJECT

public:
    ShowMetricsDialog(const Connectome &cm, QWidget *parent = 0);
    ~ShowMetricsDialog();

protected:
    void keyPressEvent (QKeyEvent *e);
    
};

#endif // SHOWMETRICSDIALOG_H
