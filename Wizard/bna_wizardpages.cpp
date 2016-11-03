#include "Wizard\conectwiz.h"

Page1BNA::Page1BNA(QWidget *parent) : QWizardPage(parent)
{
    setupUi(this);
    setTitle("Connectome Analysis");
    setSubTitle("Please fill required parameters.");

    connect(b0SourceButton,SIGNAL(clicked()),this,SLOT(onb0SourceButtonPress()));
    connect(FiberSourceButton,SIGNAL(clicked()),this,SLOT(onFiberSourceButtonPress()));
    connect(LabelSourceButton,SIGNAL(clicked()),this,SLOT(onLabelSourceButtonPress()));
    connect(T1SourceButton,SIGNAL(clicked()),this,SLOT(onT1SourceButtonPress()));
    connect(IsFreesurferCheck,SIGNAL(toggled(bool)),this,SLOT(onIsFreesurferCheck(bool)));
    connect(NamesSourceButton,SIGNAL(clicked()),this,SLOT(onNamesSourceButtonPress()));
    connect(SaveFolderButton,SIGNAL(clicked()),this,SLOT(onSaveFolderButtonPress()));

    connect(b0SourceEdit,SIGNAL(textChanged(QString)),this,SLOT(onb0SourceEditChange(QString)));
    connect(FiberSourceEdit,SIGNAL(textChanged(QString)),this,SLOT(onFiberSourceEditChange(QString)));
    connect(LabelSourceEdit,SIGNAL(textChanged(QString)),this,SLOT(onLabelSourceEditChange(QString)));
    connect(T1SourceEdit,SIGNAL(textChanged(QString)),this,SLOT(onT1SouceEditChange(QString)));

    connect(RegisterLabelCheckBox,SIGNAL(clicked(bool)),this,SLOT(onRegisterLabelCheckBoxEnable(bool)));

    registerField("page1bna.fiberSourceFolder*",FiberSourceEdit);
    registerField("page1bna.labelSourceFolder*",LabelSourceEdit);
    registerField("page1bna.b0SourceFolder*",b0SourceEdit);
    registerField("page1bna.namesSourceFile*",NamesLineEdit);
    registerField("page1bna.T1SouceFolder",T1SourceEdit);
    registerField("page1bna.saveFolder*",SaveFolderEdit);
    registerField("page1bna.computeMetrics",ComputeMetricsCheck);

    registerField("page1bna.fiberFileType",FiberFileTypeBox);
    registerField("page1bna.labelFileType",LabelFileTypeBox);
    registerField("page1bna.b0FileType",b0FileTypeBox);
    registerField("page1bna.T1FileType",T1FileTypeBox);

    registerField("page1bna.T1BaseName",T1BaseNameEdit);

    registerField("page1bna.isFreesurfer",IsFreesurferCheck);
}

void Page1BNA::initializePage()
{
    individual = field("intro.singleSubject").toBool();
    FiberBaseNameEdit->setDisabled(individual);
    LabelBaseNameEdit->setDisabled(individual);
    b0BaseNameEdit->setDisabled(individual);
    if (individual) {
        registerField("page1bna.b0BaseName",b0BaseNameEdit);
        registerField("page1bna.fiberBaseName",FiberBaseNameEdit);
        registerField("page1bna.labelBaseName",LabelBaseNameEdit);
    }
    else {
        registerField("page1bna.b0BaseName*",b0BaseNameEdit);
        registerField("page1bna.fiberBaseName*",FiberBaseNameEdit);
        registerField("page1bna.labelBaseName*",LabelBaseNameEdit);
    }
}

void Page1BNA::onb0SourceButtonPress()
{
    // choose a file in case of an individual, a directory for group studies
    QString source;
    QChar sep = QDir::separator();
    if (individual) {
            source = QFileDialog::getOpenFileName(this,
                                                  "Select file", ".",
                                                  "Image files (*.hdr *.img *.nii)");
    }
    else {
        source = QFileDialog::getExistingDirectory(this, "Select Directory",".",
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
        if (!source.isEmpty())
            source += sep;
    }
    if (!source.isEmpty()) {
        b0SourceEdit->setText(source.replace("/",sep));
        if (individual)
            b0BaseNameEdit->setText(QFileInfo(source).baseName());
    }
}

void Page1BNA::onFiberSourceButtonPress()
{
    // choose a file in case of an individual, a directory for group studies
    QString source;
    QChar sep = QDir::separator();
    if (individual) {
            source = QFileDialog::getOpenFileName(this,
                                                  "Select file", ".",
                                                  "Fiber files (*.fib)");
    }
    else {
        source = QFileDialog::getExistingDirectory(this, "Select Directory",".",
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
        if (!source.isEmpty())
            source += sep;
    }
    if (!source.isEmpty()) {
        FiberSourceEdit->setText(source.replace("/",sep));
        if (individual)
            FiberBaseNameEdit->setText(QFileInfo(source).baseName());
    }
}

void Page1BNA::onLabelSourceButtonPress()
{
    // choose a file in case of an individual, a directory for group studies
    QString source;
    QChar sep = QDir::separator();
    if (individual) {
            source = QFileDialog::getOpenFileName(this,
                                                  "Select file", ".",
                                                  "Image files (*.hdr *.img *.nii)");
    }
    else {
        source = QFileDialog::getExistingDirectory(this, "Select Directory",".",
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
        if (!source.isEmpty())
            source += sep;
    }
    if (!source.isEmpty()) {
        LabelSourceEdit->setText(source.replace("/",sep));
        if (individual)
            LabelBaseNameEdit->setText(QFileInfo(source).baseName());
    }
}

void Page1BNA::onT1SourceButtonPress()
{
    // choose a file in case of an individual, a directory for group studies
    QString source;
    QChar sep = QDir::separator();
    if (individual) {
            source = QFileDialog::getOpenFileName(this,
                                                  "Select file", ".",
                                                  "Image files (*.hdr *.img *.nii)");
    }
    else {
        source = QFileDialog::getExistingDirectory(this, "Select Directory",".",
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
        if (!source.isEmpty())
            source += sep;
    }
    if (!source.isEmpty()) {
        T1SourceEdit->setText(source.replace("/",sep));
        if (individual)
            T1BaseNameEdit->setText(QFileInfo(source).baseName());
    }
}

void Page1BNA::onSaveFolderButtonPress()
{
    // choose a folder to save data
    QString folderName;
    QChar sep = QDir::separator();
    folderName = QFileDialog::getExistingDirectory(this, "Select Directory",".",
                                                   QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
    if (!folderName.isEmpty()) {
        folderName += sep;
        SaveFolderEdit->setText(folderName.replace("/",sep));
    }
}


void Page1BNA::onb0SourceEditChange(QString str)
{
    Q_UNUSED(str);
    if (individual)
        b0BaseNameEdit->setText(QFileInfo(b0SourceEdit->text()).baseName());
}

void Page1BNA::onFiberSourceEditChange(QString str)
{
    Q_UNUSED(str);
    if (individual)
        FiberBaseNameEdit->setText(QFileInfo(FiberSourceEdit->text()).baseName());
}

void Page1BNA::onLabelSourceEditChange(QString str)
{
    Q_UNUSED(str);
    if (individual)
        LabelBaseNameEdit->setText(QFileInfo(LabelSourceEdit->text()).baseName());
}

void Page1BNA::onIsFreesurferCheck(bool status)
{
    if (status) {
        QChar sep = QDir::separator();
        NamesLineEdit->setText(QDir::currentPath().replace('/',sep)+sep+"FreesurferLabels.txt");
    }
}

void Page1BNA::onNamesSourceButtonPress()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open label names file", ".",
                                                    QString("(*.txt)"));
    if (fileName.isEmpty())
        return;
    NamesLineEdit->setText(fileName.replace('/',QDir::separator()));
}

void Page1BNA::onT1SouceEditChange(QString str)
{
    Q_UNUSED(str);
    if (individual)
        T1BaseNameEdit->setText(QFileInfo(T1SourceEdit->text()).baseName());
}

bool Page1BNA::validatePage()
{
    /*
      need to check:
      All folders exists
      If individual:
      All files exists
      If group:
      1) number of subjects is the same and equal in all folders
      2) subjects contains the required files

      Also save folder exists for both cases
      */

    // check for valid files/folders entries
    // check the data source folders
    if (!QFileInfo(FiberSourceEdit->text()).exists()) {
        QMessageBox::critical(this, "Warning!", "Fiber source does not exist. Please correct it.");
        return false;
    }
    if (!QFileInfo(LabelSourceEdit->text()).exists()) {
        QMessageBox::critical(this, "Warning!", "Label source does not exist. Please correct it.");
        return false;
    }
    if (RegisterLabelCheckBox->isChecked() && !QFileInfo(b0SourceEdit->text()).exists()) {
        QMessageBox::critical(this, "Warning!", "b0 source does not exist. Please correct it.");
        return false;
    }
    if (RegisterLabelCheckBox->isChecked() && !QFileInfo(T1SourceEdit->text()).exists()) {
        QMessageBox::critical(this, "Warning!", "T1 source does not exist. Please correct it.");
        return false;
    }
    // group study tests
    // check every folder have the same subjects and that they contain the correct files
    if (!individual) {
        QStringList allSubj1 = QDir(FiberSourceEdit->text()).entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
        QStringList allSubj2 = QDir(LabelSourceEdit->text()).entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
        if (allSubj1.isEmpty() || allSubj2.isEmpty()) {
            QMessageBox::critical(this, "Warning!", "No subjects in the Fiber and/or Label folders.");
            return false;
        }
        if (allSubj1 != allSubj2) {
            QMessageBox::critical(this, "Warning!", "Missing subjects. Directory contents mismatch "
                                  "between Fiber and Label folders. Please correct it.");
            return false;
        }
        // leave rest of the checks during processing

    }


    if (!QFileInfo(SaveFolderEdit->text()).exists()) {
        QMessageBox::critical(this, "Warning!", "Save folder does not exist. Please correct it.");
        return false;
    }
    return true;

}

/**************************************************************************************************/


