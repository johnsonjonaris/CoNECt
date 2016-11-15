#ifndef MRIIMAGE_H
#define MRIIMAGE_H

#include <QtCore>
#include <QMessageBox>
#include "UI\myprogressdialog.h"
#include <QFileInfo>
#include "armadillo"
#include "datatypes.h"
using namespace arma;
using namespace std;

/**
 * \struct Analyze Header Key
 * sizeof_header Must indicate the byte size of the header file
 * extents Should be 16384, the image file is created as contiguous with a minimum extent size
 * regular Must be ‘r’ to indicate that all images and volumes are the same size
*/
struct HeaderKey               /* header key */
{                               /* off + size */
    int sizeof_hdr;             /* 0 + 4 */
    char hdr_data_type[10];     /* 4 + 10 */
    char db_name[18];           /* 14 + 18 */
    int extents;                /* 32 + 4 */
    short int session_error;    /* 36 + 2 */
    char regular;               /* 38 + 1 */
    char hkey_un0;              /* 39 + 1 */
}; /* total=40 bytes */

struct ImageDimension
{
    short int dim[8];   /**< Array of the image dimensions
                         * dim[0]        Number of dimensions in database; usually 4
                         * dim[1]        Image X dimension; number of pixels in an image row
                         * dim[2]        Image Y dimension; number of pixel rows in slice
                         * dim[3]        Volume Z dimension; number of slices in a volume
                         * dim[4]        Time points, number of volumes in a database
                         */
    short int unused8;
    short int unused9;
    short int unused10;
    short int unused11;
    short int unused12;
    short int unused13;
    short int unused14;
    short int datatype;     ///< ImageDataType type for this image set
    short int bitpix;       ///< number of bits per pixel: 1, 8, 16, 32, or 64.
    short int dim_un0;
    float pixdim[8];        /**< real world measurements in mm. and ms.
                             * pixdim[1]     voxel width in mm.
                             * pixdim[2]     voxel height in mm.
                             * pixdim[3]     slice thickness in mm.
                             */
    float vox_offset;       /**< byte offset in the .img file at which voxels start.
                             * This value can not be negative to specify that the
                             * absolute value is applied for every image in the file. */
    float funused1;
    float funused2;
    float funused3;
    float cal_max,cal_min;  ///< specify the range of calibration values
    float compressed;
    float verified;
    int glmax,glmin;        ///< The max and min pixel values for the entire database.
}; /*total = 108 bytes*/

struct DataHistory
{
    //orient:

    char descrip[80];               /* 0 + 80 */
    char aux_file[24];              /* 80 + 24 */
    char orient;                    /** slice orientation for this dataset.
                                      * 0 transverse unflipped
                                      * 1 coronal unflipped
                                      * 2 sagittal unflipped
                                      * 3 transverse flipped
                                      * 4 coronal flipped
                                      * 5 sagittal flipped */
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

template<typename T>
Cube<T> swapCube(const Cube<T> &in, uchar direction);

template<typename T>
Cube<T> mirrorCube(const Cube<T> &in, uchar direction);

bool readHeader(QString headerFileName, AnalyzeHeader& header, ImageFileType fileType);

bool checkImageFile(QString imgFileName, uint fileType, const AnalyzeHeader &header);

// read image function
template<typename T>
void orientCubes(const QVector<uchar> &orientation, MyProgressDialog *progress,
                 AnalyzeHeader &header, QList< Cube<T> > &data);

template void orientCubes(const QVector<uchar> &o, MyProgressDialog *p, AnalyzeHeader &h, QList< uchar_cube > &d);
template void orientCubes(const QVector<uchar> &o, MyProgressDialog *p, AnalyzeHeader &h, QList< s16_cube > &d);
template void orientCubes(const QVector<uchar> &o, MyProgressDialog *p, AnalyzeHeader &h, QList< s32_cube > &d);
template void orientCubes(const QVector<uchar> &o, MyProgressDialog *p, AnalyzeHeader &h, QList< fcube > &d);
template void orientCubes(const QVector<uchar> &o, MyProgressDialog *p, AnalyzeHeader &h, QList< cube > &d);

// read image function
template<typename T>
bool readImage(QString imgFileName,uint fileType, bool preview,MyProgressDialog *progress,
                      AnalyzeHeader &header,QList< Cube<T> > &out);

template bool readImage(QString im, uint ft, bool pv, MyProgressDialog *p, AnalyzeHeader &h, QList< uchar_cube > &out);
template bool readImage(QString im, uint ft, bool pv, MyProgressDialog *p, AnalyzeHeader &h, QList< s16_cube > &out);
template bool readImage(QString im, uint ft, bool pv, MyProgressDialog *p, AnalyzeHeader &h, QList< s32_cube > &out);
template bool readImage(QString im, uint ft, bool pv, MyProgressDialog *p, AnalyzeHeader &h, QList< fcube > &out);
template bool readImage(QString im, uint ft, bool pv, MyProgressDialog *p, AnalyzeHeader &h, QList< cube > &out);

// write header function
bool writeHeader(QString headerFileName, AnalyzeHeader header);

// write image function
template <typename T>
bool writeImage(QString imgfileName, uint fileType,
                AnalyzeHeader header, QList< Cube<T> > &data);

// main function to write a file
template<typename T>
bool saveImages(QString fileName,uint fileType,
                AnalyzeHeader header,QList< Cube<T> > &data);

// to prevent implementation in the header file, compiler needs an instance of every type:
// http://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file/495056#495056
template bool saveImages(QString fn, uint ft, AnalyzeHeader h, QList< uchar_cube > &d);
template bool saveImages(QString fn, uint ft, AnalyzeHeader h, QList< s16_cube > &d);
template bool saveImages(QString fn, uint ft, AnalyzeHeader h, QList< s32_cube > &d);
template bool saveImages(QString fn, uint ft, AnalyzeHeader h, QList< fcube > &d);
template bool saveImages(QString fn, uint ft, AnalyzeHeader h, QList< cube > &d);


// prepare header
AnalyzeHeader prepareHeader(quint32 dim[4], ImageDataType dataType,
                            float pixDim[3], ImageFileType fileType);

DiffusionModelDimension getParameters(const AnalyzeHeader &header);

#endif // MRIIMAGE_H
