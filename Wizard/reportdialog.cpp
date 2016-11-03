#include "reportdialog.h"

ReportDialog::ReportDialog(const QString &text, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    ReportTextEditor->setPlainText(text);
}

