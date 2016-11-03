#include <QCoreApplication>
#include <QtCore>
#include <QtGui>
#include <QFileInfo>

#include "armadillo"
using namespace arma;
using namespace std;

bool openMatlabFile(QString IpFile, cube& NewImg, uvec& volInfo, bool preview)
{
    MATFile *pmat;
    const char **dir, *name;
    int	  ndir;
    // convert fileName
    QByteArray tmp = IpFile.toLocal8Bit();
    const char *matFile = tmp.data();
    // open file and get directory of MAT-file
    pmat = matOpen(matFile, "r");
    if (pmat == NULL) {
        QMessageBox::warning(0, "Open Image", "Cannot read file.");
        return false;
    }
    dir = (const char **)matGetDir(pmat, &ndir);
    if (dir == NULL) {
        QMessageBox::warning(0, "Open Image", "Cannot read file.");
        return false;
    }
    mxFree(dir);
    // In order to use matGetNextXXX correctly, reopen file
    if (matClose(pmat) != 0) {
        QMessageBox::warning(0, "Open Image", "Cannot read file.");
        return false;
    }
    // Read in the file contents: assumed to contain only one structure named "data"
    pmat = matOpen(matFile, "r");
    if (pmat == NULL) {
        QMessageBox::warning(0, "Open Image", "Cannot read file.");
        return false;
    }
    mxArray *data = matGetNextVariable(pmat, &name); // get variable name
    if (data == NULL) {
        QMessageBox::warning(0, "Open Image","Cannot read file.");
        return false;
    }
    if (matClose(pmat) != 0) {
        QMessageBox::warning(0, "Open Image", "Cannot read file.");
        return false;
    }
    // check for a correct file: contains a numerical array
    if ( !mxIsNumeric(data) || mxIsComplex(data)) {
        QMessageBox::warning(0, "Open Image", "The loaded file is an inapropriate matlab file, the file should contains 3D or 4D array.");
        return false;
    }
    // read data into arma volume
    const mwSize *dataSz;
    mwSize nDim;
    // check for correct size
    nDim = mxGetNumberOfDimensions(data);
    if ( nDim < 3 || nDim > 4) {
        QMessageBox::warning(0, "Open Image", "The loaded file is an inapropriate matlab file, the file should contains 3D or 4D array.");
        return false;
    }
    // get data dimension
    uint nRows, nCols, nSlices, nVolumes, nVox;
    dataSz = mxGetDimensions(data);
    nRows = uint(dataSz[0]);
    nCols = uint(dataSz[1]);
    nSlices = uint(dataSz[2]);
    nVox = nRows*nCols*nSlices;
    if (nDim ==4 && !preview)
        nVolumes = uint(dataSz[3]);
    else
        nVolumes = 1;
    volInfo = Col<u32>(5);
    volInfo.at(0) = nRows;
    volInfo.at(1) = nCols;
    volInfo.at(2) = nSlices;
    volInfo.at(3) = nVolumes;
    // get class of data
    NewImg.set_size(nVox*nVolumes,1,1);
    mxClassID dataClass = mxGetClassID(data);
    // get data and store its class in the 5th location in volInfo
//    mxDOUBLE_CLASS    64
//    mxSINGLE_CLASS    16
//    mxINT8_CLASS      2
//    mxUINT8_CLASS     2
//    mxINT16_CLASS     4
//    mxUINT16_CLASS    4
//    mxINT32_CLASS,    8
//    mxUINT32_CLASS    8
//    mxINT64_CLASS     64
//    mxUINT64_CLASS    64
    if (dataClass == mxDOUBLE_CLASS) {
        volInfo(4) = 64;
        double *dataPtr;
        dataPtr = mxGetPr(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxSINGLE_CLASS) {
        volInfo(4) = 16;
        float *dataPtr;
        dataPtr = (float *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxINT8_CLASS) {
        volInfo(4) = 2;
        qint8 *dataPtr;
        dataPtr = (qint8 *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxUINT8_CLASS) {
        volInfo(4) = 2;
        unsigned char *dataPtr;
        dataPtr = (unsigned char *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxINT16_CLASS) {
        volInfo(4) = 4;
        qint16 *dataPtr;
        dataPtr = (qint16 *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxUINT16_CLASS) {
        volInfo(4) = 4;
        quint16 *dataPtr;
        dataPtr = (quint16 *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxINT32_CLASS) {
        volInfo(4) = 8;
        qint32 *dataPtr;
        dataPtr = (qint32 *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxUINT32_CLASS) {
        volInfo(4) = 8;
        quint32 *dataPtr;
        dataPtr = (quint32 *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxINT64_CLASS) {
        volInfo(4) = 64;
        qint64 *dataPtr;
        dataPtr = (qint64 *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);
    }
    else if (dataClass == mxUINT64_CLASS) {
        volInfo(4) = 64;
        quint64 *dataPtr;
        dataPtr = (quint64 *)mxGetData(data);
        for (uint i=0; i<nVox*nVolumes;i++)
            NewImg.at(i,0,0) = double(dataPtr[i]);    }
    NewImg.set_size(nVox*nVolumes,1,1);
    return true;
}

