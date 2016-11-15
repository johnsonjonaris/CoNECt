#ifndef DATATYPES_H
#define DATATYPES_H

#include <QtCore>

typedef QVector<QVector3D> Polyline;

/// selected region of interest (ROI) shape
enum ROIShape
{
    FREEHAND,               ///< freehand ROI
    OVAL,                   ///< oval shape ROI
    RECT                    ///< rectangular shape ROI
};

/// selected user action type
enum UserAction
{
    ROI_ACTION,             ///< Regoin of interest action
    VOI_ACTION              ///< Volume of interest action
};
/// selected user ROI effect
enum ROIType
{
    AND_ROI,                ///< future ROI has an AND effect with previous ROI(s)
    NOT_ROI                 ///< future ROI has an exclusion effect with previous ROI(s)
};
/// diffusion model type
enum DiffusionModelType
{
    DTI=0,                  ///< diffusion tensor imaging
    TDF=1,                  ///< tensor distribution function
    ODF,                    ///< orientation distribution function
    SH_QBALL,               ///< spherical harmonics
    EMPTY                   ///< empty model
};
/// fiber tracts coloring method
enum FiberColoringMethod
{
    DIRECTIONAL = 0,        ///< direction dependent
    SOLID = 1,              ///< solid color
    SCALAR = 2              ///< weighted by a scalar
};
/// fiber tract file type
enum FiberFileType
{
    CONECT = 0,             ///< CoNECt file type
    DTISTUDIO = 1           ///< DTI Studio
};

/**
  * \enum ImageDataType
  * Acceptable datatype values for images
  */
enum ImageDataType{
    DT_NONE             = 0,    ///< NONE
    DT_UNKNOWN          = 0,    ///< unknown type
    DT_BINARY           = 1,    ///< Binary (1 bit per voxel)
    DT_UNSIGNED_CHAR    = 2,    ///< Unsigned character (1B per voxel)
    DT_SIGNED_SHORT     = 4,    ///< Signed short (2B per voxel)
    DT_SIGNED_INT       = 8,    ///< Signed integer (4B per voxel)
    DT_FLOAT            = 16,   ///< Floating point (4B per voxel)
    DT_COMPLEX          = 32,   ///< Complex (2 float) (8B per voxel) - not yet supported
    DT_DOUBLE           = 64,   ///< Double precision (8B per voxel)
    DT_RGB              = 128,  ///< Unsigned character RGB (1B X 3 per voxel) - not yet supported
    DT_ALL              = 255,
    DT_INT8             = 256,  ///< Signed char (1B per voxel)
    DT_RGB96            = 511,  ///< Single (float) RGB (4B X 3 per voxel)
    DT_UNINT16          = 512,  ///< Unsigned short integer (2B per voxel)
    DT_UNINT32          = 768,  ///< Unsigned integer (4B per voxel)
    DT_INT64            = 1024, ///< Signed long long (8B per voxel)
    DT_UINT64           = 1280  ///< Unsigned long long (8B per voxel)
};

/**
  * \enum ImageFileType
  * acceptable image file types
  */
enum ImageFileType{ DICOM = 0, ANALYZE = 1, NIFTI = 2, RAW = 3 };
enum ImageCorrectionAction{ SWAP_XY = 0,SWAP_XZ = 1,SWAP_YZ = 2,
                            FLIP_X = 3, FLIP_Y = 4, FLIP_Z = 5 };
enum ImageOrientation{ CORONAL = 0,SAGITTAL = 1,AXIAL = 2 };
enum {NEGATIVE_Seq = 1};
// tracking structure
struct TrackingParameters
{
    // common to all types
    double maxBendingAngle;     ///< maximum bending angle in degree
    float propStep;             ///< propagation step size
    float minFiberLength;       ///< min fiber length
    bool smooth,                ///< do smoothing
         useLimitingMetric;     ///< use limiting metric
    quint8 smoothingQual;       ///< smoothing quality
    float limitingMetricStart,
          limitingMetricStop;   ///< start and stop thresholds in case a metric was chosen
    QColor fiberColor;          ///< fiber color

    quint8 tractsPerSeed;       ///< tracts per seed (for probabilistic tracking only)
};

struct DiffusionModelDimension
{
    quint32 nRows, nCols, nSlices;
    float dx, dy, dz;
};




// connectome stuff
enum SizeType {EQUAL, WEIGHT};
enum ColorType {SAME, MODULAR, THRESHOLD};
enum DisplayType {ALL, NONE, THRESHOLDS};

/// visualization parameters, compatible for node or edge
struct VisualizationParameters
{
    // Display
    DisplayType         dType;
    float               dThreshold;
    uint                dThresholdIdx;
    // size
    SizeType            sType;
    float               size;
    uint                sWeightIdx;
    float               sScale;
    // color
    ColorType           cType;
    QVector<QColor>     cList;
    uint                cModularIdx;
    float               cThreshold;
    uint                cThresholdIdx;
};

#endif // DATATYPES_H
