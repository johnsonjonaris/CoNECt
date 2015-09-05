#include "connectomedatainspectiondialog.h"

ConnectomeInspectionDialog::ConnectomeInspectionDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    connect(NoChangeRadio,SIGNAL(clicked()),this,SLOT(onNoChangeCheck()));
    connect(InvertXRadio,SIGNAL(clicked(bool)),this,SLOT(onInvertXCheck(bool)));
    connect(InvertYRadio,SIGNAL(clicked(bool)),this,SLOT(onInvertYCheck(bool)));
    connect(InvertZRadio,SIGNAL(clicked(bool)),this,SLOT(onInvertZCheck(bool)));
    connect(SwapXYRadio,SIGNAL(clicked(bool)),this,SLOT(onSwapXYCheck(bool)));
    connect(SwapXZRadio,SIGNAL(clicked(bool)),this,SLOT(onSwapXZCheck(bool)));
    connect(SwapYZRadio,SIGNAL(clicked(bool)),this,SLOT(onSwapYZCheck(bool)));
}

void ConnectomeInspectionDialog::onNoChangeCheck()
{
    InvertXRadio->setChecked(false);
    InvertYRadio->setChecked(false);
    InvertZRadio->setChecked(false);
    SwapXYRadio->setChecked(false);
    SwapXZRadio->setChecked(false);
    SwapYZRadio->setChecked(false);
}

void ConnectomeInspectionDialog::onInvertXCheck(bool status)
{
    if (status) {
        NoChangeRadio->setChecked(false);
    }
}
void ConnectomeInspectionDialog::onInvertYCheck(bool status)
{
    if (status) {
        NoChangeRadio->setChecked(false);
    }
}
void ConnectomeInspectionDialog::onInvertZCheck(bool status)
{
    if (status) {
        NoChangeRadio->setChecked(false);
    }
}
void ConnectomeInspectionDialog::onSwapXYCheck(bool status)
{
    if (status) {
        NoChangeRadio->setChecked(false);
    }
}
void ConnectomeInspectionDialog::onSwapXZCheck(bool status)
{
    if (status) {
        NoChangeRadio->setChecked(false);
    }
}
void ConnectomeInspectionDialog::onSwapYZCheck(bool status)
{
    if (status) {
        NoChangeRadio->setChecked(false);
    }
}
