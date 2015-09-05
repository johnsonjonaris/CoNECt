#ifndef MODULARDIALOG_H
#define MODULARDIALOG_H

#include "QLabel"
#include "QPushButton"
#include <QMouseEvent>
#include <QColorDialog>
#include "ui_modulardialog.h"
#include <QDialog>
#include <QDebug>

// this button emits clicked with a pointer to itself
// it allows the receiver to control its properties
class QPushButton2 :public QPushButton
{
    Q_OBJECT
    void mousePressEvent(QMouseEvent *e)
    {
        if (e->type() == QEvent::MouseButtonPress)
            emit clicked(this);
        QPushButton::mousePressEvent(e);
    }
public:
    explicit QPushButton2(QWidget *parent=0) : QPushButton(parent) {}
    explicit QPushButton2(const QString &text, QWidget *parent=0) : QPushButton(text,parent) {}
signals:
    void clicked(QPushButton2*);
};

class ModularDialog : public QDialog, private Ui::ModularDialog
{
    Q_OBJECT
    
public:
    QVector<QColor> cListOut;

    ModularDialog(const QVector<QColor> &cList, QWidget *parent = 0);
    ~ModularDialog() { qDeleteAll(this->children()); }
    
private slots:
    void onModuleButtonPress(QPushButton2*);

};

#endif // MODULARDIALOG_H
