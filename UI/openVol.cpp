#include "openVol.h"

OpenImageDialog::OpenImageDialog(QString fileName,QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    ByteChoice->setChecked(true);
    RawCheck->setChecked(true);
    // set text validators
    QRegExp regExp("[1-9][0-9]{0,2}");
    IMG_H->setValidator(new QRegExpValidator(regExp, this));
    IMG_W->setValidator(new QRegExpValidator(regExp, this));
    nSlice->setValidator(new QRegExpValidator(regExp, this));
    FOV_H->setValidator(new QDoubleValidator(0.01, 1024, 2, this));
    FOV_W->setValidator(new QDoubleValidator(0.01, 1024, 2, this));
    Thickness->setValidator(new QDoubleValidator(0.01, 10, 4, this));

    // connect signals and slots
    connect(ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(ButtonBox, SIGNAL(accepted()), this, SLOT(checkBeforeExit()));
    connect(IMG_H, SIGNAL(textEdited(QString)),this,SLOT(lineEditTextChanged()));
    connect(IMG_W, SIGNAL(textEdited(QString)),this,SLOT(lineEditTextChanged()));
    connect(nSlice, SIGNAL(textEdited(QString)),this,SLOT(lineEditTextChanged()));
    connect(FOV_H, SIGNAL(textEdited(QString)),this,SLOT(lineEditTextChanged()));
    connect(FOV_W, SIGNAL(textEdited(QString)),this,SLOT(lineEditTextChanged()));
    connect(BitChoice, SIGNAL(clicked()),this,SLOT(lineEditTextChanged()));
    connect(ByteChoice, SIGNAL(clicked()),this,SLOT(lineEditTextChanged()));
    connect(Integer16Choice, SIGNAL(clicked()),this,SLOT(lineEditTextChanged()));
    connect(Integer32Choice, SIGNAL(clicked()),this,SLOT(lineEditTextChanged()));
    connect(FloatChoice, SIGNAL(clicked()),this,SLOT(lineEditTextChanged()));
    connect(DoubleChoice, SIGNAL(clicked()),this,SLOT(lineEditTextChanged()));

    connect(PreviewButton,SIGNAL(clicked()),this,SLOT(preview()));
    connect(RawCheck, SIGNAL(clicked(bool)),VoxSizeGroup,SLOT(setEnabled(bool)));

    connect(SwapXY,SIGNAL(clicked(bool)),this,SLOT(updateSwapXY(bool)));
    connect(SwapXZ,SIGNAL(clicked(bool)),this,SLOT(updateSwapXZ(bool)));
    connect(SwapYZ,SIGNAL(clicked(bool)),this,SLOT(updateSwapYZ(bool)));
    connect(FlipX,SIGNAL(clicked(bool)),this,SLOT(updateFlipX(bool)));
    connect(FlipY,SIGNAL(clicked(bool)),this,SLOT(updateFlipY(bool)));
    connect(FlipZ,SIGNAL(clicked(bool)),this,SLOT(updateFlipZ(bool)));
    //
    previewed = false;
    // save filename
    inputFileName = fileName;
}

void OpenImageDialog::updateSwapXY(bool checked)
{
    if (checked)
        orientation.append(SWAP_XY);
    else {
        for (int i=0; i<orientation.size();i++)
            if (orientation.at(i) == SWAP_XY)
                orientation.remove(i);
    }
    if (previewed) {
        applyRotation();
        drawImg();
    }
}
void OpenImageDialog::updateSwapXZ(bool checked)
{
    if (checked)
        orientation.append(SWAP_XZ);
    else {
        for (int i=0; i<orientation.size();i++)
            if (orientation.at(i) == SWAP_XZ)
                orientation.remove(i);
    }
    if (previewed) {
        applyRotation();
        drawImg();
    }
}
void OpenImageDialog::updateSwapYZ(bool checked)
{
    if (checked)
        orientation.append(SWAP_YZ);
    else {
        for (int i=0; i<orientation.size();i++)
            if (orientation.at(i) == SWAP_YZ)
                orientation.remove(i);
    }
    if (previewed) {
        applyRotation();
        drawImg();
    }
}
void OpenImageDialog::updateFlipX(bool checked)
{
    if (checked)
        orientation.append(FLIP_X);
    else {
        for (int i=0; i<orientation.size();i++)
            if (orientation.at(i) == FLIP_X)
                orientation.remove(i);
    }
    if (previewed) {
        applyRotation();
        drawImg();
    }
}
void OpenImageDialog::updateFlipY(bool checked)
{
    if (checked)
        orientation.append(FLIP_Y);
    else {
        for (int i=0; i<orientation.size();i++)
            if (orientation.at(i) == FLIP_Y)
                orientation.remove(i);
    }
    if (previewed) {
        applyRotation();
        drawImg();
    }
}
void OpenImageDialog::updateFlipZ(bool checked)
{
    if (checked)
        orientation.append(FLIP_Z);
    else {
        for (int i=0; i<orientation.size();i++)
            if (orientation.at(i) == FLIP_Z)
                orientation.remove(i);
    }
    if (previewed) {
        applyRotation();
        drawImg();
    }
}

void OpenImageDialog::lineEditTextChanged()
{
    bool check1 = (IMG_W->hasAcceptableInput() &&
                   FOV_W->hasAcceptableInput());
    bool check2 = (IMG_H->hasAcceptableInput() &&
                   FOV_H->hasAcceptableInput());
    bool check3 = (IMG_W->hasAcceptableInput() &&
                   IMG_H->hasAcceptableInput() &&
                   nSlice->hasAcceptableInput());
    double Dx   = FOV_W->text().toDouble()/IMG_W->text().toDouble();
    double Dy   = FOV_H->text().toDouble()/IMG_H->text().toDouble();
    double blocks  = QFileInfo(inputFileName).size()/(IMG_W->text().toDouble()*
                                                      IMG_H->text().toDouble()*
                                                      nSlice->text().toDouble());
    // update resolution
    if (check1 == true)
        dx->setText(QString::number(Dx,'g',6));
    if (check2 == true)
        dy->setText(QString::number(Dy,'g',6));
    // update number of blocks
    if (check3 == true) {
        if (BitChoice->isChecked())
            nBlocks->setText(QString::number(int(blocks*8.0)));
        else if (ByteChoice->isChecked())
            nBlocks->setText(QString::number(int(blocks)));
        else if (Integer16Choice->isChecked())
            nBlocks->setText(QString::number(int(blocks/2)));
        else if (Integer32Choice->isChecked())
            nBlocks->setText(QString::number(int(blocks/4)));
        else if (FloatChoice->isChecked())
            nBlocks->setText(QString::number(int(blocks/4)));
        else if (DoubleChoice->isChecked())
            nBlocks->setText(QString::number(int(blocks/8)));
    }
}

void OpenImageDialog::checkBeforeExit()
{
    // check if
    bool check1 = (IMG_W->hasAcceptableInput() &&
                   IMG_H->hasAcceptableInput() &&
                   nSlice->hasAcceptableInput());
    bool check2 = (FOV_W->hasAcceptableInput() &&
                   FOV_H->hasAcceptableInput() &&
                   Thickness->hasAcceptableInput());
    if (RawCheck->isChecked() && check1 && check2)
        accept();
    else if (AnalyzeCheck->isChecked())
        accept();
    else if (NIFTI_Check->isChecked())
        accept();
    else
        QMessageBox::warning(this, "Warning!","Please enter valid numbers in all boxes.");
}

void OpenImageDialog::preview()
{
    if (!RawCheck->isChecked()) {
        // clear stuff if raw is not chosen
        IMG_W->clear();      IMG_H->clear();      nSlice->clear();
        nBlocks->clear();
        FOV_W->clear();     FOV_H->clear();
        dx->clear();        dy->clear();        Thickness->clear();
    }
    previewed = false;
    // clear previewing space
    Cor2Lbl->clear();
    Sag2Lbl->clear();
    Ax2Lbl->clear();
    // reset all orientation options
    SwapXY->setChecked(false);
    SwapXZ->setChecked(false);
    SwapYZ->setChecked(false);
    FlipX->setChecked(false);
    FlipY->setChecked(false);
    FlipZ->setChecked(false);
    orientation.clear();
    // start reading
    // check for acceptable inputs if raw data
    bool check1 = (IMG_W->hasAcceptableInput() && IMG_H->hasAcceptableInput() && nSlice->hasAcceptableInput());
    AnalyzeHeader   header;
    // start by reading header data
    ImageFileType ft = fileType();
    QString fn = QFileInfo(inputFileName).absolutePath()+"/"+QFileInfo(inputFileName).baseName();
    QString imgFileName = (ft == RAW)?inputFileName:(ft == ANALYZE)?(fn + ".img"):fn + ".nii";
    // check which type of files
    if (ft == RAW && check1)
        // define dimensions
        getHeader(header);
    else if (ft != RAW) {
        QString headerFileName = (ft == ANALYZE)?(fn + ".hdr"):fn + ".nii";
        // read analyze file
        if (!readHeader(headerFileName, header, ft))
            return;
    }
    else {
        QMessageBox::warning(this, "Load Image!","Please enter valid numbers in all boxes.");
        return;
    }
    // get dimension from header
    quint32 nRowsImg = header.dime.dim[1],
            nColsImg = header.dime.dim[2],
            nSlicesImg = header.dime.dim[3],
            dt = header.dime.datatype;
    fcube tmp;
    // read first volume
    if (checkImageFile(imgFileName,ft,header)) {
        MyProgressDialog *prog = new MyProgressDialog(this);
        prog->disableMain();
        prog->disableCancel();
        if (dt == DT_BINARY) {
            QList< Cube<unsigned char> > out = QList< Cube<unsigned char> >();
            if (readImage(imgFileName,ft,true,prog,header,out))
                newImg = 255*out.first();
            else return;
        }
        else if (dt == DT_UNSIGNED_CHAR) {
            QList< Cube<unsigned char> > out = QList< Cube<unsigned char> >();
            if (readImage(imgFileName,ft,true,prog,header,out))
                newImg = out.first();
            else return;
        }
        else if (dt == DT_SIGNED_SHORT) {
            QList< Cube<short> > out = QList< Cube<short> >();
            if(readImage(imgFileName,ft,true,prog,header,out)) {
                tmp = conv_to<fcube>::from(out.first());
                newImg = conv_to<uchar_cube>::from(tmp/tmp.max()*255.0);
            }
            else return;
        }
        else if (dt == DT_SIGNED_INT) {
            QList< Cube<s32> > out = QList< Cube<s32> >();
            if (readImage(imgFileName,ft,true,prog,header,out)) {
                tmp = conv_to<fcube>::from(out.first());
                newImg = conv_to<uchar_cube>::from(tmp/tmp.max()*255.0);
            }
            else return;
        }
        else if (dt == DT_FLOAT) {
            QList< Cube<float> > out = QList< Cube<float> >();
            if (readImage(imgFileName,ft,true,prog,header,out))
                newImg = conv_to<uchar_cube>::from(out.first()/out.first().max()*255.0);
            else return;
        }
        else if (dt == DT_DOUBLE) {
            QList< Cube<double> > out = QList< Cube<double> >();
            if (readImage(imgFileName,ft,true,prog,header,out))
                newImg = conv_to<uchar_cube>::from(out.first()/out.first().max()*255.0);
            else return;
        }
        delete prog;
    }
    else
        return;
    untouchedImg = newImg;
    // update info
    if (!(fileType() == RAW)) {
        IMG_W->setText(QString::number(nRowsImg));
        IMG_H->setText(QString::number(nColsImg));
        nSlice->setText(QString::number(nSlicesImg));
        nBlocks->setText(QString::number(header.dime.dim[4]));
        FOV_W->setText(QString::number(1.0*nRowsImg*header.dime.pixdim[1]));
        FOV_H->setText(QString::number(1.0*nColsImg*header.dime.pixdim[2]));
        dx->setText(QString::number(header.dime.pixdim[1]));
        dy->setText(QString::number(header.dime.pixdim[2]));
        Thickness->setText(QString::number(header.dime.pixdim[3]));
        // type
        if (dt == DT_BINARY)
            BitChoice->setChecked(true);
        else if (dt == DT_UNSIGNED_CHAR)
            ByteChoice->setChecked(true);
        else if (dt == DT_SIGNED_SHORT)
            Integer16Choice->setChecked(true);
        else if (dt == DT_SIGNED_INT)
            Integer16Choice->setChecked(true);
        else if (dt == DT_FLOAT)
             FloatChoice->setChecked(true);
        else if (dt == DT_DOUBLE)
            DoubleChoice->setChecked(true);
    }
    // plot
    drawImg();
    previewed = true;
}

void OpenImageDialog::drawImg()
{
    // display it
    // for a strange region, an internal compiler error is produced when
    // using QVector instead of a QList
    QVector<QRgb> cTable = getColorTable(0.0).toList().toVector();
    QImage img;
    uchar_mat t;
    // mirror vertically
    t = newImg(span(newImg.n_rows/2),span::all,span::all);
    img = QImage(t.memptr(),newImg.n_cols,newImg.n_slices,newImg.n_cols,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    img = img.scaled(Cor2Lbl->size(),Qt::IgnoreAspectRatio);
    Cor2Lbl->setPixmap(QPixmap::fromImage( img.transformed(QTransform(1,0,0,0,-1,0,0,0,1) )));

    // mirror vertically
    t = newImg(span::all,span(newImg.n_cols/2),span::all);
    img = QImage(t.memptr(),newImg.n_rows,newImg.n_slices,newImg.n_rows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    img = img.scaled(Sag2Lbl->size(),Qt::IgnoreAspectRatio);
    Sag2Lbl->setPixmap(QPixmap::fromImage( img.transformed(QTransform(1,0,0,0,-1,0,0,0,1) )));

    // transpose then mirror vertically
    t = newImg(span::all,span::all,span(newImg.n_slices/2));
    img = QImage(t.memptr(),newImg.n_rows,newImg.n_cols,newImg.n_rows,QImage::Format_Indexed8);
    img.setColorTable(cTable);
    img = img.scaled(Ax2Lbl->size(),Qt::IgnoreAspectRatio);
    Ax2Lbl->setPixmap(QPixmap::fromImage( img.transformed(QTransform(0,-1,0,1,0,0,0,0,1) )));
}

void OpenImageDialog::applyRotation()
{
    // In this function we apply the different rotations
    // note that update in the volume dimensions is not required
    // since the rotation is applied only for preview and dimensions
    // will be corrected during actual read
    newImg = untouchedImg;
    if (orientation.isEmpty())
        return;
    for (int i =0;i<orientation.size();i++) {
        uint o = orientation.at(i);
        newImg = (o<3)?swapCube(newImg,o):mirrorCube(newImg,o);
    }
}

bool OpenImageDialog::getHeader(AnalyzeHeader &header)
{
    if (!(fileType() == RAW))
        return false;
    else {
        header.dime.dim[1] = IMG_W->text().toUInt();
        header.dime.dim[2] = IMG_H->text().toUInt();
        header.dime.dim[3] = nSlice->text().toUInt();
        header.dime.dim[4] = nBlocks->text().toUInt();

        header.dime.pixdim[1] = FOV_W->text().toFloat()/header.dime.dim[1];
        header.dime.pixdim[2] = FOV_H->text().toFloat()/header.dime.dim[2];
        header.dime.pixdim[3] = Thickness->text().toFloat();
        // type
        if(BitChoice->isChecked())
            header.dime.datatype = DT_BINARY;
        else if (ByteChoice->isChecked())           // DT_UNSIGNED_CHAR = 2,
            header.dime.datatype = DT_UNSIGNED_CHAR;
        else if (Integer16Choice->isChecked())      // DT_SIGNED_SHORT = 4,
            header.dime.datatype = DT_SIGNED_SHORT;
        else if (Integer32Choice->isChecked())
            header.dime.datatype = DT_SIGNED_INT;
        else if (FloatChoice->isChecked())          // DT_FLOAT = 16,
            header.dime.datatype = DT_FLOAT;
        else if (DoubleChoice->isChecked())         // DT_DOUBLE = 64
            header.dime.datatype = DT_DOUBLE;
    }
    return true;

}
