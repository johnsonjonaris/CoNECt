#ifndef SHOWQTABLEDIALOG_H
#define SHOWQTABLEDIALOG_H

#include <QDialog>
#include <QTableWidgetItem>
#include "ui_ShowQTableDialog.h"
#include "armadillo"

using namespace arma;

/**
  * \class ShowQTableDialog
  *
  * Display Q table on user request
  */
class ShowQTableDialog : public QDialog, public Ui::ShowQTableDialog
{
    Q_OBJECT
public:
    /// constructor populates the dialog with the Q table
    ShowQTableDialog(const mat &q, QWidget *parent = 0);
    ~ShowQTableDialog() { qDeleteAll(this->children()); }
};

#endif // SHOWQTABLEDIALOG_H
