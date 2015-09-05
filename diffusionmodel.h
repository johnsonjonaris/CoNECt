#ifndef DIFFUSIONMODEL_H
#define DIFFUSIONMODEL_H

#include <QtGui>
#include <QtCore>
#include <QMessageBox>
#include <QDebug>

#include "armadillo"
#include "DataTypes.h"
#include "myprogressdialog.h"
#include "mtrand.h"
#include "bspline_alg.h"
#include "fibertracts.h"
#include "mriimage.h"
#include "openVol.h"
#include "dtidialog.h"

#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

using namespace arma;   // don't forget it VIP

class DiffusionModel
{
public:
    DiffusionModel();

    // common parameters for all models
    quint32     nRows,nCols,nSlices,        // dimension of the dataset
                nVox;
    float       dx,dy,dz;                   // voxel dimension
    uchar_cube  Mask;                       // a binary mask
    DiffusionModelType       modelType;


    // TOD specific parameters
    float       th;                         // threshold
    mat         q,originalQ;                // angular distribution table (3X108)
    uchar_vec   qIndex;                     // voxelwise index corresponding
                                            // to the angles in the q array per tensor
                                            // 4D array
    vec         TOD;                        // the tensor orientation distribution
                                            // PDF (voxelwise), 4D array
    cube        b0;                         // the B0 for display
    uchar_cube  nTensors,                   // voxelwise number of tensors
                Rx,Ry,Rz;                   // voxelwise mean angular direction vector
    quint32     maxNTens;                   // max number of tensors

    // DTI specific parameters
    fcube       eVecPx,eVecPy,eVecPz,       // principal eigen vector
                FA;                         // metrics


    // SH-QBALL specific parameters


    // ODF specific parameters


    // file access functions
    bool readTOD(const QString &fileName,MyProgressDialog *progress);
    bool writeTOD(){}

    static bool readDTI(const QString &fileName, QList<fcube> &ADC,
                        AnalyzeHeader &header, MyProgressDialog *progress);
    bool readDTI(const QString &fileName, QList<fcube> &ADC,
                 QList<fcube> &metrics, MyProgressDialog *progress);
    void setDTI(const QList<fcube> &ADC,QList<fcube> &metrics,
                const AnalyzeHeader &header, MyProgressDialog *progress);

    bool writeDTI(){}

    static bool readDWI(const QString &fileName, ImageFileType fileType,
                        QList<s16_cube> &dwi, AnalyzeHeader &header,
                        MyProgressDialog *progress);

    // compute models
    static bool computeDTIModel(QList<s16_cube> &dwi, mat gTable,
                                float bValue, int nLevel, QList<fcube> &ADC,
                                MyProgressDialog *progress);
    static void computeDTIStuff(const QList<fcube> &ADC, QList<fcube> &metrics,
                                MyProgressDialog *progress);

    // model type
    inline DiffusionModelType type() const {return modelType;}
    // clear the model data
    void clear();
    inline bool isEmpty() {return (modelType == EMPTY);}

    // fiber tracking
    void ROItract2waysDet() {}
    bool probablistic_TOD_Tracking(TrackingParameters tp, FiberTracts &fibers,
                                   MyProgressDialog *progress);
    bool FACT_DTI(TrackingParameters tp, FiberTracts &fibers, MyProgressDialog *progress);

private:

    float longestFiber;

    void createProbabilistic_TOD_Tract(QVector3D seed,QVector3D propDir,
                                       const TrackingParameters &tp,
                                       QVector<QVector3D> &fiber);
    QVector3D getProbablistic_TOD_PropagationVector(vec tod,int nTen,uchar_vec qIdx,
                                                    QVector3D oldPropDir,
                                                    double maxBendingAngle);
    void createFACT_DTI_Tract(QVector3D seed, QVector3D propDir,
                              const TrackingParameters &tp,
                              QVector<QVector3D> &fiber);

    QVector<QVector3D> smoothFiber(QVector<QVector3D> &fiber, int smoothingQual);
};
#endif // DIFFUSIONMODEL_H
