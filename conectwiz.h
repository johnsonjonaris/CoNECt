#ifndef CONECTWIZ_H
#define CONECTWIZ_H

#include <QtGui>
#include <QWizard>
#include "ui_intropage.h"
#include "ui_page1dw.h"
#include "ui_page2dw.h"
#include "ui_page1bna.h"
#include "gtabledialog.h"
#include "DataTypes.h"
#include "mriimage.h"
#include "armadillo"

using namespace arma;

enum Algorithm{FACT = 0, TOD_TRACT = 2};
enum { PAGE_INTRO,PAGE_1_DW, PAGE_2_DW, PAGE_1_BNA, PAGE_2_BNA };
class CoNECtWiz : public QWizard
{
    Q_OBJECT

public:
    CoNECtWiz(QWidget *parent = 0);
    void getTractographyParameters(TrackingParameters *tp);
};

class IntroPage : public QWizardPage, public Ui::IntroPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

    int nextId() const {return DwRadio->isChecked()?PAGE_1_DW:PAGE_1_BNA;}

private slots:

};
/**************************************************************************************************/

class Page1DW : public QWizardPage, public Ui::Page1DW
{
    Q_OBJECT

public:
    Page1DW(QWidget *parent = 0);

    void initializePage();
    bool validatePage();
    int nextId() const {return PAGE_2_DW;}
    static bool readGradientTablesFile(QList<mat> *tablesList,QStringList *namesList);


private:
    QList<mat> *gTableList;
    bool individual;

private slots:
    void onDataSourceEditChange(QString);
    void onDataSourceButtonPress();
    void ongTableListBoxChange(int index);
    void onEditTableButtonPress();
    void onModelBoxChange(int index);
    void onSaveFolderButtonPress();
};
/**************************************************************************************************/

class Page2DW : public QWizardPage, public Ui::Page2DW
{
    Q_OBJECT
    int nImages;

public:
    Page2DW(QWidget *parent = 0);

    void initializePage();
    int nextId() const {return -1;}

private slots:
    void onAlgorithmBoxChange(int index);
    // this slot is used to solve the Wizard check on entries
    void onECCBoxCheck(bool status) { if (!status) NoiseLevelEdit->setText("0"); }
};
/**************************************************************************************************/

class Page1BNA : public QWizardPage, public Ui::Page1BNA
{
    Q_OBJECT

public:
    Page1BNA(QWidget *parent = 0);

    void initializePage();
    bool validatePage();
    int nextId() const {return -1;}//return PAGE_2_BNA;}

private:
    bool individual;

private slots:
    void onb0SourceButtonPress();
    void onFiberSourceButtonPress();
    void onLabelSourceButtonPress();
    void onT1SourceButtonPress();
    void onIsFreesurferCheck(bool status);
    void onNamesSourceButtonPress();
    void onSaveFolderButtonPress();

    void onb0SourceEditChange(QString str);
    void onFiberSourceEditChange(QString str);
    void onLabelSourceEditChange(QString str);
    void onT1SouceEditChange(QString str);

    void onRegisterLabelCheckBoxEnable(bool status)
    {   b0BaseNameEdit->setEnabled(!individual && status); }
};
/**************************************************************************************************/
#endif // CONECTWIZ_H
