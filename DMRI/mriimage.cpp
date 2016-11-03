#include "mriimage.h"

bool readHeader(QString headerFileName,AnalyzeHeader& header,ImageFileType fileType)
{
    if (headerFileName.isEmpty())
        return false;
    QFile file(headerFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0, "Error", QString("Cannot read header file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    if (file.size() != 348 && fileType == ANALYZE) {
        QMessageBox::warning(0, "Error", "Bad header file size.");
        file.close();
        return false;
    }

    file.read( (char *) &header, sizeof(header) );
    file.close();

    if (( header.hk.regular != 'r')) {
        QMessageBox::warning(0, "Error", "Improper Image file.");
        return false;
    }
    if ((header.dime.datatype == DT_NONE) ||
            (header.dime.datatype == DT_COMPLEX) ||
            (header.dime.datatype == DT_RGB) ){
        QMessageBox::warning(0, "Error", "Unsupported data type.");
        return false;
    }
    return true;
}

/*************************************************************************************************/

// write header function
bool writeHeader(QString headerFileName, AnalyzeHeader header)
{
    if (headerFileName.isEmpty())
        return false;
    QFile file(headerFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(0, "Warning!", QString("Cannot write header file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    // correct header information
    swap(header.dime.dim[1],header.dime.dim[2]);
    swap(header.dime.pixdim[1],header.dime.pixdim[2]);
    file.write( (char *) &header, sizeof(AnalyzeHeader) );
    file.close();
    return true;
}

// prepare header
AnalyzeHeader prepareHeader(quint32 dim[4],ImageDataType dataType,
                            float pixDim[3],ImageFileType fileType)
{
    AnalyzeHeader header;
    // header key
    header.hk.sizeof_hdr         = 348;
    header.hk.extents            = (fileType == ANALYZE)?16384:0;
    header.hk.session_error      = 0;
    header.hk.regular            = 'r';
    // image dimension
    header.dime.dim[0]           = 4;
    header.dime.dim[1]           = dim[0];
    header.dime.dim[2]           = dim[1];
    header.dime.dim[3]           = dim[2];
    header.dime.dim[4]           = dim[3];
    header.dime.dim[5]           = 0;
    header.dime.dim[6]           = 0;
    header.dime.dim[7]           = 0;
    header.dime.unused8          = 0;
    header.dime.unused9          = 0;
    header.dime.unused10         = 0;
    header.dime.unused11         = 0;
    header.dime.unused12         = 0;
    header.dime.unused13         = 0;
    header.dime.unused14         = 0;
    header.dime.dim_un0          = 0;
    header.dime.pixdim[0]        = 4;
    header.dime.pixdim[1]        = pixDim[0];
    header.dime.pixdim[2]        = pixDim[1];
    header.dime.pixdim[3]        = pixDim[2];
    header.dime.pixdim[4]        = 1;
    header.dime.pixdim[5]        = 0;
    header.dime.pixdim[6]        = 0;
    header.dime.pixdim[7]        = 0;
    header.dime.vox_offset       = (fileType == ANALYZE)?0:352;
    header.dime.datatype         = dataType;
    header.dime.funused1         =0.0;
    header.dime.funused2         =0.0;
    header.dime.funused3         =0.0;
    header.dime.cal_max          = 0;
    header.dime.cal_min          = 0;
    header.dime.compressed       = 0;
    header.dime.verified         = 0;
    header.dime.glmin            = INT_MAX;
    header.dime.glmax            = INT_MIN;

    if (dataType == DT_BINARY) {
        header.dime.datatype     = DT_UNSIGNED_CHAR;
        header.dime.bitpix       = 8;
    }
    else if (dataType == DT_UNSIGNED_CHAR)
        header.dime.bitpix       = 8;
    else if (dataType == DT_SIGNED_SHORT)
        header.dime.bitpix       = 16;
    else if (dataType == DT_SIGNED_INT || dataType == DT_FLOAT)
        header.dime.bitpix = 32;
    else if (dataType == DT_DOUBLE)
        header.dime.bitpix       = 64;

    // history
    header.hist.orient           = 0;
    // strcpy_s(header.hist.originator,"CoNECt");
    header.hist.views            = 0;
    header.hist.vols_added       = 0;
    header.hist.start_field      = 0;
    header.hist.field_skip       = 0;
    header.hist.omax             = 0;
    header.hist.omin             = 0;
    header.hist.smax             = 0;
    header.hist.smin             = 0;

    return header;
}

DiffusionModelDimension getParameters(const AnalyzeHeader &header)
{
    DiffusionModelDimension dmp;
    dmp.nRows = header.dime.dim[1];
    dmp.nCols = header.dime.dim[2];
    dmp.nSlices = header.dime.dim[3];
    dmp.dx = header.dime.pixdim[1];
    dmp.dy = header.dime.pixdim[2];
    dmp.dz = header.dime.pixdim[3];
    return dmp;
}
