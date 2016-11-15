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

/// fiber tracts coloring method
enum FiberColoringMethod
{
    DIRECTIONAL = 0,        ///< direction dependent
    SOLID = 1,              ///< solid color
    SCALAR = 2              ///< weighted by a scalar
};

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
