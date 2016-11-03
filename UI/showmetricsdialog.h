#ifndef SHOWMETRICSDIALOG_H
#define SHOWMETRICSDIALOG_H

#include <QDialog>
#include "ui_showmetricsdialog.h"
#include "Connectome\connectome.h"
#include "Algorithms\misc_functions.h"
#include "armadillo"
using namespace arma;

/**
  * \class ShowMetricsDialog
  *
  * display graph theoritic metrics computed for a connectome
  */
class ShowMetricsDialog : public QDialog, private Ui::ShowMetricsDialog
{
    Q_OBJECT

public:
    /// populates the dialog with graph theoretic metrics
    ShowMetricsDialog(const Connectome &cm, QWidget *parent = 0);
    ~ShowMetricsDialog();

protected:
    /// allows copying the displayed results into clipboard
    void keyPressEvent (QKeyEvent *e);
    
};

#endif // SHOWMETRICSDIALOG_H
