#include "dtidialog.h"

DTIDialog::DTIDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    tensorOrder<<0<<1<<2<<3<<4<<5;
    // connect signals
    connect(TensorList,SIGNAL(itemSelectionChanged()),this,SLOT(onTensorItemSlection()));
    connect(PushUpButton,SIGNAL(clicked()),this,SLOT(onPushUpButtonPress()));
    connect(PushDownButton,SIGNAL(clicked()),this,SLOT(onPushDownButtonPress()));
}

void DTIDialog::onTensorItemSlection()
{
    int index = TensorList->currentRow();
    PushUpButton->setDisabled(index == 0);
    PushDownButton->setDisabled(index == 5);
}

void DTIDialog::onPushUpButtonPress()
{
    int index = TensorList->currentRow();
    TensorList->insertItem(index-1,TensorList->takeItem(index));
    TensorList->setCurrentRow(index-1);
    tensorOrder.swap_rows(index,index-1);
}

void DTIDialog::onPushDownButtonPress()
{
    int index = TensorList->currentRow();
    TensorList->insertItem(index+1,TensorList->takeItem(index));
    TensorList->setCurrentRow(index+1);
    tensorOrder.swap_rows(index,index+1);
}
