#include "aboutFrame.h"
AboutFrame::AboutFrame(QWidget *parent) :  QDialog(parent)
{
    setupUi(this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}
