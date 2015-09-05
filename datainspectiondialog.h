#ifndef DATAINSPECTIONDIALOG_H
#define DATAINSPECTIONDIALOG_H

#include <QDialog>
#include <QDir>
#include <QMessageBox>
#include <QEvent>
#include <QWheelEvent>
#include "myprogressdialog.h"
#include "DataTypes.h"
#include "mriimage.h"
#include "ui_datainspectiondialog.h"
#include "armadillo"

using namespace arma;

class DataInspectionDialog : public QDialog, private Ui::DataInspectionDialog
{
    Q_OBJECT
    bool individual;
    QStringList subjects;
    QString path;
    QString baseName;
    ImageFileType fileType;
    QList<uchar_cube> data;
    QVector<QRgb> cTable;

    int     nRows, nCols, nSlices,
            imgRow,imgCol,imgSlice,
            nSubjects,subjectIndex, imageIndex, expectedNImages;

    // display funtions
    void clearDisplay();
    void displayImage();
    bool openImage();
    void updateSubject();
    void plotCoronal(int slice);
    void plotSagittal(int slice);
    void plotAxial(int slice);

public:
    DataInspectionDialog(QWidget *parent = 0);
    ~DataInspectionDialog() { qDeleteAll(this->children()); }
    void init(bool isInd,const QString &src, const QString &bn, ImageFileType ft,int nImgs);
    ucolvec subjectsStatus;
    
protected:
    void keyPressEvent(QKeyEvent *e);
    bool eventFilter(QObject *t, QEvent *e);

private slots:
    void onImageBoxChange(int index);
    void onPreviousButtonClick();
    void onNextButtonClick();
    void onTreeItemPressed(QTreeWidgetItem *item,int col);
    void checkBeforeExit();
};

#endif // DATAINSPECTIONDIALOG_H
