#ifndef SIZEDIALOG_H
#define SIZEDIALOG_H

#include <QDialog>
#include "ui_sizedialog.h"

namespace Ui {
    class SizeDialog;
}

class SizeDialog : public QDialog, public Ui::SizeDialog
{
    Q_OBJECT

public:
    SizeDialog(QWidget *parent = 0);
};

#endif // SIZEDIALOG_H
