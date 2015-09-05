#ifndef CONNECTOMEDATAINSPECTIONDIALOG_H
#define CONNECTOMEDATAINSPECTIONDIALOG_H

#include "ui_connectomedatainspectiondialog.h"
#include <QDialog>

class ConnectomeInspectionDialog : public QDialog, private Ui::ConnectomeInspectionDialog
{
    Q_OBJECT
    
public:
    ConnectomeInspectionDialog(QWidget *parent = 0);
    ~ConnectomeInspectionDialog() { qDeleteAll(this->children()); }
    
private:

private slots:
    void onNoChangeCheck();
    void onInvertXCheck(bool status);
    void onInvertYCheck(bool status);
    void onInvertZCheck(bool status);
    void onSwapXYCheck(bool status);
    void onSwapXZCheck(bool status);
    void onSwapYZCheck(bool status);

};

#endif // CONNECTOMEDATAINSPECTIONDIALOG_H
