#ifndef SAVEVOL_H
#define SAVEVOL_H

#include <QDialog>
#include <QtGui>
#include "ui_saveVol.h"
#include "armadillo"
#include "DataTypes.h"
using namespace arma;

class SaveImageDialog : public QDialog, public Ui::SaveImageDialog
{
    Q_OBJECT

public:
    SaveImageDialog(const QStringList &list, const mat &info, QWidget *parent=0);
    ~SaveImageDialog() {qDeleteAll(this->children());}

    QVector<uint> volsToBeSaved;

    inline ImageFileType fileType() { return (DoAnalyze->isChecked())?ANALYZE:NIFTI; }
private:
    mat volsInfo;
private slots:
    void addItem();
    void removeItem();
    void checkBeforeExit();
};

#endif // SAVEVOL_H
