#ifndef MYPROGRESSDIALOG_H
#define MYPROGRESSDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QElapsedTimer>
#include <QShortcut>
#include <QAction>
#include <QPointer>
#include <QDebug>
#include <QKeyEvent>
#include "ui_myprogressdialog.h"

namespace Ui {
class MyProgressDialog;
}
// this class is mimicing the QProgressDialog class
class MyProgressDialog : public QDialog, private Ui::MyProgressDialog
{
    Q_OBJECT
    Q_PROPERTY(bool wasCanceled READ wasCanceled)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(bool autoReset READ getAutoReset WRITE setAutoReset)
    Q_PROPERTY(bool autoClose READ getAutoClose WRITE setAutoClose)
    Q_PROPERTY(int minimumDuration READ minimumDuration WRITE setMinimumDuration)
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)

    bool cancellationFlag;
    bool autoClose;
    bool autoReset;
    bool shownOnce;
    bool forceHide;
    int showTime;
    QTimer *forceTimer;
    QElapsedTimer starttime;

#ifndef QT_NO_CURSOR
    QCursor parentCursor;
#endif

    QPointer<QObject> receiverToDisconnectOnClose;
    QByteArray memberToDisconnectOnClose;

    void open(QObject *receiver, const char *member);

private slots:
    void _q_disconnectOnClose();
    
signals:
    void canceled();

public:
    MyProgressDialog(QWidget *parent = 0);
    ~MyProgressDialog();

    int minimum() const { return ProgBar->maximum(); }
    int maximum() const { return ProgBar->minimum(); }
    int value() const { return ProgBar->value(); }

    int minimumMain() const {
        if (MainProgBar) return ProgBar->maximum();
        return -1;
    }
    int maximumMain() const {
        if (MainProgBar) return MainProgBar->minimum();
        return -1;
    }
    int valueMain() const {
        if (MainProgBar) return MainProgBar->value();
        return -1;
    }

    QString labelText() const { return SecondLabel->text(); }
    QString mainLabelText() const {
        if (MainLabel) return MainLabel->text();
        return QString();
    }

    int minimumDuration() const { return showTime; }

    bool wasCanceled() const { return cancellationFlag; }
    void setAutoReset(bool reset) { autoReset = reset;}
    bool getAutoReset() const { return autoReset; }
    void setAutoClose(bool close) { autoClose = close; }
    bool getAutoClose() const { return autoClose; }

    void disableMain();
    void disableCancel();

protected:
    void keyPressEvent(QKeyEvent *e) {
        if(e->key()== Qt::Key_Escape && CancelButton == NULL)
            e->accept();
        else if (e->key()== Qt::Key_Escape) {
            emit cancel(); e->accept();
        }
        else
            QDialog::keyPressEvent(e);
    }

public slots:
    void cancel();
    void reset();

    void setMaximum(int maximum) { ProgBar->setMaximum(maximum); }
    void setMinimum(int minimum) { ProgBar->setMinimum(minimum); }
    void setRange(int minimum, int maximum) { ProgBar->setRange(minimum, maximum); }
    void setValue(int progress);

    void setMaximumMain(int maximum)
    { if (MainProgBar) MainProgBar->setMaximum(maximum); }
    void setMinimumMain(int minimum)
    { if (MainProgBar) MainProgBar->setMinimum(minimum); }
    void setRangeMain(int minimum, int maximum)
    { if (MainProgBar) MainProgBar->setRange(minimum, maximum); }
    void setValueMain(int progress);

    void setLabelText(const QString &text) { SecondLabel->setText(text); }
    void setMainLabelText(const QString &text) { if (MainLabel) MainLabel->setText(text); }

    void setMinimumDuration(int ms);

protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);

protected slots:
    void forceShow();

private:
    Q_DISABLE_COPY(MyProgressDialog)

};

#endif // MYPROGRESSDIALOG_H
