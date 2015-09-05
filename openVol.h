#ifndef OPENVOL_H
#define OPENVOL_H

#include <QtGui>
#include <QDialog>
#include "misc_functions.h"
#include "myprogressdialog.h"
#include "ui_openVol.h"
#include "mriimage.h"
#include "armadillo"

using namespace arma;
using namespace std;

class OpenImageDialog: public QDialog, public Ui::OpenImageDialog
{
    Q_OBJECT

public:
    OpenImageDialog(QString fileName,QWidget *parent = 0);
    ~OpenImageDialog() {qDeleteAll(this->children());}

    QVector<uchar>    orientation;
    inline ImageFileType fileType() {
        if(AnalyzeCheck->isChecked())           return ANALYZE;
        else if (NIFTI_Check->isChecked())      return NIFTI;
        else                                    return RAW;
    }
    bool getHeader(AnalyzeHeader &header);

private slots:
    void updateSwapXY(bool checked);
    void updateSwapXZ(bool);
    void updateSwapYZ(bool);
    void updateFlipX(bool);
    void updateFlipY(bool);
    void updateFlipZ(bool);

    void lineEditTextChanged();
    void checkBeforeExit();
    void preview();

private:
    QString         inputFileName;
    uchar_cube      newImg,untouchedImg;
    bool            previewed; //RefVolExist,

    void drawImg();
    void applyRotation();
};

#endif // OPENVOL_H
