#include "fibertracts.h"

FiberTracts::FiberTracts()
{
    // default parameters, not used in the program
    // needed for compatability with DTIStudio
    ffh.nFiberNr = 0;
    ffh.nFiberLenMax = 0;
    ffh.fFiberLenMean = 0;

    ffh.nImgWidth = 0;
    ffh.nImgHeight = 0;
    ffh.nImgSlices = 0;

    ffh.fPixelSizeWidth = 0;
    ffh.fPixelSizeHeight = 0;
    ffh.fSliceThickness = 0;

    ffh.enumSliceOrientation = 0;
    ffh.enumSliceSequencing = 0;
}

bool FiberTracts::readFiber(const QString &fileName,const DiffusionModelDimension &dmp,
                            MyProgressDialog *progress, bool append)
{
    // try opening the file
    QFile file(fileName);
    QFileInfo fi(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, "Warning!", QString("Cannot read file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    // read file header
    FiberFileHeader ffhNew;
    file.read( (char *) &ffhNew, sizeof(FiberFileHeader) );
    // test it is a DTIStudio fiber file
    QString test = QString(ffhNew.sFiberFileTag);
    test.resize(8);
    if ( test != "FiberDat" ) {
        QMessageBox::critical(0, "Warning!", "This is not a Fiber file.");
        file.close();
        return false;
    }
    // swap x and y for DTIStudio files, note that those parameters are not needed
    // in the reading process, but they are needed in compatability checks
    // and length calculation
    if (fi.suffix() == "dat") {
        qSwap(ffhNew.fPixelSizeWidth,ffhNew.fPixelSizeHeight);
        qSwap(ffhNew.nImgWidth,ffhNew.nImgHeight);
    }
    // Notify user if the loaded file doesn't agree with the loaded model
    bool test2;
    test2 = ffhNew.nImgWidth         == dmp.nRows &&
            ffhNew.nImgHeight        == dmp.nCols &&
            ffhNew.nImgSlices        == dmp.nSlices &&
            ffhNew.fPixelSizeWidth   == dmp.dx &&
            ffhNew.fPixelSizeHeight  == dmp.dy &&
            ffhNew.fSliceThickness   == dmp.dz;
    if(!test2)
        QMessageBox::warning(0, "Warning!",
                             "The file dimensions are not compatible with the current model.");

    // if append, test both fibers are compatible
    if (append) {
        test2 = ffh.nImgWidth         == ffhNew.nImgWidth &&
                ffh.nImgHeight        == ffhNew.nImgHeight &&
                ffh.nImgSlices        == ffhNew.nImgSlices &&
                ffh.fPixelSizeWidth   == ffhNew.fPixelSizeWidth &&
                ffh.fPixelSizeHeight  == ffhNew.fPixelSizeHeight &&
                ffh.fSliceThickness   == ffhNew.fSliceThickness;
        if (!test2) {
            int msgBox = QMessageBox(QMessageBox::Critical, "CoNECt",
                                     "The file dimensions are not compatible "
                                     "with the current fibers. Discard current "
                                     "fibers or cancel loading?",
                                     QMessageBox::Yes|QMessageBox::No).exec();
            if (msgBox == QMessageBox::Yes)
                this->clear();
            else
                return false;
        }
    }
    else
        // make sure we empty the fibers before loading
        this->clear();
    ffh = ffhNew;
    progress->setLabelText("Reading fibers ... ");
    progress->setRange(0,ffh.nFiberNr);
    progress->setFixedSize(progress->sizeHint());
    progress->setModal(true);
    progress->show();

    // read fiber data here
    // move file pointer 128, where fiber-data starts
    file.seek(128);
    FiberHeader fh;
    double length;
    QVector3D scale = QVector3D(ffh.fPixelSizeWidth,ffh.fPixelSizeHeight,ffh.fSliceThickness);
    // update the length vector size, note that we assume appending fibers
    // note resize is used to preserve data
    fLength.resize(ffh.nFiberNr+fXYZChain.size(),1);
    fXYZChain.reserve(fLength.size());
    int sz = 0;
    for (int i=0; i<ffh.nFiberNr; i++) {
         // read fiber header
        file.read((char *) &fh, sizeof(FiberHeader));
        // save color
        fColors.append(QColor(fh.rgbColor.r,
                              fh.rgbColor.g,
                              fh.rgbColor.b));
        // fill the fXYZChain list
        if (fi.suffix() == "fib") {         // our format
            // test for correct reading
            fXYZChain.append(QVector<QVector3D>());
            fXYZChain.last().resize(fh.nLength);
            sz = sizeof(QVector3D)*fh.nLength;
            if (file.read((char *) fXYZChain.last().data(), sz) != sz) {
                QMessageBox::critical(0, "Warning!", QString("Cannot continue reading file %1:\n%2.")
                                      .arg(file.fileName()).arg(file.errorString()));
                file.close();
                return false;
            }
        }
        else if (fi.suffix() == "dat") {    // DTIStudio, swap XY and flip X
            // read fiber-chain data
            TagXYZ_TRIPLE *pts = new TagXYZ_TRIPLE[fh.nLength];
            sz = sizeof(TagXYZ_TRIPLE)*fh.nLength;
            if (file.read((char *) pts, sz) != sz) {
                QMessageBox::critical(0, "Warning!", QString("Cannot continue reading file %1:\n%2.")
                                      .arg(file.fileName()).arg(file.errorString()));
                file.close();
                return false;
            }
            fXYZChain.append(QVector<QVector3D>());
            fXYZChain.last().reserve(fh.nLength);
            for (int k=0;k<fh.nLength;k++) {
                // if slicing sequencing is in negative,
                // adjusting z-component before saving the data
                if (ffh.enumSliceSequencing == NEGATIVE_Seq)
                    pts[k].z = (ffh.nImgSlices-1) - pts[k].z;
                fXYZChain.last().append(QVector3D(ffh.nImgWidth-pts[k].y-1,pts[k].x,pts[k].z));
            }
            // clear memory
            delete pts;
            pts = NULL;
        }
        // get the fiber length
        length = 0;
        for (int m =0; m<fXYZChain.last().size()-1;m++)
            length += (scale*(fXYZChain.last().at(m)-fXYZChain.last().at(m+1))).length();
        // update the fXYZChain and fLength lists
        // By default, loaded fibers are appended to current fibers
        fLength(fXYZChain.size()-1) = length;
        progress->setValue(i);
    }
    // update the number of fibers
    ffh.nFiberNr = fXYZChain.size();
    file.close();
    return true;
}

bool FiberTracts::writeFiber(const QString &fileName,const QList<quint32> &saveIdx,
                             MyProgressDialog *progress)
{
    if (saveIdx.isEmpty())
        return false;
    // try opening the file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(0, "Warning!", QString("Cannot write file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    progress->setLabelText("Writing Fiber ... ");
    progress->setRange(0,saveIdx.size());
    progress->setModal(true);
    progress->setFixedSize(progress->sizeHint());
    progress->show();

    // prepare and write header
    char temp[] = "FiberDat";
    for (int i=0;i<8;++i)
        ffh.sFiberFileTag[i] = temp[i];
    ffh.nFiberNr = saveIdx.size();
    if (file.write( (char *) &ffh, sizeof(FiberFileHeader) ) == -1) {
        file.close();
        return false;
    }
    // read fiber data here
    // move file pointer 128, where fiber-data starts;
    if(!file.seek(128)) {
        file.close();
        return false;
    }
    FiberHeader fh;
    int idx;
    for (int i=0; i<ffh.nFiberNr; i++) {
        idx = saveIdx.at(i);
        // prepare fiber header
        fh.nLength      = fXYZChain[idx].size();
        fh.nSelStatus   = 1;
        fh.rgbColor.r   = fColors[idx].red();
        fh.rgbColor.g   = fColors[idx].green();
        fh.rgbColor.b   = fColors[idx].blue();
        fh.nSelLenStart = 0;
        fh.nSelLenEnd   = fXYZChain[idx].size();
         // write fiber header
        if ( file.write((char *) &fh, sizeof(FiberHeader)) == -1) {
            file.close();
            return false;
        }
        // write fiber-chain data
        if (file.write((char *) fXYZChain.at(idx).data(), sizeof(QVector3D)*fh.nLength) == -1) {
            file.close();
            return false;
        }
        progress->setValue(i);
    }
    file.close();
    return true;
}

void FiberTracts::filterFibersByLength(QList<quint32> &list,int minL,int maxL)
{
    if (isEmpty() || list.isEmpty())
        return;
    quint32 idx;
    QList<quint32> tmp;
    tmp.reserve(list.size());
    for (int j = 0 ; j<list.size();++j) {
        idx = list.at(j);
        // check for length conditon and remove fiber if not satisfied
        if (minL < fLength[idx] && fLength[idx] < maxL)
            tmp.append(idx);
    }
    list.swap(tmp);
}

void FiberTracts::clear()
{
    ffh.nFiberNr = 0;
    ffh.nFiberLenMax = 0;
    ffh.fFiberLenMean = 0;
    ffh.enumSliceOrientation = 0;
    ffh.enumSliceSequencing = 0;

    fXYZChain.clear();
    fLength.clear();
    fColors.clear();
}





