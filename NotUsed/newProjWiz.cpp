#include <QtGui>

#include "newProjWiz.h"

NewProjWiz::NewProjWiz(QWidget *parent)
    : QWizard(parent)
{
    setPage(PAGE_INTRO, new IntroPage);
    setPage(PAGE_1_DW, new Page1DW);
    setPage(PAGE_2_DW, new Page2DW);
    setPage(PAGE_1_BNA,new Page1BNA);
    setPage(PAGE_2_BNA,new Page2BNA);

    setStartId(PAGE_INTRO);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif
    setWindowTitle(tr("Data Analysis Wizard"));
}


IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
    setupUi(this);
    setTitle("New Study Wizard");
    setPixmap(QWizard::WatermarkPixmap, QPixmap("C:/Users/jgadelkarim.UICPSYCH/Dropbox/TractCreator22/"
                                                "images/connectWizard.png"));
    registerField("intro.singleSubject",SingleSubjectRadio);
}
/**************************************************************************************************/
Page1DW::Page1DW(QWidget *parent) : QWizardPage(parent)
{
    setupUi(this);
    setTitle("DW Analysis");
    setSubTitle("Please fill information about the data source and specification. "
                "Also, please choose an imaging model.");
    bValueEdit->setValidator(new QIntValidator(0, 10000, this));

    connect(DataSourceEdit,SIGNAL(textChanged(QString)),this,SLOT(onDataSourceEditChange(QString)));
    connect(DataSourceButton,SIGNAL(clicked()),this,SLOT(onDataSourceButtonPress()));
    connect(gTableListBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ongTableListBoxChange(int)));
    connect(EditTableButton,SIGNAL(clicked()),this,SLOT(onEditTableButtonPress()));
    connect(ModelBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onModelBoxChange(int)));
    connect(SaveFolderButton,SIGNAL(clicked()),this,SLOT(onSaveFolderButtonPress()));

    registerField("page1dw.dataType",DataTypeBox);
    registerField("page1dw.dataSource*",DataSourceEdit);
    registerField("page1dw.gTable",gTableListBox);
    registerField("page1dw.bValue*",bValueEdit);
    registerField("page1dw.imagingModel",ModelBox);
    registerField("page1dw.computeMetricsDTI",ComputeDTIMetricsCheck);
    registerField("page1dw.computeMetricsTDF",ComputeTDFMetricsCheck);
    registerField("page1dw.saveMetricFormat",MetricFormatBox);
    registerField("page1dw.saveFolder*",SaveFolderEdit);

    // scan current folder for gtbl files
    // read all gradient tables, add their file names to the box
    gTableList = new QList<mat>();
    QStringList *namesList = new QStringList();
    if (readGradientTablesFile(gTableList,namesList))
        gTableListBox->addItems(*namesList);
}
void Page1DW::initializePage()
{
    individual = field("intro.singleSubject").toBool();
    SourceBaseNameEdit->setDisabled(individual);
    if (!individual)
        registerField("page1dw.sourceBaseName*",SourceBaseNameEdit);
}

void Page1DW::onDataSourceEditChange(QString str)
{
    Q_UNUSED(str);
    if (individual)
        SourceBaseNameEdit->setText(QFileInfo(DataSourceEdit->text()).baseName());
}

void Page1DW::onDataSourceButtonPress()
{
    // choose a file in case of an individual, a directory for group studies
    QString source;
    if (individual)
        source = QFileDialog::getOpenFileName(this,
                                              "Select file", ".",
                                              "Image files (*.hdr *.img *.nii *.dcm)");
    else {
        source = QFileDialog::getExistingDirectory(this, "Select Directory",".",
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
        if (!source.isEmpty())
            source+="\\";
    }
    if (!source.isEmpty()) {
        DataSourceEdit->setText(source.replace("/","\\"));
        if (individual)
            SourceBaseNameEdit->setText(QFileInfo(source).baseName());
    }

}

void Page1DW::ongTableListBoxChange(int index)
{
    if (gTableList == NULL ||
            gTableList->isEmpty() ||
            index > gTableList->size()-1 ||
            index < 0) {
        nb0ImagesEdit->setText("");
        nGradientsEdit->setText("");
        return;
    }
    colvec v =  sum(gTableList->at(index),1);
    uvec vv = arma::find(v == 0);
    nb0ImagesEdit->setText(QString::number(vv.n_elem));
    nGradientsEdit->setText(QString::number(v.n_elem-vv.n_elem));
}

void Page1DW::onEditTableButtonPress()
{
    gTableDialog w(gTableList,this);
    // fill tables names
    for (int i=0;i<gTableListBox->count();i++)
        w.gTableListBox->addItem(gTableListBox->itemText(i));
    if(!w.exec()) {
        gTableListBox->clear();
        for (int i=0;i<gTableList->size();i++)
            gTableListBox->addItem(w.gTableListBox->itemText(i));
        gTableListBox->setCurrentIndex(w.gTableListBox->currentIndex());
    }
}

void Page1DW::onModelBoxChange(int index)
{
    ComputeDTIMetricsCheck->setEnabled((index == DTI));
    ComputeTDFMetricsCheck->setEnabled((index == TDF));
}

void Page1DW::onSaveFolderButtonPress()
{
    QString folderName = QFileDialog::getExistingDirectory(this, "Select Directory",".",
                                                           QFileDialog::ShowDirsOnly
                                                           | QFileDialog::DontResolveSymlinks);
    if (!folderName.isEmpty()) {
        folderName +="\\";
        SaveFolderEdit->setText(folderName.replace("/","\\"));
    }
}

bool Page1DW::readGradientTablesFile(QList<mat> *tablesList,QStringList *namesList)
{
    if (tablesList == NULL || namesList == NULL)
        return false;
    // scan the internal gradient tables file
    QFile file(QDir::currentPath() + "/gTables.gtbl");
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream in(&file);
    QStringList fields;
    mat gTable = Mat<double>(1024,3);
    int c =0;
    bool ok1,ok2,ok3;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty())
            continue;
        // test for table name, if so, save the table's name and the table
        // reset the table
        if (line.contains("#")) {
            namesList->append(line.remove(QRegExp("#")));
            if (c>0) { // to correct for first one
                gTable.shed_rows(c,1023);
                tablesList->append(gTable);
                gTable = Mat<double>(1024,3);
                c = 0;
            }
            continue;
        }
        // parse text, expected 4 fields: No.:  X, Y, Z
        fields = line.split(QRegExp("\\s+|:|,"), QString::SkipEmptyParts);
        if (fields.size() < 4)
            return false;
        // fill gradient table
        gTable(c,0) = fields.at(1).toDouble(&ok1);
        gTable(c,1) = fields.at(2).toDouble(&ok2);
        gTable(c,2) = fields.at(3).toDouble(&ok3);
        if (!ok1 || !ok2 || !ok3)
            return false;
        c++;
        if (c>1023)
            return false;
    }
    // this for the last one
    gTable.shed_rows(c,1023);
    tablesList->append(gTable);
    return true;
}

bool Page1DW::validatePage()
{
    // check for valid folders entries
    // check the data source folder
    if (!QFileInfo(DataSourceEdit->text()).exists()) {
        QMessageBox::critical(this, "Warning!", "Data source does not exist. "
                              "Please correct it.");
        return false;
    }
    // check subfolderss in case of a group study
    if (!field("intro.singleSubject").toBool()) {
        QStringList allDirs = (QDir(DataSourceEdit->text())).
                entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  |
                          QDir::AllDirs);
        for (int i =0; i<allDirs.size();i++) {
            QString str = DataSourceEdit->text()+"\\"+allDirs.at(i)+ "\\" +SourceBaseNameEdit->text();
            if (!QFileInfo(str).exists()) {
                QMessageBox::critical(this, "Warning!", QString("Data does not exist for subject %1."
                                                                "Can not continue until this is fixed.")
                                      .arg(allDirs.at(i)));
                return false;
            }
        }
    }
    if (!QFileInfo(SaveFolderEdit->text()).exists()) {
        QMessageBox::critical(this, "Warning!", "Save folder does not exist. Please correct it.");
        return false;
    }
    return true;
}

/**************************************************************************************************/
Page2DW::Page2DW(QWidget *parent) : QWizardPage(parent)
{
    setupUi(this);
    setTitle("DW Analysis");
    setSubTitle("Please examine a sample of the data, fill Eddy Current "
                "Corretion options and fill tractography parameters (optional).");
    NoiseLevelEdit->setValidator(new QIntValidator(0, 10000, this));

    connect(ECCBox,SIGNAL(clicked(bool)),this,SLOT(onECCBoxCheck(bool)));
    connect(AlgorithmBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onAlgorithmBoxChange(int)));

    registerField("page2dw.doECC",ECCBox);
    registerField("page2dw.referenceImage",ECCReferenceBox);
    registerField("page2dw.noiseLevel*",NoiseLevelEdit);
    registerField("page2dw.costFunction1",CostFunction1);
    registerField("page2dw.costFunction2",CostFunction2);
    registerField("page2dw.costFunction3",CostFunction3);
    registerField("page2dw.tractPerVoxel",TractsPerSeedSpinner);
    registerField("page2dw.maxBendingAngle",MaxBendAngleSpinner);
    registerField("page2dw.stepSize",StepSizeSpinner);
    registerField("page2dw.minFiber",MinLengthSpinner);
    registerField("page2dw.startFA",StartMetricSpinner);
    registerField("page2dw.stopFA",StopMetricSpinner);
    registerField("page2dw.smoothFibers",SmoothFibersCheck);
}

void Page2DW::initializePage()
{
    if (field("page1dw.imagingModel").toInt() == DTI) {
        // Enable FACT
        AlgorithmBox->setItemData(0,33,Qt::UserRole-1);
        // Disable TDF
        AlgorithmBox->setItemData(1,0,Qt::UserRole-1);
        AlgorithmBox->setItemData(2,0,Qt::UserRole-1);
        AlgorithmBox->setCurrentIndex(0);
    }
    else {
        // Disable FACT
        AlgorithmBox->setItemData(0,0,Qt::UserRole-1);
        // Enable TDF
        AlgorithmBox->setItemData(1,33,Qt::UserRole-1);
        AlgorithmBox->setItemData(2,33,Qt::UserRole-1);
        AlgorithmBox->setCurrentIndex(1);
    }
}

void Page2DW::onAlgorithmBoxChange(int index)
{
    TractsPerSeedSpinner->setEnabled((index == 2)); // TOD PROB
    StartMetricSpinner->setEnabled((index == 0));   // FACT
    StopMetricSpinner->setEnabled((index == 0));    // FACT
}
/**************************************************************************************************/
Page1BNA::Page1BNA(QWidget *parent) : QWizardPage(parent)
{
    setupUi(this);
    setTitle("Connectome Analysis");
    setSubTitle("Please fill required parameters.");
}

/**************************************************************************************************/
Page2BNA::Page2BNA(QWidget *parent) : QWizardPage(parent)
{
    setupUi(this);
    setTitle("Connectome Analysis");
    setSubTitle("Please examine a sample of the data and "
                "correct for orientation if needed.");
}
