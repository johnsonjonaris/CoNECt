#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include "ui_reportdialog.h"
#include <QDialog>

class ReportDialog : public QDialog, private Ui::ReportDialog
{
    Q_OBJECT
    
public:
    ReportDialog(const QString &text, QWidget *parent = 0);
    ~ReportDialog() {qDeleteAll(this->children());}
    
private:

};

#endif // REPORTDIALOG_H
