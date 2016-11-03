#include "connectomegeneratedialog.h"

ConnectomeGenerateDialog::ConnectomeGenerateDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    connect(FileNameEdit,SIGNAL(textEdited(QString)),this,SLOT(onLineEditTextChange()));
    connect(UseFreesurferBox,SIGNAL(toggled(bool)),this,SLOT(onUseFSCheck(bool)));
    connect(BrowseButton,SIGNAL(clicked()),this,SLOT(onBrowseButtonPress()));
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void ConnectomeGenerateDialog::onLineEditTextChange()
{
    buttonBox->button(QDialogButtonBox::Ok)->setDisabled(FileNameEdit->text().isEmpty());
}

void ConnectomeGenerateDialog::onUseFSCheck(bool status)
{
    if (status) {
        QChar sep = QDir::separator();
        FileNameEdit->setText(QDir::currentPath().replace('/',sep)+sep+"FreesurferLabels.txt");
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void ConnectomeGenerateDialog::onBrowseButtonPress()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open label names file", ".",
                                                    QString("(*.txt)"));
    if (fileName.isEmpty())
        return;
    FileNameEdit->setText(fileName.replace('/',QDir::separator()));
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
