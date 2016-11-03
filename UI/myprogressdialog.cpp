#include "myprogressdialog.h"

MyProgressDialog::MyProgressDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    cancellationFlag = false;
    autoReset = true;
    autoClose = true;
    shownOnce = false;
    forceHide = false;
    showTime = 4000;

    connect(this, SIGNAL(canceled()), this, SLOT(cancel()));
    connect(CancelButton,SIGNAL(clicked()),this,SLOT(cancel()));
    forceTimer = new QTimer(this);
    connect(forceTimer, SIGNAL(timeout()), this, SLOT(forceShow()));
}

MyProgressDialog::~MyProgressDialog()
{
    qDeleteAll(this->children());
}

void MyProgressDialog::disableMain()
{
    if (MainLabel)
        delete MainLabel;
    MainLabel = NULL;
    if (MainProgBar)
        delete MainProgBar;
    MainProgBar = NULL;
    if (MainGroupBox)
        delete MainGroupBox;
    MainGroupBox = NULL;
}

void MyProgressDialog::disableCancel()
{
    if (CancelButton)
        delete CancelButton;
    CancelButton = NULL;
    setWindowFlags(Qt::Dialog | Qt::WindowContextHelpButtonHint | Qt::CustomizeWindowHint);
}

void MyProgressDialog::reset()
{
#ifndef QT_NO_CURSOR
    if (value() >= 0) {
        if (parentWidget())
            parentWidget()->setCursor(parentCursor);
    }
#endif
    if (autoClose || forceHide)
        hide();
    ProgBar->reset();
    cancellationFlag = false;
    shownOnce = false;
    forceTimer->stop();

    /*
        I wish we could disconnect the user slot provided to open() here but
        unfortunately reset() is usually called before the slot has been invoked.
        (reset() is itself invoked when canceled() is emitted.)
    */
    if (receiverToDisconnectOnClose)
        QMetaObject::invokeMethod(this, "_q_disconnectOnClose", Qt::QueuedConnection);
}

void MyProgressDialog::cancel()
{
    qDebug()<<"cancel";
    forceHide = true;
    reset();
    forceHide = false;
    cancellationFlag = true;
}

void MyProgressDialog::setValue(int progress)
{
    if (progress == ProgBar->value()
        || (ProgBar->value() == -1 && progress == ProgBar->maximum()))
        return;

    ProgBar->setValue(progress);

    if (isModal())
        QApplication::processEvents();

    if ( progress == ProgBar->maximum() && autoReset && (MainProgBar == NULL) )
        reset();
}

void MyProgressDialog::setValueMain(int progress)
{
    if ( MainProgBar == NULL || progress == MainProgBar->value()
        || (MainProgBar->value() == -1 && progress == MainProgBar->maximum()))
        return;

    MainProgBar->setValue(progress);

    if (isModal())
        QApplication::processEvents();

    if ( progress == MainProgBar->maximum() && autoReset )
        reset();
}

void MyProgressDialog::setMinimumDuration(int ms)
{
    showTime = ms;
    if (ProgBar->value() == 0) {
        forceTimer->stop();
        forceTimer->start(ms);
    }
}

void MyProgressDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    forceTimer->stop();
}

void MyProgressDialog::closeEvent(QCloseEvent *e)
{
    emit canceled();
    QDialog::closeEvent(e);
}

void MyProgressDialog::forceShow()
{
    forceTimer->stop();
    if (shownOnce || cancellationFlag)
        return;
    show();
    shownOnce = true;
}

void MyProgressDialog::open(QObject *receiver, const char *member)
{
    connect(this, SIGNAL(canceled()), receiver, member);
    receiverToDisconnectOnClose = receiver;
    memberToDisconnectOnClose = member;
    QDialog::open();
}

void MyProgressDialog::_q_disconnectOnClose()
{
    if (receiverToDisconnectOnClose) {
        disconnect(this, SIGNAL(canceled()), receiverToDisconnectOnClose,
                            memberToDisconnectOnClose);
        receiverToDisconnectOnClose = 0;
    }
    memberToDisconnectOnClose.clear();
}
