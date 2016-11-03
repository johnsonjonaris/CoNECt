#ifndef NEWPROJWIZ_H
#define NEWPROJWIZ_H

#include <QWizard>
#include "ui_intropage.h"
#include "ui_page1dw.h"
#include "ui_page2dw.h"
#include "ui_page1bna.h"
#include "ui_page2bna.h"
#include "gtabledialog.h"
#include "armadillo"

using namespace arma;

//namespace Ui {
//class IntroPage;
//class Page1DW;
//class Page2DW;
//class Page1BNA;
//class Page2BNA;
//}

enum {DICOM =0,ANALYZE = 1,NIFTI=2};
enum {DTI = 0, TDF = 1};
enum { PAGE_INTRO,PAGE_1_DW, PAGE_2_DW, PAGE_1_BNA, PAGE_2_BNA };
class NewProjWiz : public QWizard
{
    Q_OBJECT

public:
    NewProjWiz(QWidget *parent = 0);
};

class IntroPage : public QWizardPage, public Ui::IntroPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

    int nextId() const {return DwRadio->isChecked()?PAGE_1_DW:PAGE_1_BNA;}

private slots:

};

class Page1DW : public QWizardPage, public Ui::Page1DW
{
    Q_OBJECT

public:
    Page1DW(QWidget *parent = 0);

    void initializePage();
    int nextId() const {return PAGE_2_DW;}

private:
    QList<mat> *gTableList;
    bool individual;
    bool readGradientTablesFile(QList<mat> *tablesList,QStringList *namesList);
    bool validatePage();

private slots:
    void onDataSourceEditChange(QString);
    void onDataSourceButtonPress();
    void ongTableListBoxChange(int index);
    void onEditTableButtonPress();
    void onModelBoxChange(int index);
    void onSaveFolderButtonPress();
};


class Page2DW : public QWizardPage, public Ui::Page2DW
{
    Q_OBJECT

public:
    Page2DW(QWidget *parent = 0);

    void initializePage();
    int nextId() const {return -1;}

private slots:
    void onAlgorithmBoxChange(int index);
    // this slot is used to solve the Wizard check on entries
    void onECCBoxCheck(bool status) {if (!status); NoiseLevelEdit->setText("0");}
};


class Page1BNA : public QWizardPage, public Ui::Page1BNA
{
    Q_OBJECT

public:
    Page1BNA(QWidget *parent = 0);

    int nextId() const {return PAGE_2_BNA;}

private:
};

class Page2BNA : public QWizardPage, public Ui::Page2BNA
{
    Q_OBJECT

public:
    Page2BNA(QWidget *parent = 0);

    int nextId() const {return -1;}

private:
};

#endif // NEWPROJWIZ_H
