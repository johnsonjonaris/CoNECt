#include <QCoreApplication>
#include <QtCore>
#include <QtGui>
#include <QFileInfo>
#include "armadillo"
#include "DataTypes.h"
using namespace arma;
using namespace std;
/* Acceptable values for datatype */
//enum {
//    DT_UNSIGNED_CHAR = 2,
//    DT_SIGNED_SHORT = 4,
//    DT_FLOAT = 16,
//    DT_DOUBLE = 64
//};
// read image function
bool openRowFile(QString ImgFilename, cube& OutVol, uvec volInfo, bool preview)
{
    // check file openning
    QFile file(ImgFilename);
    if (!file.open(QIODevice::ReadOnly)) {
        cout<<"Cannot read image file.";
        QMessageBox::warning(0, "Error", "Cannot read image file.");
        return false;
    }
    // check file size
    uint nRows = volInfo(0),
            nCols = volInfo(1),
            nSlices = volInfo(2),
            nVolumes = volInfo(3),
            nVox = nRows*nCols*nSlices,
            datatype = volInfo(4),
            fileSize;
    if (datatype == DT_UNSIGNED_CHAR)       // 8 bit unsigned integer
        fileSize = nVox*nVolumes;
    else if (datatype == DT_SIGNED_SHORT)   // 16 bit integer
        fileSize = nVox*2*nVolumes;
    else if (datatype == DT_FLOAT)          // 32 bit float
        fileSize = nVox*4*nVolumes;
    else if (datatype == DT_DOUBLE)         // 64 bit double
        fileSize = nVox*8*nVolumes;
    // check file size
    if (file.size() != fileSize ) {
        QMessageBox::warning(0, "Error", "Wrong file size, check input parameters.");
        return false;
    }
    // read file according to the datatype
    // if preview load the first volume only
    if (preview)
        nVolumes = 1;
    OutVol = Cube<double>(nVox*nVolumes,1,1);
    if (datatype == DT_UNSIGNED_CHAR) {
        quint8 tmp;
        for (uint i = 0;i<nVox*nVolumes;i++) {
            file.read( (char *) &tmp, sizeof(tmp) );
            OutVol.at(i,0,0) = double(tmp);
        }
    }
    else if (datatype == DT_SIGNED_SHORT) {
        qint16 tmp;
        for (uint i = 0;i<nVox*nVolumes;i++) {
            file.read( (char *) &tmp, sizeof(tmp) );
            OutVol.at(i,0,0) = double(tmp);
        }
    }
    else if (datatype == DT_FLOAT) {
        float tmp;
        for (uint i = 0;i<nVox*nVolumes;i++) {
            file.read( (char *) &tmp, sizeof(tmp) );
            OutVol.at(i,0,0) = double(tmp);
        }
    }
    else if (datatype == DT_DOUBLE) {
        double tmp;
        for (uint i = 0;i<nVox*nVolumes;i++) {
            file.read( (char *) &tmp, sizeof(tmp) );
            OutVol.at(i,0,0) = tmp;
        }
    }
    OutVol.set_size(nVox,nVolumes,1);
    return true;
}

