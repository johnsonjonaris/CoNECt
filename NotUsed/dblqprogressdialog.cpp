#include "dblqprogressdialog.h"

DblQProgressDialog::DblQProgressDialog(QWidget *parent) :
    QProgressDialog(parent)
{
    mainBar = new QProgressBar(this);
    QVBoxLayout *hl = new QVBoxLayout(this);
    hl->setContentsMargins(4,4,4,4);
    this->setLayout(hl);
    QObjectList l = this->children();
    QString objName;
    QLabel *lbl = 0;
    QProgressBar *bar = 0;
    QPushButton *pb = 0;

    for (int i=0;i<l.size();i++) {
        objName = l[i]->metaObject()->className();
        if (objName == "QLabel")
            lbl = qobject_cast<QLabel *>(l[i]);
        else if (objName == "QProgressBar" &&
                 qobject_cast<QProgressBar *>(l[i]) != mainBar)
            bar = qobject_cast<QProgressBar *>(l[i]);
        else if (objName == "QPushButton")
            pb = qobject_cast<QPushButton *>(l[i]);
    }
    hl->addWidget(lbl);
    hl->addWidget(bar);
    hl->addWidget(mainBar);
    hl->addWidget(pb);
}

void DblQProgressDialog::setValueMain(int progress)
{
    if (progress == mainBar->value()
            || (mainBar->value() == -1 && progress == mainBar->maximum()))
        return;

    mainBar->setValue(progress);

    if (isModal())
        qApp->processEvents();

#ifdef Q_WS_MAC
    QApplication::flush();
#endif

    if (progress == mainBar->maximum() && autoReset())
        reset();
}

void DblQProgressDialog::reset()
{
    QProgressDialog::reset();
    mainBar->reset();
}
