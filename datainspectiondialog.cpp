#include "datainspectiondialog.h"

DataInspectionDialog::DataInspectionDialog(QWidget *parent) :  QDialog(parent)
{
    setupUi(this);
    CoronalLabel->installEventFilter(this);
    SagittalLabel->installEventFilter(this);
    AxialLabel->installEventFilter(this);
    // connect signals and slots
    connect(ImagesBox,SIGNAL(activated(int)),this,SLOT(onImageBoxChange(int)));
    connect(PreviousButton,SIGNAL(clicked()),this,SLOT(onPreviousButtonClick()));
    connect(NextButton,SIGNAL(clicked()),this,SLOT(onNextButtonClick()));
    connect(SubjectsTree,SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this,SLOT(onTreeItemPressed(QTreeWidgetItem*,int)));
    connect(ButtonBox, SIGNAL(accepted()), this, SLOT(checkBeforeExit()));
}

void DataInspectionDialog::init(bool isInd,const QString &src, const QString &bn,
                                ImageFileType ft,int nImgs)
{
    // note that fileType should never be allowed to be RAW
    individual = isInd;
    path = src;
    baseName = bn;
    fileType = ft;
    expectedNImages = nImgs;

    PreviousButton->setEnabled(false);
    if (individual) {
        // if individual, disable all control buttons, no need for tree
        NextButton->setEnabled(false);
        Subject_ID_Text->setEnabled(false);
        SubjectsTree->setEnabled(false);
    }
    else {
        QStringList subjects = QDir(path).entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
        nSubjects = subjects.size();
        NextButton->setEnabled(nSubjects > 1);
        // identify first subject
        Subject_ID_Text->setText("Subject ID: "+subjects.first());
        SubjectsTree->clear();
        // prepare tree
        for (int i =0; i<subjects.size();i++) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setData(0,Qt::CheckStateRole,true);
            item->setText(1,subjects.at(i));
            item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
            SubjectsTree->insertTopLevelItem(i,item);
        }
        SubjectsTree->setColumnWidth(0,36);
        SubjectsTree->setColumnWidth(1,64);
        SubjectsTree->setCurrentItem(SubjectsTree->invisibleRootItem()->child(0));
    }
    subjectIndex = 0;
    imageIndex = 0;
    // prepare color table
    for (int i = 0;i<256;i++)
        cTable.append(qRgb( i, i, i ));

    // load image
    clearDisplay();
    data.clear();
    updateSubject();
}

bool DataInspectionDialog::eventFilter(QObject *t, QEvent *e)
{
    if (e->type() == QEvent::Wheel) {
        QWheelEvent *we = static_cast<QWheelEvent *>(e);
        int s = we->delta()/120;
        if (t == CoronalLabel) {
            int slice = qMin(qMax(imgRow+s,0),nRows-1);
            if (slice != imgRow) {
                imgRow = slice;
                plotCoronal(imgRow);
            }
        }
        else if (t == SagittalLabel) {
            int slice = qMin(qMax(imgCol+s,0),nCols-1);
            if (slice != imgCol) {
                imgCol = slice;
                plotSagittal(imgCol);
            }
        }
        else if (t == AxialLabel) {
            int slice = qMin(qMax(imgSlice+s,0),nSlices-1);
            if (slice != imgSlice) {
                imgSlice = slice;
                plotAxial(imgSlice);
            }
        }
    }
    // this is a must otherwise keys are not handled
    return QDialog::eventFilter(t,e);
}

void DataInspectionDialog::onImageBoxChange(int index)
{
    if (index != imageIndex) {
        imageIndex = index;
        displayImage();
    }
}

void DataInspectionDialog::onPreviousButtonClick()
{
    subjectIndex--;
    PreviousButton->setDisabled(subjectIndex == 0);
    NextButton->setEnabled(subjectIndex < nSubjects);
    SubjectsTree->setCurrentItem(SubjectsTree->invisibleRootItem()->child(subjectIndex));
    updateSubject();
}

void DataInspectionDialog::onNextButtonClick()
{
    subjectIndex++;
    NextButton->setDisabled(subjectIndex == nSubjects-1);
    PreviousButton->setEnabled(subjectIndex > 0);
    SubjectsTree->setCurrentItem(SubjectsTree->invisibleRootItem()->child(subjectIndex));
    updateSubject();
}

void DataInspectionDialog::onTreeItemPressed(QTreeWidgetItem *item, int col)
{
    if ( (item == NULL) || SubjectsTree->selectedItems().isEmpty() || col == -1)
        return;
    uint index = SubjectsTree->invisibleRootItem()->indexOfChild(item);
    qDebug()<<"tree ind "<<index<<" current: "<<subjectIndex;
    if (index == subjectIndex)
        return;
    subjectIndex = index;
    PreviousButton->setDisabled(subjectIndex == 0);
    NextButton->setDisabled(subjectIndex == nSubjects-1);
    updateSubject();
}

void DataInspectionDialog::updateSubject()
{
    QString sb = "";
    if (!individual) {
        sb = SubjectsTree->currentItem()->text(1);
        Subject_ID_Text->setText("Subject ID: "+sb);
    }
    if (openImage())
        displayImage();
    else {
        if (individual)
            QMessageBox::critical(this,"Error opening data",
                                  "Could not open data for subject "+baseName+".");
        else
            QMessageBox::critical(this,"Error opening data",
                                  "Could not open data for subject "+sb+".");
    }
}

void DataInspectionDialog::checkBeforeExit()
{
    if (individual)
        accept();
    QTreeWidgetItem *root = SubjectsTree->invisibleRootItem();
    if (root == NULL)
        accept();
    subjectsStatus = ucolvec(root->childCount());
    for (int i = 0; i < root->childCount();++i)
        subjectsStatus(i) = root->child(i)->checkState(0);
    accept();
}

// Handle Image Control
bool DataInspectionDialog::openImage()
{
    if (fileType == RAW) return false;
    QString fileName;
    if (individual)
        fileName = path+"/"+baseName;
    else
        fileName = path+"/"+SubjectsTree->currentItem()->text(1)+"/"+baseName;

    if (fileName.isEmpty())
        return false;
    // read analyze
    AnalyzeHeader header;
    MyProgressDialog *progress = new MyProgressDialog(this);
    progress->disableMain();
    progress->disableCancel();
    // start by reading header data
    QString fn = QFileInfo(fileName).absolutePath()+"/"+QFileInfo(fileName).baseName();
    QString imgFileName = (fileType == ANALYZE)?(fn+".img"):(fn+".nii");
    QString headerFileName = (fileType == ANALYZE)?(fn+".hdr"):(fn+".nii");
    if (!readHeader(headerFileName, header, fileType))
        return false;
    // Acquire information from the header
    uint dt = header.dime.datatype;
    data.clear();
    // read data according to its type, diffusion data can not be float or double
    if (dt == DT_UNSIGNED_CHAR) {
        if (readImage(imgFileName,fileType,false,progress,header,data)) {
        } else return false;
    }
    else if (dt == DT_SIGNED_SHORT) {
        QList< Cube<short> > out = QList< Cube<short> >();
        if (readImage(imgFileName,fileType,false,progress,header,out)) {
            fcube tmp;
            for (int i =0;i<out.size();i++) {
                tmp = conv_to<fcube>::from(out.at(i));
                tmp = 255.0*(tmp - tmp.min())/(tmp.max()-tmp.min());
                data.append(conv_to<uchar_cube>::from(tmp));
                tmp.clear();
            }
            out.clear();
        } else return false;
    }
    else if (dt == DT_SIGNED_INT) {
        QList< Cube<s32> > out = QList< Cube<s32> >();
        if(readImage(imgFileName,fileType,false,progress,header,out)) {
            fcube tmp;
            for (int i =0;i<out.size();i++) {
                tmp = conv_to<fcube>::from(out.at(i));
                tmp = 255.0*(tmp - tmp.min())/(tmp.max()-tmp.min());
                data.append(conv_to<uchar_cube>::from(tmp));
                tmp.clear();
            }
            out.clear();
        } else return false;
    }
    else {
        QMessageBox::critical(this,"Faulty data","Non integer data can not be used for diffusion.");
        delete progress;
        return false;
    }
    // store volumes info
    nRows = data.first().n_rows;
    nCols = data.first().n_cols;
    nSlices = data.first().n_slices;
    // update images box
    ImagesBox->clear();
    // below is a workaround due to an internal compiler error
    // adding item by item causes that error
    QStringList list;
    for (int i = 0;i < data.size();i++)
        list.append(QString("Image %1").arg(i));
    ImagesBox->addItems(list);
    ImagesBox->setCurrentIndex(0);
    imageIndex = 0;
    progress->setValue(progress->maximum());
    QApplication::restoreOverrideCursor();
    delete progress;
    if (expectedNImages != data.size())
        QMessageBox::critical(this,"Image mismatch","Number of loaded images does not match "
                              "the gradient table ("+QString::number(expectedNImages)+").");
    return true;
}

void DataInspectionDialog::clearDisplay()
{
    CoronalLabel->clear();
    SagittalLabel->clear();
    AxialLabel->clear();
}

void DataInspectionDialog::displayImage()
{
    clearDisplay();
    imgRow = nRows/2;
    imgCol = nCols/2;
    imgSlice = nSlices/2;
    plotCoronal(imgRow);
    plotSagittal(imgCol);
    plotAxial(imgSlice);
}

void DataInspectionDialog::plotCoronal(int slice)
{
    if (data.isEmpty() || imageIndex == -1)
        return;
    // mirror vertically
    uchar_mat t = data.at(imageIndex)(span(slice),span::all,span::all);
    QImage img = QImage(t.memptr(),nCols,nSlices,nCols,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    img = img.scaled(QSize(250,250));
    CoronalLabel->setPixmap(QPixmap::fromImage( img.transformed(QTransform(1,0,0,0,-1,0,0,0,1)) ));
}

void DataInspectionDialog::plotSagittal(int slice)
{
    if (data.isEmpty() || imageIndex == -1 || imageIndex > data.size()-1)
        return;
    // mirror vertically
    uchar_mat t = data.at(imageIndex)(span::all,span(slice),span::all);
    QImage img = QImage(t.memptr(),nRows,nSlices,nRows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    img = img.scaled(QSize(250,250));
    SagittalLabel->setPixmap(QPixmap::fromImage( img.transformed(QTransform(1,0,0,0,-1,0,0,0,1)) ));
}

void DataInspectionDialog::plotAxial(int slice)
{
    if (data.isEmpty() || imageIndex == -1 || imageIndex > data.size()-1)
        return;
    // transpose then mirror vertically
    uchar_mat t = data.at(imageIndex)(span::all,span::all,span(slice));
    QImage img = QImage(t.memptr(),nRows,nCols,nRows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    img = img.scaled(QSize(250,250));
    AxialLabel->setPixmap(QPixmap::fromImage( img.transformed(QTransform(0,-1,0,1,0,0,0,0,1)) ));
}

void DataInspectionDialog::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    if (key == Qt::Key_4) {
        if (!PreviousButton->isEnabled())
            return;
        PreviousButton->click();
    }
    else if (key == Qt::Key_6) {
        if (!NextButton->isEnabled())
            return;
        NextButton->click();
    }
    else if (key == Qt::Key_8) {
        if (imageIndex == 0)
            return;
        ImagesBox->setCurrentIndex(--imageIndex);
        displayImage();
        return;
    }
    else if (key == Qt::Key_2) {
        if (imageIndex == ImagesBox->count()-1)
            return;
        ImagesBox->setCurrentIndex(++imageIndex);
        displayImage();
        return;
    }
    else if (key == Qt::Key_S) {
        Qt::CheckState state = SubjectsTree->currentItem()->checkState(0);
        state = (state == Qt::Checked)?Qt::Unchecked:Qt::Checked;
        SubjectsTree->currentItem()->setCheckState(0,state);
        return;
    }
    else
        QDialog::keyPressEvent(e);
}

