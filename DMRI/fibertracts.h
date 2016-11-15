#ifndef FIBERTRACTS_H
#define FIBERTRACTS_H

#include <QtGui>
#include "UI\myprogressdialog.h"
#include "armadillo"
#include "datatypes.h"

using namespace arma;   // don't forget it VIP

// define DTIStudio fiber header
// define fiber header, RGB and XYZ structures
struct RGB_TRIPLE { quint8 r, g, b; };
struct TagXYZ_TRIPLE { float x, y, z; };

struct FiberHeader
{
    qint32          nLength;            ///< fiber length;
    qint8           nSelStatus;
    RGB_TRIPLE      rgbColor;           ///< fiber solid color
    qint32          nSelLenStart;       ///< the start-point of the selected fiber
    qint32          nSelLenEnd;         ///< the end-point of the selected fiber
};

struct FiberFileHeader
{
    char        sFiberFileTag[8];       ///< file tag = FiberDat
    qint32      nFiberNr;               ///< total number of fibers
    qint32      nFiberLenMax;           ///< max-length of fibers
    float       fFiberLenMean;          ///< mean-length of fibers
    quint32     nImgWidth;              ///< image width
    quint32     nImgHeight;             ///< image height
    quint32     nImgSlices;             ///< number of slices
    float       fPixelSizeWidth;        ///< width voxel size
    float       fPixelSizeHeight;       ///< height voxel size
    float       fSliceThickness;        ///< slice thickness
    uchar       enumSliceOrientation;   ///< slice orientation
    uchar       enumSliceSequencing;    ///< slice sequencing
    //        char sVersion[8];                // version number
};

class FiberTracts
{
    // data, note that we can not use std::vector since
    // there are no direct memory pointer to the data
    // it is only present in C++11
    QList < Polyline >              fXYZChain;
    QList <QColor>                  fColors;
    fvec                            fLength;
    FiberFileHeader                 ffh;

public:
    FiberTracts();
    ~FiberTracts() {this->clear();}


    // access
    inline void append(const Polyline &f, const QColor &c,float l)
    {
        fXYZChain.append(f);
        fColors.append(c);
        fLength.resize(fLength.n_elem+1,1);
        fLength[fLength.n_elem-1] = l;
        ffh.nFiberNr++;
    }
    inline void remove(int index)
    {
        if (isEmpty() || index >= size())
            return;
        fXYZChain.removeAt(index);
        fColors.removeAt(index);
        fLength.shed_row(index);
    }

    // two very important notes here:
    // 1) access functions should be constant since they will not modify the data
    // 2) access functions should be inline for speed, so they must be implemented in the header
    // It is up to the user to be sure that the indeces used are in range
    inline Polyline getFiber(int index) const { return fXYZChain.at(index); }
    inline int size() const {return ffh.nFiberNr;}
    inline bool isEmpty() const {return ffh.nFiberNr == 0;}

    inline float getLength(int index) const { return fLength.at(index); }
    inline float minLength() const { return fLength.min();}
    inline float maxLength() const { return fLength.max();}
    inline float meanLength() const { return arma::mean(fLength); }

    inline QColor getColor(int index) const { return fColors.at(index); }
    inline void setColor(const QColor &c,int index) { fColors[index] = c; }

    inline void setDataDimension (quint32 rows,quint32 cols, quint32 slices)
    { ffh.nImgWidth = rows; ffh.nImgHeight = cols; ffh.nImgSlices = slices; }
    inline void setVoxelDimension (float dx,float dy, float dz)
    { ffh.fPixelSizeWidth = dx; ffh.fPixelSizeHeight = dy; ffh.fSliceThickness = dz; }
    inline void getDataDimension (quint32 &rows,quint32 &cols, quint32 &slices) const
    { rows = ffh.nImgWidth; cols = ffh.nImgHeight; slices = ffh.nImgSlices; }
    inline void getVoxelDimension (float &dx,float &dy,float &dz) const
    { dx = ffh.fPixelSizeWidth; dy = ffh.fPixelSizeHeight; dz = ffh.fSliceThickness; }

    // file access functions
    bool readFiber(const QString &fileName,const DiffusionModelDimension &dmp,
                   MyProgressDialog *progress, bool append);
    bool writeFiber(const QString &fileName,const QList<quint32> &saveIdx,
                    MyProgressDialog *progress);
    // Manipulation
    void filterFibersByLength(QList<quint32> &list,int minL,int maxL);
    void clear();
};

#endif // FIBERTRACTS_H
