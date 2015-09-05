#include "diffusionmodel.h"
DiffusionModel::DiffusionModel()
{
    modelType       = EMPTY;
    longestFiber    = 450;
}

void DiffusionModel::clear()
{
    // clear all data and assign empty to the model
    modelType = EMPTY;

    Mask.clear();
    b0.clear();

    TOD.clear();    nTensors.clear();
    q.clear();      qIndex.clear();
    Rx.clear();     Ry.clear();     Rz.clear();

    eVecPx.clear(); eVecPy.clear(); eVecPz.clear();
    FA.clear();
}
// reading files
/**************************************************************************************************/
bool DiffusionModel::readTOD(const QString &fileName, MyProgressDialog *progress)
{
    if (fileName.isEmpty())
        return false;
    this->clear();
    // try opening the file
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0, "Warning!", QString("Cannot read file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    // test it is a TOD file
    char temp[3];
    file.read( (char *) &temp, 3 );
    QString test = QString(temp);
    test.resize(3);
    if ( test != "TOD" ) {
        QMessageBox::warning(0, "Warning!", "The file is not a TOD file.");
        return false;
    }
    // read file header
    quint32 nDirQ, fileSize;
    double ver;
    file.read( (char *) &nRows,4 );
    file.read( (char *) &nCols,4 );
    file.read( (char *) &nSlices,4 );
    file.read( (char *) &maxNTens, 4 );
    file.read( (char *) &nDirQ, 4 );
    file.read( (char *) &ver, 8 );
    file.read( (char *) &fileSize, 4 );
    file.read( (char *) &dx,4 );
    file.read( (char *) &dy,4 );
    file.read( (char *) &dz,4 );
    file.read( (char *) &th, 4 );
    if ( fileSize != quint32(file.size()) ) {
        QMessageBox::warning(0, "Warning!", "The file size is not correct, can't load the TOD file.");
        return false;
    }
    progress->setLabelText("Loading Model ... ");
    progress->setRange(0,7);
    progress->setFixedSize(progress->sizeHint());
    progress->setModal(true);
    progress->setValue(0);
    progress->show();

    // start reading the TOD file content
    file.seek(64);
    nVox = nRows*nCols*nSlices;
    // read data into arma volumes
    //q:            [3*N double]
    //qIndex:       [Nten*X*Y*Z uint8]
    //TOD:          [Nten*X*Y*Z double]
    //mask:         [X*Y*Z uint8]
    //n_tensors:    [X*Y*Z uint8]
    //EI:           [X*Y*Z double]
    //R:            [X*Y*Z*3 double] will read it in int
    // qIndex      [Nten*X*Y*Z uint8]
    qIndex = uchar_colvec(nVox*maxNTens);
    file.read( (char *) qIndex.memptr(), nVox*maxNTens );
    progress->setValue(1);
    // TOD          [Nten*X*Y*Z double]
    TOD = colvec(nVox*maxNTens);
    file.read( (char *) TOD.memptr(), sizeof(double)*nVox*maxNTens );
    progress->setValue(2);
    //mask:         [X*Y*Z uint8]
    Mask = uchar_cube(nRows,nCols,nSlices);
    file.read( (char *) Mask.memptr(), nVox );
    progress->setValue(3);
    //n_tensors:    [X*Y*Z uint8]
    nTensors = uchar_cube(nRows,nCols,nSlices);
    file.read( (char *) nTensors.memptr(), nVox );
    progress->setValue(4);
    //EI:           [X*Y*Z double]
    b0 = cube(nRows,nCols,nSlices);
    file.read( (char *) b0.memptr(), sizeof(double)*nVox );
    progress->setValue(5);
    //q:            [3*N double]
    // since the qIndex starts from 1 since matlab indexing start from 1,
    // we add a dummy column of zeros at first
    q = mat(3,nDirQ);
    file.read( (char *) q.memptr(), sizeof(double)*nDirQ*3 );
    q.insert_cols(0,1);
    originalQ = q;
    progress->setValue(6);
    //R:            [X*Y*Z*3 double]
    if (ver >= 0) {
        double *tmpDblIn = new double[nVox];
        quint8 *tmpChIn = new quint8[nVox];
        file.read( (char *) tmpDblIn, sizeof(double)*nVox );
        for (uint i=0; i<nVox;i++)
            tmpChIn[i] = 255*qAbs(tmpDblIn[i]);
        Rx = uchar_cube(tmpChIn,nRows,nCols,nSlices,false,true);
        delete tmpDblIn;

        tmpDblIn = new double[nVox];
        tmpChIn = new quint8[nVox];
        file.read( (char *) tmpDblIn, sizeof(double)*nVox );
        for (uint i=0; i<nVox;i++)
            tmpChIn[i] = 255*qAbs(tmpDblIn[i]);
        Ry = uchar_cube(tmpChIn,nRows,nCols,nSlices,false,true);
        delete tmpDblIn;

        tmpDblIn = new double[nVox];
        tmpChIn = new quint8[nVox];
        file.read( (char *) tmpDblIn, sizeof(double)*nVox );
        for (uint i=0; i<nVox;i++)
            tmpChIn[i] = 255*qAbs(tmpDblIn[i]);
        Rz = uchar_cube(tmpChIn,nRows,nCols,nSlices,false,true);
        delete tmpDblIn;
    }
    else {
        file.close();
        clear();
        return false; // should update code to compute R
    }

    file.close();
    modelType = TDF;
    progress->setValue(7);
    return true;
}

bool DiffusionModel::readDTI(const QString &fileName,QList<fcube> &ADC,
                             QList<fcube> &metrics,MyProgressDialog *progress)
{
    // this function returns the ADC to the user and sets the
    // current model to DTI and acquire the model parameters
    // it also computes the metrics and returns it to the user
    // it helds the principal eigen vector and FA for tractography
    // returned metrics are: eVal1, eVal2, eVal3, MD, FA, RA, VR

    if (fileName.isEmpty())
        return false;
    this->clear();

    AnalyzeHeader header;
    if(!readDTI(fileName,ADC,header,progress))
        return false;
    setDTI(ADC,metrics,header,progress);
}

bool DiffusionModel::readDTI(const QString &fileName, QList<fcube> &ADC,
                             AnalyzeHeader &header, MyProgressDialog *progress)
{
    if (fileName.isEmpty())
        return false;
    OpenImageDialog w(fileName,progress->parentWidget());
    w.LabelCheck->setEnabled(false);
    w.BitChoice->setEnabled(false);
    w.ByteChoice->setEnabled(false);
    w.Integer16Choice->setEnabled(false);
    w.Integer32Choice->setEnabled(false);
    if (!w.exec())
        return false;
    // read analyze
    ImageFileType ft = w.fileType();
    // start by reading header data
    QString fn = QFileInfo(fileName).absolutePath()+"/"+QFileInfo(fileName).baseName();
    QString imgFileName = (ft == RAW)?fileName:(ft == ANALYZE)?(fn+".img"):(fn+".nii");
    if (w.fileType() == RAW)
        // get file information data in form of a header
        w.getHeader(header);
    else {
        QString headerFileName = (ft == ANALYZE)?(fn+".hdr"):(fn+".nii");
        if (!readHeader(headerFileName, header, ft))
            return false;
    }
    // Acquire information from the header
    uint    dt          = header.dime.datatype,
            nVolsImg    = header.dime.dim[4];

    if (nVolsImg != 6) {
        QMessageBox::critical(progress->parentWidget(),"Wrong DTI file","Number of volumes in the "
                              "selected file is not equal to 6. The file can not be a DTI model file.");
        return false;
    }
    else if (!(dt == DT_FLOAT || dt == DT_DOUBLE)) {
        QMessageBox::critical(progress->parentWidget(),"Wrong DTI file","The datatype in the selected "
                              "file is not FLOAT neither DOUBLE. The file can not be a DTI model file.");
        return false;
    }

    DTIDialog wd;
    if(!wd.exec())
        return false;

    // read data according to its type, tensor expected to be
    // either double or float, it can not be an integer
    uvec idx;
    if (dt == DT_FLOAT) {
        QList< fcube > out = QList< fcube >();
        if (readImage(imgFileName,ft,false,progress,header,out)) {
            if (!w.orientation.isEmpty())
                orientCubes(w.orientation, progress,header,out);
            idx = find(wd.tensorOrder == 0,1); ADC.append(out[idx[0]]);
            idx = find(wd.tensorOrder == 1,1); ADC.append(out[idx[0]]);
            idx = find(wd.tensorOrder == 2,1); ADC.append(out[idx[0]]);
            idx = find(wd.tensorOrder == 3,1); ADC.append(out[idx[0]]);
            idx = find(wd.tensorOrder == 4,1); ADC.append(out[idx[0]]);
            idx = find(wd.tensorOrder == 5,1); ADC.append(out[idx[0]]);
            out.clear();
        } else return false;
    }
    else if (dt == DT_DOUBLE) {
        QList< cube > out = QList< cube >();
        if (readImage(imgFileName,ft,false,progress,header,out)) {
            if (!w.orientation.isEmpty())
                orientCubes(w.orientation, progress,header,out);
            idx = find(wd.tensorOrder == 0,1); ADC.append(conv_to<fcube>::from(out[idx[0]]));
            idx = find(wd.tensorOrder == 1,1); ADC.append(conv_to<fcube>::from(out[idx[0]]));
            idx = find(wd.tensorOrder == 2,1); ADC.append(conv_to<fcube>::from(out[idx[0]]));
            idx = find(wd.tensorOrder == 3,1); ADC.append(conv_to<fcube>::from(out[idx[0]]));
            idx = find(wd.tensorOrder == 4,1); ADC.append(conv_to<fcube>::from(out[idx[0]]));
            idx = find(wd.tensorOrder == 5,1); ADC.append(conv_to<fcube>::from(out[idx[0]]));
            out.clear();
        } else return false;
    }
    return true;
}

void DiffusionModel::setDTI(const QList<fcube> &ADC, QList<fcube> &metrics,
                            const AnalyzeHeader &header,MyProgressDialog *progress)
{
    this->clear();
    /*
      The background color of that model will be the principal
      eigenvector of the diffusion tensor
      */
    // compute dti stuff
    computeDTIStuff(ADC,metrics,progress);
    if (metrics.size() < 10)
        return;

    nRows   = header.dime.dim[1];
    nCols   = header.dime.dim[2];
    nSlices = header.dime.dim[3];
    nVox    = nRows*nCols*nSlices;
    dx      = header.dime.pixdim[1];
    dy      = header.dime.pixdim[2];
    dz      = header.dime.pixdim[3];

    modelType = DTI;

    // held the eigen vector and FA
    FA = metrics[4];
    eVecPz = metrics.takeAt(9);
    eVecPy = metrics.takeAt(8);
    eVecPx = metrics.takeAt(7);

    // create mask for FA > 0.45
    Mask = conv_to<uchar_cube>::from(round(FA+0.45));
}

bool DiffusionModel::readDWI(const QString &fileName, ImageFileType fileType,
                             QList<s16_cube> &dwi, AnalyzeHeader &header,
                             MyProgressDialog *progress)
{
    if (fileName.isEmpty())
        return false;
    // read data
    // start by reading header data
    if (fileType == RAW) return false;
    QString fn = QFileInfo(fileName).absolutePath()+"/"+QFileInfo(fileName).baseName();
    QString imgFileName = (fileType == ANALYZE)?(fn+".img"):(fn+".nii");
    QString headerFileName = (fileType == ANALYZE)?(fn+".hdr"):(fn+".nii");
    if (!readHeader(headerFileName, header, fileType))
        return false;
    // Acquire information from the header
    uint dt = header.dime.datatype;
    // read data according to its type, diffusion data can not be float or double
    if (dt == DT_UNSIGNED_CHAR) {
        QList< uchar_cube > out = QList< uchar_cube >();
        if (readImage(imgFileName,fileType,false,progress,header,out)) {
            for (int i =0;i<out.size();i++)
                dwi.append(conv_to<s16_cube>::from(out[i]));
            out.clear();
        } else return false;
    }
    else if (dt == DT_SIGNED_SHORT) {
        if (readImage(imgFileName,fileType,false,progress,header,dwi)) {
        } else return false;
    }
    else if (dt == DT_SIGNED_INT) {
        QList< s32_cube > out = QList< s32_cube >();
        if(readImage(imgFileName,fileType,false,progress,header,out)) {
            for (int i =0;i<out.size();i++)
                dwi.append(conv_to<s16_cube>::from(out[i]));
            out.clear();
        } else return false;
    }
    else {
        QMessageBox::critical(0,"Faulty data","Non integer data can not be used for diffusion.");
        return false;
    }
    return true;
}

bool DiffusionModel::computeDTIModel(QList<s16_cube> &dwi, mat gTable,
                                     float bValue, int nLevel, QList<fcube> &ADC,
                                     MyProgressDialog *progress)
{
    if (dwi.isEmpty() || gTable.n_rows != dwi.size() || gTable.n_cols != 3)
        return false;
    // get data info
    uint r = dwi.first().n_rows, c = dwi.first().n_cols, s = dwi.first().n_slices;
    // get the mean b0 image
    uvec vv = find(sum(abs(gTable),1) == 0);
    // if no b0 then we can not continue
    if (vv.is_empty()) return false;
    cube b0 = zeros<cube>(r,c,s);
    // compute the mean and remove b0 images from dwi and table
    for (int i = vv.n_elem-1;i>-1;--i) {
        b0 += conv_to<cube>::from(dwi[vv(i)]);
        dwi[vv(i)].clear();
        dwi.removeAt(vv(i));
        gTable.shed_row(vv(i));
    }
    b0 /= vv.n_elem;
    // remove any erronous volumes with vector (100,100,100)
    vv = find(sum(abs(gTable),1) == 300);
    for (int i = vv.n_elem-1;i>-1;--i) {
        dwi[vv(i)].clear();
        dwi.removeAt(vv(i));
        gTable.shed_row(vv(i));
    }
    // prepare progress dialog box
    progress->setLabelText(QString("Computing ADC for slice XXX of %1.").arg(s));
    progress->setRange(0,s);
    progress->setFixedSize(progress->sizeHint());
    progress->setModal(true);
    progress->setValue(0);
    progress->show();

    // prepare B matrix
    mat B = gTable%gTable;                          // gx^2 gy^2 gz^2
    B.reshape(B.n_rows,6);
    B.col(3) = 2*gTable.col(0)%gTable.col(1);       // gxgy
    B.col(4) = 2*gTable.col(0)%gTable.col(2);       // gxgz
    B.col(5) = 2*gTable.col(1)%gTable.col(2);       // gygz
    B *= bValue;
    if (!pinv(B,B))
        return false;
    // prepare ADC
    ADC.clear();
    for (int i = 0;i<6;++i)
        ADC.append(zeros<fcube>(r,c,s));
    vec norSig = zeros<vec>(dwi.size()), adc = zeros<vec>(6);
    for (uint z = 0; z < s; ++z) {
        progress->setLabelText(QString("Computing ADC for slice %1 of %2.").arg(z).arg(s));
        for (uint x = 0; x < r; ++x)
            for (uint y = 0; y < c; ++y) {
                if (b0(x,y,z) >= nLevel) {
                    for (int i =0; i< dwi.size();++i)
                        norSig(i) = dwi.at(i)(x,y,z)/b0(x,y,z);                    
                    norSig = -arma::log(norSig);
                    // correct inf issue, replace it with -log(0.9999)
                    norSig(find(norSig == datum::inf)).fill(1e-5);
                    adc = B*norSig;
                    for (int i =0;i<6;++i)
                        ADC[i](x,y,z) = adc(i);
                }
            }
        progress->setValue(z);
    }
    return true;
}

void DiffusionModel::computeDTIStuff(const QList<fcube> &ADC, QList<fcube> &metrics,
                                     MyProgressDialog *progress)
{
    if (ADC.size() != 6)
        return;
    int r = ADC.first().n_rows, c = ADC.first().n_cols, s = ADC.first().n_slices;
    progress->setLabelText(QString("Computing DTI metrics for slice 1 of %1.").arg(s));
    progress->setRange(0,s);
    progress->setFixedSize(progress->sizeHint());
    progress->setModal(true);
    progress->setValue(0);
    progress->show();

    // the order of the output
    // eVal1, eVal2, eVal3, MD, FA, RA, VR, eVecPx,eVecPy,eVecPz
    for (uint i =0;i<10;++i)
        metrics.append(zeros<fcube>(r,c,s));

    fvec eigVal;
    fmat eigVec, Tensor;
    float avgD, tmp;
    for (uint z = 0; z < s; ++z) {
        progress->setLabelText(QString("Computing DTI metrics for slice %1 of %2.").
                               arg(z+1).arg(s));
        for (uint x = 0; x < r; ++x)
            for (uint y = 0; y < c; ++y) {
                // compose the tensor
                Tensor << ADC[0](x,y,z) << ADC[3](x,y,z) << ADC[4](x,y,z) << endr
                       << ADC[3](x,y,z) << ADC[1](x,y,z) << ADC[5](x,y,z) << endr
                       << ADC[4](x,y,z) << ADC[5](x,y,z) << ADC[2](x,y,z) << endr;
                // decompose the symmetric tensor
                // test for unsuccessful decomposition
                if ( ADC[0](x,y,z) != 0.0 && eig_sym(eigVal, eigVec, Tensor)) {
                    // store eigenvalues, note they are sorted in ascending order
                    metrics[0](x,y,z) = eigVal(2);
                    metrics[1](x,y,z) = eigVal(1);
                    metrics[2](x,y,z) = eigVal(0);
                    // store the principal eigenvector, assumed the last column
                    metrics[7](x,y,z) = eigVec(0,2);
                    metrics[8](x,y,z) = eigVec(1,2);
                    metrics[9](x,y,z) = eigVec(2,2);
                    // compute the metrics, for defintions, review:
                    // PETER B. KINGSLEY
                    // Introduction to Diffusion Tensor Imaging Mathematics:
                    // Part II. Anisotropy, Diffusion-Weighting Factors, and
                    // Gradient Encoding Schemes
                    // Concepts in Magnetic Resonance Part A DOI 10.1002/cmr.a
                    // correct for negative eigen values
                    avgD = sum(abs(eigVal))/3.0;
                    tmp = sqrt(pow((eigVal(0)-avgD),2)+
                               pow((eigVal(1)-avgD),2)+
                               pow((eigVal(2)-avgD),2));
                    // in order: MD, FA, RA, VR
                    metrics[3](x,y,z) = avgD;
                    metrics[4](x,y,z) = qMax(qMin(tmp/sqrt(1/1.5*dot(eigVal,eigVal)),1.0),0.0);
                    metrics[5](x,y,z) = qMax(qMin(tmp/(sqrt(6.0)*avgD),sqrt(2.0)),0.0);
                    metrics[6](x,y,z) = qMax<float>(qMin<float>(prod(eigVal)/pow(avgD,3),1.0),0.0);
                }
                Tensor.clear();
            }
        progress->setValue(z+1);
    }
}

// Fiber tracking
/**************************************************************************************************/
// TOD Probablistic Tractography
bool DiffusionModel::probablistic_TOD_Tracking(TrackingParameters tp,
                                               FiberTracts &fibers,
                                               MyProgressDialog *progress)
{
    if (modelType != TDF)
        return false;
    // this function takes as input the ROI points and the tracking parameters
    // and output the fibers
    double minRatio = 0.4, ratio, straight, length,
            reserveLen = (longestFiber+5.0)/tp.propStep;
    int nPtsPerFiber;
    QVector3D   seed, propDir;


    // double in [0, 1) generator, already init
//    time_t ltime;
//    time( &ltime );
//    struct _timeb tstruct;
//    _ftime64_s( &tstruct );
//    seedr = ltime/100+tstruct.millitm;
    QTime t = QTime::currentTime();
    unsigned long seedr = t.hour()+t.minute()+t.second()+t.msec();
    MTRand drand(seedr);

    // use all brain as ROI
    QVector<QVector3D> ROI_Points;
    ROI_Points.reserve(accu(Mask));
    for (uint i=0;i<nRows;i++)
        for (uint j=0;j<nCols;++j)
            for (uint k=0;k<nSlices;k++)
                if (Mask(i,j,k)==1)
                    ROI_Points.append(QVector3D(i,j,k));
    ROI_Points.squeeze();

    progress->setLabelText("Generating tracts ...");
    progress->setRange(0,ROI_Points.size());
    progress->setModal(true);
    progress->setValue(0);
    progress->setFixedSize(progress->sizeHint());
    progress->show();

    // initialize the fiber object, we will clear all previously existing fibers
    fibers.clear();
    fibers.setDataDimension(nRows,nCols,nSlices);
    fibers.setVoxelDimension(dx,dy,dz);

    for (int i=0;i<ROI_Points.size();++i) {
        if (progress->wasCanceled())
            return false;

        if ( Mask(ROI_Points[i].x(),ROI_Points[i].y(),ROI_Points[i].z()) == 0)
            continue;
        QVector<QVector3D> fiber;
        for (int j=0;j<tp.tractsPerSeed;++j) {
            // everytime start from a random location
            seed = ROI_Points[i] - QVector3D(0.5,0.5,0.5) + QVector3D(drand(),drand(),drand());
            fiber.reserve(reserveLen);
            fiber.append(seed);
            // building in one direction (up) first
            createProbabilistic_TOD_Tract(seed,QVector3D(0,0,0),tp,fiber);
            // building in the other direction (down)
            if ( fiber.last() != fiber.first() ) {
                // store the prop direction before flipping
                propDir = (fiber[0]-fiber[1]).normalized();
                // flip fiber
                for(float k = 0; k < (fiber.size()/2.0); k++)
                    qSwap(fiber[k],fiber[fiber.size()-(1+k)]);
                createProbabilistic_TOD_Tract(seed,propDir,tp,fiber);
            }
            // remove excessive memory
            fiber.squeeze();
            straight = (fiber.first() - fiber.last()).length();
            nPtsPerFiber = fiber.size();
            ratio = straight/(0.25*nPtsPerFiber);
            if (ratio > minRatio) {
                // compute fiber length, we take the voxel size in consideration
                length = tp.propStep*fiber.size();
                // test length, no fiber could be longer than 350 mm, check:
                // Lee, Jin-Hee ; Hwang Shin, Su-Jeong ; Istook, Cynthia L.
                // Analysis of Human Head Shapes in the United States
                // International journal of human ecology; ISSN:1598-1762
                // @ 1598-9593 @ ; VOL.7; NO.1; PAGE.77-83; (2006)
                if (length >= tp.minFiberLength)  {
                    if (tp.smooth)
                        fiber = smoothFiber(fiber,tp.smoothingQual);
                    fibers.append(fiber,tp.fiberColor,length);
                }
            }
            fiber.clear();
        }
        progress->setValue(i+1);
    }
    // update fibers length array
    return true;
}
void DiffusionModel::createProbabilistic_TOD_Tract(QVector3D seed,QVector3D propDir,
                                                   const TrackingParameters &tp,
                                                   QVector<QVector3D> &fiber)
{
    bool ContinueFlag = true;
    double step = qSqrt(2.0)*tp.propStep/(QVector3D(dx,dy,dz).length());

    int nTen, loc;
    uchar_vec qIdx;
    vec tod;

    while (ContinueFlag) {
        // get integer point
        int j = qRound(seed.x());
        int k = qRound(seed.y());
        int l = qRound(seed.z());
        // test for out of range voxel
        if (j<0 || j>=nRows || k<0 || k>=nCols || l<0 || l>=nSlices)
            break;
        nTen = nTensors(j,k,l);                 // get number of tensors
        if((Mask(j,k,l) == 0) ||(nTen == 0))    // test for non-relevant voxel
            break;
        else {
            qIdx = Col<unsigned char>(nTen);
            tod = Col<double>(nTen);
            for (int i=0; i<nTen;i++){
                loc = nRows*nCols*maxNTens*l + nRows*maxNTens*k + maxNTens*j + i;
                qIdx(i) = qIndex(loc);
                tod(i) = TOD(loc);
            }
            propDir = getProbablistic_TOD_PropagationVector(tod,nTen,qIdx,
                                                            propDir,tp.maxBendingAngle);
            seed += step*propDir;
            fiber.append(seed);
        }
        if (tp.propStep*fiber.size() > longestFiber)
            ContinueFlag = false;
    }
}
QVector3D DiffusionModel::getProbablistic_TOD_PropagationVector(vec tod,int nTen,
                                                                uchar_vec qIdx,
                                                                QVector3D oldPropDir,
                                                                double maxBendingAngle)
{
    bool directionFound = false, flag;
    double s,t, dotProd;
    int nTries = 0;
    // to solve the problem if there is no propagation direction
    QVector3D newPropDir = oldPropDir;
    tod = tod/sum(tod);
    double sumOldPropDir = oldPropDir.x()+oldPropDir.y()+oldPropDir.z();
    //    timeval a;
    //    gettimeofday(&a,NULL);
    //    drand.seed(a.tv_sec*100 + a.tv_usec/100);
    // double in [0, 1) generator, already init
//    time_t ltime;
//    time( &ltime );
//    struct _timeb tstruct;
//    _ftime64_s( &tstruct );
//    seedr = ltime/100+tstruct.millitm;
    QTime time = QTime::currentTime();
    unsigned long seedr = time.hour()+time.minute()+time.second()+time.msec();
    MTRand drand(seedr);

    while (!directionFound) {
        t = drand();// Throwing a random number
        s = 0;
        flag = true;
        for (int j=0; j<nTen; j++) {
            if(flag) {
                s  = s + tod(j);
                if(s > t) {
                    flag = false;
                    newPropDir = QVector3D(q(0,qIdx(j)),q(1,qIdx(j)),q(2,qIdx(j)));
                }
            }
        }
        nTries++;
        // oldPropDir . newPropDir = cos(theta) since they are unit vectors
        /* note that theta should  be less than  the bendMax not greater,
          the reason for  that can be understood from the following drawing:
                /
               /theta
          --->/---->

          from the above, we can see that we want the smallest possible theta to
          assure that the fiber doesn't go into an accute direction,
          the dotProd should be greater than the maxBendingAngle since in fact
          the maxBendingAngle = cos(Angle), for the cosine, decreasing the angle
          will increase the cosine value. In equation, we want:
          theta < maxAngle -> cos(theta) > cos(maxAngle)
          */
        dotProd = QVector3D::dotProduct(oldPropDir,newPropDir);
        if (abs(dotProd) > maxBendingAngle || sumOldPropDir ==0 || nTries == 5)
            directionFound = true;
    }
    if(dotProd < 0)
        newPropDir = -newPropDir;
    return newPropDir;
}
/**************************************************************************************************/
// FACT tracking
bool DiffusionModel::FACT_DTI(TrackingParameters tp,FiberTracts &fibers,MyProgressDialog *progress)
{
    double nSeeds, reserveLen = (longestFiber+5.0)/tp.propStep;
    if (modelType != DTI)
        return false;
    // this function takes as input the ROI points and the tracking parameters
    // and output the fibers
    double length;
    QVector3D   propDir;
    // use all brain as ROI
    QVector<QVector3D> ROI_Points;
    ROI_Points.reserve(accu(Mask));
    for (uint i=0;i<nRows;i++)
        for (uint j=0;j<nCols;j++)
            for (uint k=0;k<nSlices;k++)
                if (FA(i,j,k) >= tp.limitingMetricStart)
                    ROI_Points.append(QVector3D(i,j,k));
    ROI_Points.squeeze();
    nSeeds = ROI_Points.size();

    progress->setLabelText("Generating tracts ...");
    progress->setRange(0,ROI_Points.size());
    progress->setModal(true);
    progress->setValue(0);
    progress->setFixedSize(progress->sizeHint());
    progress->show();

    // initialize the fiber object, we will clear all previously existing fibers
    fibers.clear();
    fibers.setDataDimension(nRows,nCols,nSlices);
    fibers.setVoxelDimension(dx,dy,dz);
    QVector<QVector3D> fiber;
    int j =0, k =0,l = 0;
    for (int i=0;i<ROI_Points.size();++i) {
        if (progress->wasCanceled()) {
            // free memory
            fiber.clear();
            return false;
        }
        j = ROI_Points[i].x();
        k = ROI_Points[i].y();
        l = ROI_Points[i].z();
        // to speed up, reserve memory then squeeze
        fiber.reserve(reserveLen);
        fiber.append(ROI_Points[i]);
        // building in one direction (up) first
        propDir = QVector3D(eVecPy.at(j,k,l),eVecPx.at(j,k,l),eVecPz.at(j,k,l));
        createFACT_DTI_Tract(ROI_Points[i],propDir,tp,fiber);
        // building in the other direction (down)
        if ( fiber.last() != fiber.first() ) {
            // flip fiber
            for(float k = 0; k < (fiber.size()/2.0); k++)
                qSwap(fiber[k],fiber[fiber.size()-(1+k)]);
            createFACT_DTI_Tract(ROI_Points[i],-propDir,tp,fiber);
        }
        // remove excessive memory
        fiber.squeeze();
        // compute fiber length, we take the voxel size in consideration
        length = tp.propStep*fiber.size();
        if (length >= tp.minFiberLength)  {
            if (tp.smooth)
                fiber = smoothFiber(fiber,tp.smoothingQual);
            fibers.append(fiber,tp.fiberColor,length);
        }
        // free memory
        fiber.clear();
        progress->setValue(i+1);
    }
    return true;
}

void DiffusionModel::createFACT_DTI_Tract(QVector3D seed, QVector3D propDir,
                                          const TrackingParameters &tp,
                                          QVector<QVector3D> &fiber)
{
    bool ContinueFlag = true;
    double dotProd = 0, step = qSqrt(2.0)*tp.propStep/(QVector3D(dx,dy,dz).length());
    QVector3D newPropDir = QVector3D(0,0,0);
    // first seed is known to be integer and in the volume
    int i = seed.x(), j = seed.y(), k = seed.z();

    while (ContinueFlag) {
        // test for non-relevant voxel
        if(FA(i,j,k) < tp.limitingMetricStop)
            break;
        else {
            seed += step*propDir;
            fiber.append(seed);

            i = qRound(seed.x());
            j = qRound(seed.y());
            k = qRound(seed.z());

            // test for out of range voxel
            if (i<0 || i>=nRows || j<0 || j>=nCols || k<0 || k>=nSlices)
                break;

            // propagate
            newPropDir = QVector3D(eVecPy(i,j,k),eVecPx(i,j,k),eVecPz(i,j,k));
            dotProd = QVector3D::dotProduct(propDir,newPropDir);
            if(dotProd < 0) newPropDir = -newPropDir;
            // propDir . newPropDir = cos(theta) since they are unit vectors
            /* note that theta should  be less than  the bendMax not greater,
              the reason for  that can be understood from the following drawing:

                    /
                   /theta
              --->/---->

              from the above, we can see that we want to break once the angle surpasses
              the threshold angle, hence the dot product should be less than the
              maxBendingAngle since in fact the maxBendingAngle = cos(Angle), and for
              the cosine, increasing the angle will decrease the cosine value. In
              equation, break if theta > maxAngle -> cos(theta) < cos(maxAngle).

             */
            if(qAbs(dotProd) < tp.maxBendingAngle)
                ContinueFlag = false;
            propDir = newPropDir;
        }
        // no fiber could be longer than 350 mm, check:
        // Lee, Jin-Hee ; Hwang Shin, Su-Jeong ; Istook, Cynthia L.
        // Analysis of Human Head Shapes in the United States
        // International journal of human ecology; ISSN:1598-1762
        // @ 1598-9593 @ ; VOL.7; NO.1; PAGE.77-83; (2006)
        if (tp.propStep*fiber.size() > longestFiber)
            ContinueFlag = false;
    }
}

/**************************************************************************************************/
inline QVector<QVector3D> DiffusionModel::smoothFiber(QVector<QVector3D> &fiber,
                                                      int smoothingQual)
{
    // sample the fiber to decrease number of points
    // Given that the sampling quality can vary from 5 to 20,
    // I want small fibers (<50 points) stay small but not be
    // oversampled in case of large smoothing quality. On the other
    // hand, I want large fibers (>300) to be largerly sampled so that
    // the number of points after reconstruction does not exceed 300
    int skip = smoothingQual;
    if (fiber.size() < 50 && smoothingQual > 10)
        skip = 5;
    else if (fiber.size() > 300)
        skip = qCeil(fiber.size()*smoothingQual/300.0);

    QVector<QVector3D> tmp;
    tmp.reserve(qCeil(fiber.size()/skip)+1);
    for (int s = 0; s < fiber.size() - 1; s+=skip)
        tmp.append(fiber.at(s));
    tmp.append(fiber.last());
    return BSPLINE::bSplineDeBoor(tmp,smoothingQual);
}
