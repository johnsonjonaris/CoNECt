#ifndef DTIDIALOG_H
#define DTIDIALOG_H

#include <QDialog>
#include "ui_dtidialog.h"
#include "armadillo"

using namespace arma;

class DTIDialog : public QDialog, Ui::DTIDialog
{
    Q_OBJECT
    
public:
    DTIDialog(QWidget *parent = 0);
    const uvec& getTensorOrder() const { return tensorOrder; }
    
private slots:
    void onTensorItemSlection();
    void onPushUpButtonPress();
    void onPushDownButtonPress();

private:
    uvec tensorOrder;

};

#endif // DTIDIALOG_H
