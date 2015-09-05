#ifndef MRIIMAGE_H
#define MRIIMAGE_H

#include <QtCore>
#include <QMessageBox>
#include "myprogressdialog.h"
#include <QFileInfo>
#include "armadillo"
#include "DataTypes.h"
using namespace arma;
using namespace std;


template<typename T>
inline Cube<T> swapCube(const Cube<T> &in, uchar direction)
{
    Cube<T> out = Cube<T>();
    if (direction == SWAP_XY) {                 // swap XY
        out = Cube<T>(in.n_cols,in.n_rows,in.n_slices);
        for (uint i =0; i< in.n_slices;i++)
            out.slice(i) = in.slice(i).t();
    }
    else if (direction == SWAP_XZ) {            // swap XZ
        out = Cube<T>(in.n_slices,in.n_cols,in.n_rows);
        Mat<T> tmp = Mat<T>();
        for (uint i =0; i< in.n_cols;i++) {
            tmp = in(span::all,span(i),span::all);
            out(span::all,span(i),span::all) = tmp.t();
        }
    }
    else if (direction == SWAP_YZ) {            // swap YZ
        out = Cube<T>(in.n_rows,in.n_slices,in.n_cols);
        Mat<T> tmp = Mat<T>();
        for (uint i =0; i< in.n_rows;i++) {
            tmp = in(span(i),span::all,span::all);
            out(span(i),span::all,span::all) = tmp.t();
        }
    }
    return out;
}

template<typename T>
inline Cube<T> mirrorCube(const Cube<T> &in,uchar direction)
{
    Cube<T> out = Cube<T>(in.n_rows,in.n_cols,in.n_slices);
    if (direction == FLIP_X) {          // flip X
        for (uint i =0; i< in.n_slices;i++)
            out.slice(i) = flipud(in.slice(i));
    }
    else if (direction == FLIP_Y) {     // flip Y
        for (uint i =0; i<  in.n_slices;i++)
            out.slice(i) = fliplr(in.slice(i));
    }
    else if (direction == FLIP_Z) {     // flip Z
        Mat<T> tmp = Mat<T>();
        for (uint i =0; i< in.n_cols;i++) {
            tmp = in(span::all,span(i),span::all);
            out(span::all,span(i),span::all) = fliplr(tmp);
        }
    }
    return out;
}

bool readHeader(QString headerFileName,AnalyzeHeader& header,ImageFileType fileType);

inline bool checkImageFile(QString imgFileName, uint fileType, const AnalyzeHeader &header)
{
    {
        if (imgFileName.isEmpty())
            return false;
        // check file openning
        QFile file(imgFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(0, "Error", QString("Cannot read file %1:\n%2.")
                                  .arg(file.fileName()).arg(file.errorString()));
            return false;
        }
        // check file size
        quint32 nRows = header.dime.dim[1],
                nCols = header.dime.dim[2],
                nSlices = header.dime.dim[3],
                nVolumes = header.dime.dim[4],
                nVox = nRows*nCols*nSlices,
                dt = header.dime.datatype,
                fileSize;
        if (dt == DT_BINARY)
            fileSize = nVox/8*nVolumes;
        else if (dt == DT_UNSIGNED_CHAR)
            fileSize = nVox*nVolumes;
        else if (dt == DT_SIGNED_SHORT)
            fileSize = nVox*2*nVolumes;
        else if ( (dt == DT_SIGNED_INT) || (dt == DT_FLOAT))
            fileSize = nVox*4*nVolumes;
        else if (dt == DT_DOUBLE)
            fileSize = nVox*8*nVolumes;
        if (fileType == NIFTI)
            fileSize += header.dime.vox_offset; // add header size
        if (file.size() != fileSize ) {
            QMessageBox::critical(0, "Error", "File size does not agree with header data.");
            file.close();
            return false;
        }
        file.close();
        return true;
    }
}

// read image function
template<typename T>
static bool readImage(QString imgFileName,uint fileType, bool preview,MyProgressDialog *progress,
                      AnalyzeHeader &header,QList< Cube<T> > &out)
{
    if (!checkImageFile(imgFileName, fileType,header))
        return false;
    // check file openning
    QFile file(imgFileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    // check file size
    quint32 nRows = header.dime.dim[1],
            nCols = header.dime.dim[2],
            nSlices = header.dime.dim[3],
            nVolumes = header.dime.dim[4],
            nVox = nRows*nCols*nSlices,
            dt = header.dime.datatype;
    if (fileType == NIFTI)
        file.seek(qint64(header.dime.vox_offset));
    // read file according to the datatype
    // if preview load the first volume only
    if (preview)
        nVolumes = 1;
    // prepare progress bar    
    progress->setLabelText("Reading Image ... ");
    progress->setRange(0,2*nVolumes+2);
    progress->setModal(true);
    progress->setFixedSize(progress->sizeHint());
    progress->setValue(0);
    progress->show();
    // read data
    if (dt == DT_BINARY) {
        // this code is only meaningful in case of uchar
        if (sizeof(T) != sizeof(uchar)) {
            file.close();
            return false;
        }
        int n = nVox*nVolumes/8;
        uchar *data = new uchar[n];
        if (file.read( (char *) data, sizeof(uchar)*n) != sizeof(uchar)*n) {
            QMessageBox::critical(0, "Error", "Can not read file.");
            file.close();
            return false;
        }
        quint32 c = 0;
        // decode bit by bit
        uchar *tmp = new uchar[n*8];
        for (int i = 0; i<n; i++) {
            for (int j = 0; j<8; j++)
                tmp[c++] = (data[i] & (1<<j)) >> j;
        }
        delete data;
        // store data
        for (uint k = 0; k<nVolumes;k++) {
            T *data = new T[nVox];
            for (uint i = 0;i<nVox;i++)
                data[i] = tmp[i+k*nVox];
            out.append(Cube<T>(data,nRows,nCols,nSlices,false,false));
            progress->setValue(progress->value()+1);
        }
        delete tmp;
    }
    else {
        for (uint j = 0; j<nVolumes;j++) {
            out.append(Cube<T>(nRows,nCols,nSlices));
            if (file.read( (char *) out.last().memptr(), sizeof(T)*nVox ) != sizeof(T)*nVox) {
                QMessageBox::critical(0, "Error", "Can not read file.");
                file.close();
                return false;
            }
            progress->setValue(progress->value()+1);
        }
    }
    file.close();
    // correct orientation for display
    // permute rows and columns
    if (fileType == NIFTI || fileType == ANALYZE) {
        for (uint i =0; i<nVolumes;i++) {
            out[i] = swapCube(out[i],SWAP_XY);
            progress->setValue(progress->value()+1);
        }
        // correct header information
        swap(header.dime.dim[1],header.dime.dim[2]);
        swap(header.dime.pixdim[1],header.dime.pixdim[2]);
    }
    return true;
}


// read image function
template<typename T>
void orientCubes(const QVector<uchar> &orientation, MyProgressDialog *progress,
                 AnalyzeHeader &header,QList< Cube<T> > &data)
{
    if (orientation.isEmpty() || data.isEmpty())
        return;
    static umat s = Mat<uword>(2,3);
    s <<1<<2<<endr<<1<<3<<endr<<2<<3<<endr;
    progress->setLabelText("Rotating images ... ");
    progress->setRange(0,orientation.size()+2);
    progress->setValue(0);
    for (int k = 0; k < orientation.size();k++) {
        uint o = orientation.at(k);
        if (o < 3) {    // permutation
            for (int i =0; i<data.size();i++)
                data[i] = swapCube(data[i],orientation.at(k));
            // correct header information
            swap(header.dime.dim[s(o,1)],header.dime.dim[s(o,1)]);
            swap(header.dime.pixdim[s(o,1)],header.dime.pixdim[s(o,1)]);
        }
        else {          // mirroring
            for (int i =0; i<data.size();i++)
                data[i] = mirrorCube(data[i],orientation.at(k));
        }
        progress->setValue(progress->value()+1);
    }
}

/*************************************************************************************************/

// main function to write a file
template<typename T>
bool saveImages(QString fileName,uint fileType,
                AnalyzeHeader header,QList< Cube<T> > &data)
{
    /*
        ANALYZE75WRITE writes image file of Mayo Analyze 7.5 data set.
        ANALYZE75WRITE writes the image data from the IMG file of
        an Analyze 7.5 format data set (a pair of fileName.HDR and fileName.IMG
        files).  For single-frame images, I is an M-by-N array where M is the
        number of rows and N is the number of columns. For multi-dimensional
        images, I can be an M-by-N-by-O or M-by-N-by-O-by-P array where M is
        the number of rows, N is the number of columns, O is the number of
        slices per volume and P is the number of volumes or time points. The
        data type of I is consistent with the image data type specified in
        the metadata obtained from the header file.
    */
    if (!fileName.isEmpty()) {
        // assure correct fileNames for header and image files
        fileName = QFileInfo(fileName).absolutePath() + "/" + QFileInfo(fileName).baseName();
        if (fileType == ANALYZE) {
            QString headerFileName = fileName + ".hdr";
            // read header file
            if (writeHeader(headerFileName, header)) {
                // read image file
                QString imgFileName = fileName + ".img";
                return writeImage(imgFileName, fileType, header, data);
            }
        }
        else if (fileType == NIFTI) {
            QString imgFileName = fileName + ".nii";
            return writeImage(imgFileName, fileType, header, data);
        }
    }
    return false;
}

// write header function
bool writeHeader(QString headerFileName, AnalyzeHeader header);

// write image function
template <typename T>
bool writeImage(QString imgfileName,uint fileType,
                AnalyzeHeader header,QList< Cube<T> > &data)
{
    // check file
    QFile file(imgfileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(0, "Warning!", QString("Cannot write image file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    // permute rows and columns
    for (int i =0; i<data.size();i++)
        data[i] = swapCube(data[i],SWAP_XY);
    // write header for NIFTI files
    if (fileType == NIFTI) {
        // correct header information
        swap(header.dime.dim[1],header.dime.dim[2]);
        swap(header.dime.pixdim[1],header.dime.pixdim[2]);
        file.write( (char *) &header, sizeof(header) );
        file.seek(qint64(header.dime.vox_offset));
    }
    // write file
    uint nVox = data.first().n_elem;
    for (int i = 0;i<data.size();i++)
        if (!(file.write( (char *) data[i].memptr(),
                          sizeof(T)*nVox) == sizeof(T)*nVox)) {
            QMessageBox::critical(0, "Error", "Can not write file.");
            file.close();
            return false;
        }
    file.close();
    return true;
}

// prepare header
AnalyzeHeader prepareHeader(quint32 dim[4],ImageDataType dataType,
                            float pixDim[3],ImageFileType fileType);

DiffModelDimensionParam getParameters(const AnalyzeHeader &header);

#endif // MRIIMAGE_H
