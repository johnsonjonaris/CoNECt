#ifndef SAVEVOL_H
#define SAVEVOL_H

#include <QDialog>
#include <QtGui>
#include "ui_saveVol.h"
#include "armadillo"
#include "datatypes.h"
using namespace arma;

/**
  * \class SaveImageDialog
  *
  * save one or multiple images dialog
  */
class SaveImageDialog : public QDialog, public Ui::SaveImageDialog
{
    Q_OBJECT

public:
    /// constructor populates the dialog with available image(s) to be saved
    SaveImageDialog(const QStringList &list, const mat &info, QWidget *parent=0);
    ~SaveImageDialog() {qDeleteAll(this->children());}

    QVector<uint> volsToBeSaved;

    inline ImageFileType fileType() { return (DoAnalyze->isChecked())?ANALYZE:NIFTI; }
private:
    mat volsInfo;
private slots:
    /// add image item to the list of image(s) to be saved
    void addItem();
    /// remove image item from the list of image(s) to be saved
    void removeItem();
    /// check if: 1 or more image(s) are loaded to be saved and they are compatible
    void checkBeforeExit();
};

#endif // SAVEVOL_H
