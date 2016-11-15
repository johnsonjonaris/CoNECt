#include "mriimage.h"

template<typename T>
Cube<T> swapCube(const Cube<T> &in, uchar direction)
{
    Cube<T> out = Cube<T>();
    if (direction == SWAP_XY) {                 // swap XY
        out = Cube<T>(in.n_cols,in.n_rows,in.n_slices);
        for (uint i =0; i< in.n_slices;i++)
            out.slice(i) = in.slice(i).t();
    }
    else if (direction == SWAP_XZ) {            // swap XZ
        out = Cube<T>(in.n_slices,in.n_cols,in.n_rows);
        Mat<T> tmp = Mat<T>();
        for (uint i =0; i< in.n_cols;i++) {
            tmp = in(span::all,span(i),span::all);
            out(span::all,span(i),span::all) = tmp.t();
        }
    }
    else if (direction == SWAP_YZ) {            // swap YZ
        out = Cube<T>(in.n_rows,in.n_slices,in.n_cols);
        Mat<T> tmp = Mat<T>();
        for (uint i =0; i< in.n_rows;i++) {
            tmp = in(span(i),span::all,span::all);
            out(span(i),span::all,span::all) = tmp.t();
        }
    }
    return out;
}

template<typename T>
Cube<T> mirrorCube(const Cube<T> &in, uchar direction)
{
    Cube<T> out = Cube<T>(in.n_rows,in.n_cols,in.n_slices);
    if (direction == FLIP_X) {          // flip X
        for (uint i =0; i< in.n_slices;i++)
            out.slice(i) = flipud(in.slice(i));
    }
    else if (direction == FLIP_Y) {     // flip Y
        for (uint i =0; i<  in.n_slices;i++)
            out.slice(i) = fliplr(in.slice(i));
    }
    else if (direction == FLIP_Z) {     // flip Z
        Mat<T> tmp = Mat<T>();
        for (uint i =0; i< in.n_cols;i++) {
            tmp = in(span::all,span(i),span::all);
            out(span::all,span(i),span::all) = fliplr(tmp);
        }
    }
    return out;
}

template<typename T>
void orientCubes(const QVector<uchar> &orientation, MyProgressDialog *progress,
                 AnalyzeHeader &header, QList< Cube<T> > &data)
{
    if (orientation.isEmpty() || data.isEmpty())
        return;
    static umat s = Mat<uword>(2,3);
    s <<1<<2<<endr<<1<<3<<endr<<2<<3<<endr;
    progress->setLabelText("Rotating images ... ");
    progress->setRange(0,orientation.size()+2);
    progress->setValue(0);
    for (int k = 0; k < orientation.size();k++) {
        uint o = orientation.at(k);
        if (o < 3) {    // permutation
            for (int i =0; i<data.size();i++)
                data[i] = swapCube(data[i],orientation.at(k));
            // correct header information
            swap(header.dime.dim[s(o,1)],header.dime.dim[s(o,1)]);
            swap(header.dime.pixdim[s(o,1)],header.dime.pixdim[s(o,1)]);
        }
        else {          // mirroring
            for (int i =0; i<data.size();i++)
                data[i] = mirrorCube(data[i],orientation.at(k));
        }
        progress->setValue(progress->value()+1);
    }
}

// read image function
template<typename T>
bool readImage(QString imgFileName, uint fileType, bool preview,MyProgressDialog *progress,
               AnalyzeHeader &header, QList< Cube<T> > &out)
{
    if (!checkImageFile(imgFileName, fileType,header))
        return false;
    // check file openning
    QFile file(imgFileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    // check file size
    quint32 nRows = header.dime.dim[1],
            nCols = header.dime.dim[2],
            nSlices = header.dime.dim[3],
            nVolumes = header.dime.dim[4],
            nVox = nRows*nCols*nSlices,
            dt = header.dime.datatype;
    if (fileType == NIFTI)
        file.seek(qint64(header.dime.vox_offset));
    // read file according to the datatype
    // if preview load the first volume only
    if (preview)
        nVolumes = 1;
    // prepare progress bar
    progress->setLabelText("Reading Image ... ");
    progress->setRange(0,2*nVolumes+2);
    progress->setModal(true);
    progress->setFixedSize(progress->sizeHint());
    progress->setValue(0);
    progress->show();
    // read data
    if (dt == DT_BINARY) {
        // this code is only meaningful in case of uchar
        if (sizeof(T) != sizeof(uchar)) {
            file.close();
            return false;
        }
        int n = nVox*nVolumes/8;
        uchar *data = new uchar[n];
        if (file.read( (char *) data, sizeof(uchar)*n) != sizeof(uchar)*n) {
            QMessageBox::critical(0, "Error", "Can not read file.");
            file.close();
            return false;
        }
        quint32 c = 0;
        // decode bit by bit
        uchar *tmp = new uchar[n*8];
        for (int i = 0; i<n; i++) {
            for (int j = 0; j<8; j++)
                tmp[c++] = (data[i] & (1<<j)) >> j;
        }
        delete data;
        // store data
        for (uint k = 0; k<nVolumes;k++) {
            T *data = new T[nVox];
            for (uint i = 0;i<nVox;i++)
                data[i] = tmp[i+k*nVox];
            out.append(Cube<T>(data,nRows,nCols,nSlices,false,false));
            progress->setValue(progress->value()+1);
        }
        delete tmp;
    }
    else {
        for (uint j = 0; j<nVolumes;j++) {
            out.append(Cube<T>(nRows,nCols,nSlices));
            if (file.read( (char *) out.last().memptr(), sizeof(T)*nVox ) != sizeof(T)*nVox) {
                QMessageBox::critical(0, "Error", "Can not read file.");
                file.close();
                return false;
            }
            progress->setValue(progress->value()+1);
        }
    }
    file.close();
    // correct orientation for display
    // permute rows and columns
    if (fileType == NIFTI || fileType == ANALYZE) {
        for (uint i =0; i<nVolumes;i++) {
            out[i] = swapCube(out[i],SWAP_XY);
            progress->setValue(progress->value()+1);
        }
        // correct header information
        swap(header.dime.dim[1],header.dime.dim[2]);
        swap(header.dime.pixdim[1],header.dime.pixdim[2]);
    }
    return true;
}

bool readHeader(QString headerFileName, AnalyzeHeader& header, ImageFileType fileType)
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

bool checkImageFile(QString imgFileName, uint fileType, const AnalyzeHeader &header)
{
    if (imgFileName.isEmpty())
        return false;
    // check file openning
    QFile file(imgFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, "Error", QString("Cannot read file %1:\n%2.")
                              .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    // check file size
    quint32 nRows = header.dime.dim[1],
            nCols = header.dime.dim[2],
            nSlices = header.dime.dim[3],
            nVolumes = header.dime.dim[4],
            nVox = nRows*nCols*nSlices,
            dt = header.dime.datatype,
            fileSize;
    if (dt == DT_BINARY)
        fileSize = nVox/8*nVolumes;
    else if (dt == DT_UNSIGNED_CHAR)
        fileSize = nVox*nVolumes;
    else if (dt == DT_SIGNED_SHORT)
        fileSize = nVox*2*nVolumes;
    else if ( (dt == DT_SIGNED_INT) || (dt == DT_FLOAT))
        fileSize = nVox*4*nVolumes;
    else if (dt == DT_DOUBLE)
        fileSize = nVox*8*nVolumes;
    if (fileType == NIFTI)
        fileSize += header.dime.vox_offset; // add header size
    if (file.size() != fileSize ) {
        QMessageBox::critical(0, "Error", "File size does not agree with header data.");
        file.close();
        return false;
    }
    file.close();
        return true;
}

template <typename T>
bool writeImage(QString imgfileName, uint fileType,
                AnalyzeHeader header, QList< Cube<T> > &data)
{
    // check file
    QFile file(imgfileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(0, "Warning!", QString("Cannot write image file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    // permute rows and columns
    for (int i =0; i<data.size();i++)
        data[i] = swapCube(data[i],SWAP_XY);
    // write header for NIFTI files
    if (fileType == NIFTI) {
        // correct header information
        swap(header.dime.dim[1],header.dime.dim[2]);
        swap(header.dime.pixdim[1],header.dime.pixdim[2]);
        file.write( (char *) &header, sizeof(header) );
        file.seek(qint64(header.dime.vox_offset));
    }
    // write file
    uint nVox = data.first().n_elem;
    for (int i = 0;i<data.size();i++)
        if (!(file.write( (char *) data[i].memptr(),
                          sizeof(T)*nVox) == sizeof(T)*nVox)) {
            QMessageBox::critical(0, "Error", "Can not write file.");
            file.close();
            return false;
        }
    file.close();
    return true;
}

// main function to write a file
template<typename T>
bool saveImages(QString fileName, uint fileType,
                AnalyzeHeader header, QList< Cube<T> > &data)
{
    /*
        ANALYZE75WRITE writes image file of Mayo Analyze 7.5 data set.
        ANALYZE75WRITE writes the image data from the IMG file of
        an Analyze 7.5 format data set (a pair of fileName.HDR and fileName.IMG
        files).  For single-frame images, I is an M-by-N array where M is the
        number of rows and N is the number of columns. For multi-dimensional
        images, I can be an M-by-N-by-O or M-by-N-by-O-by-P array where M is
        the number of rows, N is the number of columns, O is the number of
        slices per volume and P is the number of volumes or time points. The
        data type of I is consistent with the image data type specified in
        the metadata obtained from the header file.
    */
    if (!fileName.isEmpty()) {
        // assure correct fileNames for header and image files
        fileName = QFileInfo(fileName).absolutePath() + "/" + QFileInfo(fileName).baseName();
        if (fileType == ANALYZE) {
            QString headerFileName = fileName + ".hdr";
            // read header file
            if (writeHeader(headerFileName, header)) {
                // read image file
                QString imgFileName = fileName + ".img";
                return writeImage(imgFileName, fileType, header, data);
            }
        }
        else if (fileType == NIFTI) {
            QString imgFileName = fileName + ".nii";
            return writeImage(imgFileName, fileType, header, data);
        }
    }
    return false;
}

// prepare header
AnalyzeHeader prepareHeader(quint32 dim[4], ImageDataType dataType,
                            float pixDim[3], ImageFileType fileType)
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
    header.dime.datatype         = (dataType == DT_BINARY)?DT_UNSIGNED_CHAR:dataType;
    header.dime.funused1         =0.0;
    header.dime.funused2         =0.0;
    header.dime.funused3         =0.0;
    header.dime.cal_max          = 0;
    header.dime.cal_min          = 0;
    header.dime.compressed       = 0;
    header.dime.verified         = 0;
    header.dime.glmin            = INT_MAX;
    header.dime.glmax            = INT_MIN;

    if (dataType == DT_BINARY)
        header.dime.bitpix       = 8;
    else if (dataType == DT_UNSIGNED_CHAR)
        header.dime.bitpix       = 8;
    else if (dataType == DT_SIGNED_SHORT)
        header.dime.bitpix       = 16;
    else if (dataType == DT_SIGNED_INT || dataType == DT_FLOAT)
        header.dime.bitpix       = 32;
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
    dmp.nRows       = header.dime.dim[1];
    dmp.nCols       = header.dime.dim[2];
    dmp.nSlices     = header.dime.dim[3];
    dmp.dx          = header.dime.pixdim[1];
    dmp.dy          = header.dime.pixdim[2];
    dmp.dz          = header.dime.pixdim[3];
    return dmp;
}
