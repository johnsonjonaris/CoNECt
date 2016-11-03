#ifndef CONNECTOMEGENERATEDIALOG_H
#define CONNECTOMEGENERATEDIALOG_H

#include "ui_connectomegeneratedialog.h"
#include <QDialog>
#include <QDir>
#include <QFileDialog>

namespace Ui {
class ConnectomeGenerateDialog;
}

class ConnectomeGenerateDialog : public QDialog, private Ui::ConnectomeGenerateDialog
{
    Q_OBJECT
    
public:
    ConnectomeGenerateDialog(QWidget *parent = 0);
    ~ConnectomeGenerateDialog() {qDeleteAll(this->children());}
    bool useFreesurfer() { return UseFreesurferBox->isChecked(); }
    QString getFileName() { return FileNameEdit->text(); }
    
private slots:
    void onLineEditTextChange();
    void onUseFSCheck(bool status);
    void onBrowseButtonPress();
};

#endif // CONNECTOMEGENERATEDIALOG_H
