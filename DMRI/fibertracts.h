#ifndef FIBERTRACTS_H
#define FIBERTRACTS_H

#include <QtGui>
#include "UI\myprogressdialog.h"
#include "armadillo"
#include "datatypes.h"

using namespace arma;   // don't forget it VIP

class FiberTracts
{
    // data, note that we can not use std::vector since
    // there are no direct memory pointer to the data
    // it is only present in C++11
    QList < QVector<QVector3D> >    fXYZChain;
    QList <QColor>                  fColors;
    fvec                            fLength;
    FiberFileHeader                 ffh;

public:
    FiberTracts();
    ~FiberTracts() {this->clear();}


    // access
    inline void append(const QVector<QVector3D> &f, const QColor &c,float l)
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
    inline QVector<QVector3D> getFiber(int index) const { return fXYZChain.at(index); }
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
