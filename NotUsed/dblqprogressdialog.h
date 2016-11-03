#ifndef DBLQPROGRESSDIALOG_H
#define DBLQPROGRESSDIALOG_H

#include <QProgressBar>
#include <QProgressDialog>
#include <QLayout>
#include <QString>
#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>


class MyQProgressDialog : public QProgressDialog
{
    Q_OBJECT
    bool disableEsc;

protected:
    void keyPressEvent(QKeyEvent *e) {
        if(e->key()== Qt::Key_Escape && disableEsc)
            e->accept();
        else
            QDialog::keyPressEvent(e);
    }

public:
    MyQProgressDialog(QWidget *parent = 0) : QProgressDialog(parent) {}
    void setDisabelEsc(bool status) {disableEsc = status;}

signals:

public slots:

};

/**
  * \class DblQProgressDialog
  *
  * Special progress dialog with two progress bars.
  */
class DblQProgressDialog : public QProgressDialog
{
    Q_OBJECT
    QProgressBar *mainBar;
    bool disableEsc;

protected:
    void keyPressEvent(QKeyEvent *e) {
        if(e->key()== Qt::Key_Escape && disableEsc)
            e->accept();
        else
            QDialog::keyPressEvent(e);
    }

public:
    DblQProgressDialog(QWidget *parent = 0);
    ~DblQProgressDialog() {delete mainBar;}
    void setMinimumMain(int minimum) {mainBar->setMinimum(minimum);}
    void setMaximumMain(int maximum) {mainBar->setMaximum(maximum);}
    int minimumMain() {return mainBar->minimum();}
    int maximumMain() {return mainBar->maximum();}
    void setDisabelEsc(bool status) {disableEsc = status;}
    void setValueMain(int progress);
    void setRangeMain(int minimum, int maximum){mainBar->setRange(minimum,maximum);}
    int valueMain() { return mainBar->value();}
    void reset();

signals:
    
public slots:
    
};

#endif // DBLQPROGRESSDIALOG_H
