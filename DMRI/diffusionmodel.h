#ifndef DIFFUSIONMODEL_H
#define DIFFUSIONMODEL_H

#include <QtGui>
#include <QtCore>
#include <QMessageBox>
#include <QDebug>

#include "armadillo"
#include "datatypes.h"
#include "UI\myprogressdialog.h"
#include "Algorithms\mtrand.h"
#include "Algorithms\bspline_alg.h"
#include "fibertracts.h"
#include "mriimage.h"
#include "UI\openVol.h"
#include "UI\dtidialog.h"

#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

using namespace arma;

/// diffusion model type
enum DiffusionModelType
{
    DTI=0,                  ///< diffusion tensor imaging
    TDF=1,                  ///< tensor distribution function
    ODF,                    ///< orientation distribution function
    SH_QBALL,               ///< spherical harmonics
    EMPTY                   ///< empty model
};

enum Algorithm          ///< tracking algorithm to use
{
    FACT = 0,           ///< FACT tracking algorithm by Mori S and co.
    TOD_TRACT = 2       ///< TDF-based probabilistic tracking
};

class DiffusionModel
{
    float longestFiber;                     ///< longest possible fiber value (mm)

public:
    DiffusionModel();

    // common parameters for all models
    quint32     nRows,nCols,nSlices,        ///< dimension of the dataset
                nVox;                       ///< total number of voxels
    float       dx,dy,dz;                   ///< voxel dimension
    uchar_cube  Mask;                       ///< a binary mask of relevant voxels
    DiffusionModelType       modelType;     ///< model type: TOD, DTI ...


    // TOD specific parameters
    // TODO: define all TOD params in a struct
    float       th;                         ///< TOD threshold
    mat         q,originalQ;                ///< angular distribution table
    uchar_vec   qIndex;                     ///< voxelwise index corresponding
                                            ///< to the angles in the q array per tensor
                                            ///< 4D array
    vec         TOD;                        ///< the tensor orientation distribution
                                            ///< PDF (voxelwise), 4D array
    cube        b0;                         ///< the B0 for display
    uchar_cube  nTensors,                   ///< voxelwise number of tensors
                Rx,Ry,Rz;                   ///< voxelwise mean angular direction vector
    quint32     maxNTens;                   ///< max number of tensors

    // DTI specific parameters
    fcube       eVecPx,eVecPy,eVecPz,       ///< principal eigen vector
                FA;                         ///< metrics


    // SH-QBALL specific parameters


    // ODF specific parameters


    /// read TOD diffusion model from a file
    bool readTOD(const QString &fileName, MyProgressDialog *progress);
    /// write TOD diffusion model to a file
    bool writeTOD(){}
    /// read DTI diffusion model file
    static bool readDTI(const QString &fileName,
                        QList<fcube> &ADC,
                        AnalyzeHeader &header,
                        MyProgressDialog *progress);
    /// read DTI diffusion model file
    bool readDTI(const QString &fileName,
                 QList<fcube> &ADC,
                 QList<fcube> &metrics,
                 MyProgressDialog *progress);
    /// set DTI diffusion model file
    void setDTI(const QList<fcube> &ADC,
                QList<fcube> &metrics,
                const AnalyzeHeader &header,
                MyProgressDialog *progress);
    /// write DTI diffusion model file - not implemented yet
    bool writeDTI(){}
    /// read diffusion weighted images
    static bool readDWI(const QString &fileName,
                        ImageFileType fileType,
                        QList<s16_cube> &dwi,
                        AnalyzeHeader &header,
                        MyProgressDialog *progress);

    /// compute DTI model
    /**
     * @brief computeDTIModel compute the diffusion tensor model
     * @param dwi diffusion weighted images
     * @param gTable gradient table
     * @param bValue b-value
     * @param nLevel noise level
     * @param ADC output: apparent diffusion coefficient
     * @param progress progress bar
     * @return true if succeeded
     */
    static bool computeDTIModel(QList<s16_cube> &dwi,
                                mat gTable,
                                float bValue,
                                int nLevel,
                                QList<fcube> &ADC,
                                MyProgressDialog *progress);
    /// compute DTI-base metrics: 3 eigen values, MD, FA, RA, VR, 3 eigen vectors
    static void computeDTIStuff(const QList<fcube> &ADC,
                                QList<fcube> &metrics,
                                MyProgressDialog *progress);

    /// return the model type
    inline DiffusionModelType type() const {return modelType;}
    /// clear the model data
    void clear();
    /// test if class is not init with any diffusion model
    inline bool isEmpty() {return (modelType == EMPTY);}

    // fiber tracking
    void ROItract2waysDet() {}
    /// all brain probabilistic TOD fiber tracking
    bool probablistic_TOD_Tracking(TrackingParameters tp,
                                   FiberTracts &fibers,
                                   MyProgressDialog *progress);
    /// all brain deterministic DTI fiber tracking
    bool FACT_DTI(TrackingParameters tp, FiberTracts &fibers, MyProgressDialog *progress);

private:


    /// create a fiber tract based on TOD probabilistic method
    void createProbabilistic_TOD_Tract(QVector3D seed,
                                       QVector3D propDir,
                                       const TrackingParameters &tp,
                                       Polyline &fiber);
    /// return propagation vector based on TOD propabilistic method
    QVector3D getProbablistic_TOD_PropagationVector(vec tod,
                                                    int nTen,
                                                    uchar_vec qIdx,
                                                    QVector3D oldPropDir,
                                                    double maxBendingAngle);
    /// create a fiber tract based on FACT tracking method
    void createFACT_DTI_Tract(QVector3D seed,
                              QVector3D propDir,
                              const TrackingParameters &tp,
                              Polyline &fiber);
    /// smooth fiber tract using a b-spline
    Polyline smoothFiber(Polyline &fiber, int smoothingQual);
};
#endif // DIFFUSIONMODEL_H
