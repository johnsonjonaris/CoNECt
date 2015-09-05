#ifndef ABOUTFRAME_H
#define ABOUTFRAME_H

#include <QtGui>
#include <QDialog>
#include "ui_aboutFrame.h"

class AboutFrame : public QDialog, public Ui::AboutFrame
{
    Q_OBJECT

public:
    AboutFrame(QWidget *parent = 0);
    ~AboutFrame() {qDeleteAll(this->children());}
};

#endif // ABOUTFRAME_H
