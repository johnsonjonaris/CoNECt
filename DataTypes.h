#ifndef DATATYPES_H
#define DATATYPES_H

#include <QtCore>


enum ROIShape               { FREEHAND, OVAL, RECT };
enum UserAction             { ROI_ACTION, VOI_ACTION};
enum ROIType                { AND_ROI, NOT_ROI };
enum DiffusionModelType     { DTI=0,TDF=1,ODF,SH_QBALL,EMPTY};
enum FiberColoringMethod    { DIRECTIONAL = 0,SOLID = 1, SCALAR = 2};
enum FiberFileType          { CONECT = 0, DTISTUDIO = 1};

/* Acceptable values for datatype */
enum ImageDataType{
    DT_NONE             = 0,
    DT_UNKNOWN          = 0,
    DT_BINARY           = 1,    // 1 bit per voxel
    DT_UNSIGNED_CHAR    = 2,    // 1B per voxel
    DT_SIGNED_SHORT     = 4,    // 2B per voxel
    DT_SIGNED_INT       = 8,    // 4B per voxel
    DT_FLOAT            = 16,   // 4B per voxel
    DT_COMPLEX          = 32,   // 8B per voxel (2 float) - not yet supported
    DT_DOUBLE           = 64,   // 8B per voxel
    DT_RGB              = 128,  // 3B per voxel - not yet supported
    DT_ALL              = 255,
    DT_INT8             = 256,  // Signed char (1B per voxel)
    DT_RGB96            = 511,  // Single (float) RGB (4B X 3 per voxel)
    DT_UNINT16          = 512,  // Unsigned short integer (2B per voxel)
    DT_UNINT32          = 768,  // Unsigned integer (4B per voxel)
    DT_INT64            = 1024, // Signed long long (8B per voxel)
    DT_UINT64           = 1280  // Unsigned long long (8B per voxel)
};
enum ImageFileType{ DICOM = 0, ANALYZE = 1, NIFTI = 2, RAW = 3 };
enum ImageCorrectionAction{ SWAP_XY = 0,SWAP_XZ = 1,SWAP_YZ = 2,
                            FLIP_X = 3, FLIP_Y = 4, FLIP_Z = 5 };
enum ImageOrientation{ CORONAL = 0,SAGITTAL = 1,AXIAL = 2 };
enum {NEGATIVE_Seq = 1};
// tracking structure
struct TrackingParameters
{
    // common to all types
    double maxBendingAngle;     // maximum bending angle in degree
    float propStep;             // propagation step size
    float minFiberLength;       // min fiber length
    bool smooth,                // do smoothing
         useLimitingMetric;     // use limiting metric
    quint8 smoothingQual;       // smoothing quality
    float limitingMetricStart,
          limitingMetricStop;   // start and stop thresholds in case a metric was chosen
    QColor fiberColor;          // fiber color
    // for probabilistic tracking
    quint8 tractsPerSeed;       // tracts per seed
};
struct DiffModelDimensionParam
{
    quint32 nRows, nCols, nSlices;
    float dx, dy, dz;
};

// Analyze Header
struct HeaderKey               /* header key */
{                               /* off + size */
    int sizeof_hdr;             /* 0 + 4 */
    char hdr_data_type[10];     /* 4 + 10 */
    char db_name[18];           /* 14 + 18 */
    int extents;                /* 32 + 4 */
    short int session_error;    /* 36 + 2 */
    char regular;               /* 38 + 1 */
    char hkey_un0;              /* 39 + 1 */
    // sizeof_header Must indicate the byte size of the header file
    // extents Should be 16384, the image file is created as contiguous with a minimum extent size
    // regular Must be ‘r’ to indicate that all images and volumes are the same size
}; /* total=40 bytes */
struct ImageDimension
{
    /*
     dim[]; Array of the image dimensions
     dim[0] Number of dimensions in database; usually 4
     dim[1] Image X dimension; number of pixels in an image row
     dim[2] Image Y dimension; number of pixel rows in slice
     dim[3] Volume Z dimension; number of slices in a volume
     dim[4] Time points, number of volumes in a database
     vox_units[4] specifies the spatial units of measure for a voxel
     cal_units[4] specifies the name of the calibration unit
     datatype datatype for this image set
         DT_UNKNOWN         0       Unknown data type
         DT_BINARY          1       Binary (1 bit per voxel)
         DT_UNSIGNED_CHAR   2       Unsigned character (8 bits per voxel)
         DT_SIGNED_SHORT    4       Signed short (16 bits per voxel)
         DT_SIGNED_INT      8       Signed integer (32 bits per voxel)
         DT_FLOAT           16      Floating point (32 bits per voxel)
         DT_COMPLEX         32      Complex (2 float) (64 bits per voxel)
         DT_DOUBLE          64      Double precision (64 bits per voxel)
         DT_RGB24           128     Unsigned character RGB (8 bits X 3 per voxel)
         DT_ALL             255
         DT_INT8            256     Signed char (8 bits per voxel)
         DT_RGB96           511     Single (float) RGB (32 bits X 3 per voxel)
         DT_UNINT16         512     Unsigned short integer (16 bits per voxel)
         DT_UNINT32         768     Unsigned integer (32 bits per voxel)
         DT_INT64           1024    Signed long long (64 bits per voxel)
         DT_UINT64          1280    Unsigned long long (64 bits per voxel)

     bitpix; number of bits per pixel; 1, 8, 16, 32, or 64.
     dim_un0; unused
     pixdim[]; Parallel array to dim[], giving real world measurements in mm. and ms.
     pixdim[1]; voxel width in mm.
     pixdim[2]; voxel height in mm.
     pixdim[3]; slice thickness in mm.
     vox_offset; byte offset in the .img file at which voxels start. This value can negative
     to specify that the absolute value is applied for every image in the file.
     calibrated Max, Min specify the range of calibration values
     glmax, glmin; The maximum and minimum pixel values for the entire database.
    */
    short int dim[8];               /* 0 + 16 */
    short int unused8;              /* 16 + 2 */
    short int unused9;              /* 18 + 2 */
    short int unused10;             /* 20 + 2 */
    short int unused11;             /* 22 + 2 */
    short int unused12;             /* 24 + 2 */
    short int unused13;             /* 26 + 2 */
    short int unused14;             /* 28 + 2 */
    short int datatype;             /* 30 + 2 */
    short int bitpix;               /* 32 + 2 */
    short int dim_un0;              /* 34 + 2 */
    float pixdim[8];                /* 36 + 2 */
    float vox_offset;               /* 68 + 4 */
    float funused1;                 /* 72 + 4 */
    float funused2;                 /* 76 + 4 */
    float funused3;                 /* 80 + 4 */
    float cal_max;                  /* 84 + 4 */
    float cal_min;                  /* 88 + 4 */
    float compressed;               /* 92 + 4 */
    float verified;                 /* 96 + 4 */
    int glmax,glmin;                /* 100 + 8 */
};    /*total = 108 bytes*/
struct DataHistory
{
    //orient: slice orientation for this dataset.
    //          0 transverse unflipped
    //          1 coronal unflipped
    //          2 sagittal unflipped
    //          3 transverse flipped
    //          4 coronal flipped
    //          5 sagittal flipped

    char descrip[80];               /* 0 + 80 */
    char aux_file[24];              /* 80 + 24 */
    char orient;                    /* 104 + 1 */
    char originator[10];            /* 105 + 10 */
    char generated[10];             /* 115 + 10 */
    char scannum[10];               /* 125 + 10 */
    char patient_id[10];            /* 135 + 10 */
    char exp_date[10];              /* 145 + 10 */
    char exp_time[10];              /* 155 + 10 */
    char his_un0[3];                /* 165 + 3 */
    int views;                      /* 168 + 4 */
    int vols_added;                 /* 172 + 4 */
    int start_field;                /* 176 + 4 */
    int field_skip;                 /* 180 + 4 */
    int omax, omin;                 /* 184 + 8 */
    int smax, smin;                 /* 192 + 8 */
};
struct AnalyzeHeader
{
    struct HeaderKey hk;           /* 0 + 40 */
    struct ImageDimension dime;    /* 40 + 108 */
    struct DataHistory hist;       /* 148 + 200 */
};
/* total=348 bytes*/

// define DTIStudio fiber header
// define fiber header, RGB and XYZ structures
struct  RGB_TRIPLE{quint8 r, g, b;};
struct  TagXYZ_TRIPLE{float x, y, z;};
struct  FiberHeader
{
    qint32          nLength;            // fiber length;
    qint8           nSelStatus;
    RGB_TRIPLE      rgbColor;           // fiber solid color
    qint32          nSelLenStart;       // the start-point of the selected fiber
    qint32          nSelLenEnd;         // the end-point of the selected fiber
};
struct FiberFileHeader
{
    char        sFiberFileTag[8];       // file tag = FiberDat
    qint32      nFiberNr;               // total number of fibers
    qint32      nFiberLenMax;           // max-length of fibers
    float       fFiberLenMean;          // mean-length of fibers
    quint32     nImgWidth;              // image dimension
    quint32     nImgHeight;
    quint32     nImgSlices;
    float       fPixelSizeWidth;        // voxel size
    float       fPixelSizeHeight;
    float       fSliceThickness;
    uchar       enumSliceOrientation;   // slice orientation
    uchar       enumSliceSequencing;    // slice sequencing
    //        char sVersion[8];                // version number
};

// connectome stuff
enum SizeType {EQUAL, WEIGHT};
enum ColorType {SAME, MODULAR, THRESHOLD};
enum DisplayType {ALL, NONE, THRESHOLDS};
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
