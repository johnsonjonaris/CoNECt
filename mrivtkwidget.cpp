#include "mrivtkwidget.h"

MRIvtkWidget::MRIvtkWidget(QWidget *parent): QVTKWidget(parent)
{
    mainRenderer = vtkSmartPointer<vtkRenderer>::New();
    this->GetRenderWindow()->AddRenderer(mainRenderer);
    empty = true;
    // create stuff
    brain3D = vtkSmartPointer<vtkImageData>::New();
    coronalActor = vtkSmartPointer<vtkImageActor>::New();
    sagittalActor = vtkSmartPointer<vtkImageActor>::New();
    axialActor = vtkSmartPointer<vtkImageActor>::New();
    linesActor = vtkSmartPointer<vtkActor>::New();
    scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
}

void MRIvtkWidget::clear()
{
    // clear labels volume if existing
    clearLabelVolumes();
    // clear 3D View
    QColor bg = this->parentWidget()->palette().window().color().rgb();
    mainRenderer->SetBackground(bg.redF(),bg.greenF(),bg.blueF());
    mainRenderer->GetViewProps()->RemoveAllItems();
    mainRenderer->Clear();
    mainRenderer->ResetCamera();
    this->update();
    // initialize 3D
    brain3D = NULL;
    coronalActor = NULL;
    sagittalActor = NULL;
    axialActor = NULL;
    linesActor = NULL;
    // remove scalar bar
    scalarBar = NULL;
    // notify self that it is empty
    empty = true;
}

void MRIvtkWidget::setVolume(const cube &activeVolume,bool isGrayscaleVolume,
                           float dxImg,float dyImg,float dzImg,
                           double minActVol, double maxActVol)
{
    nRowsImg        = activeVolume.n_rows;
    nColsImg        = activeVolume.n_cols;
    nSlicesImg      = activeVolume.n_slices;
    minVolumeValue  = minActVol;
    maxVolumeValue  = maxActVol;
    // fill the 3D volume with activeVol
    brain3D = NULL;
    brain3D = vtkSmartPointer<vtkImageData>::New();
    brain3D->SetDimensions(nRowsImg,nColsImg,nSlicesImg);
    brain3D->SetNumberOfScalarComponents(1);
    brain3D->SetSpacing(dxImg,dyImg,dzImg);
    brain3D->SetOrigin(0,0,0);
    // !activeImgIsLabel || (loadedImgsInfo(newVolIdx-1,9)==0)
    if (isGrayscaleVolume) {
        brain3D->SetScalarTypeToUnsignedChar();
        brain3D->AllocateScalars();
        uchar* pixel;
        for (int z = 0; z < nSlicesImg; z++)
            for (int y = 0; y < nColsImg; y++)
                for (int x = 0; x < nRowsImg; x++) {
                    pixel = static_cast<uchar*>(brain3D->GetScalarPointer(x,y,z));
                    pixel[0] = uchar( 255.0*(activeVolume(x,y,z)-minActVol)/(maxActVol-minActVol) );
                }
    }
    else {
        brain3D->SetScalarTypeToUnsignedShort();
        brain3D->AllocateScalars();
        ushort* pixel;
        for (int z = 0; z < nSlicesImg; z++)
            for (int y = 0; y < nColsImg; y++)
                for (int x = 0; x < nRowsImg; x++) {
                    // note that using the below technique is much faster than using:
                    // brain3D->SetScalarComponentFromDouble(x,y,z,0,activeVol(x,y,z));
                    pixel = static_cast<ushort*>(brain3D->GetScalarPointer(x,y,z));
                    pixel[0] = ushort( activeVolume(x,y,z) );
                }
    }
    // notify self to be non empty
    empty = false;
}

// handle 3D volume
void MRIvtkWidget::drawCurrentVolume(const QVector<QRgb> &cTable,bool isColorVolume,
                                     int selectedSlices[3],int opValue,bool enableCuts[3])
{
    if (isEmpty())
        return;
    // remove actors from renderer
    mainRenderer->RemoveActor(coronalActor);
    mainRenderer->RemoveActor(sagittalActor);
    mainRenderer->RemoveActor(axialActor);
    // Creating lookup table.
    vtkSmartPointer<vtkLookupTable> LUT = vtkSmartPointer<vtkLookupTable>::New();
    LUT->SetNumberOfColors(cTable.size());
    LUT->SetTableRange (0, cTable.size()-1);
    QColor c;
    for (int i = 0; i<cTable.size();i++) {
        c = cTable.at(i);
        if ( c == Qt::black && isColorVolume) // LUT_box->currentIndex() > 0
            LUT->SetTableValue(i,c.redF(),c.greenF(),c.blueF(),0);
        else
            LUT->SetTableValue(i,c.redF(),c.greenF(),c.blueF(),1);
    }
    LUT->Build(); //effective built
    /* Creating three orthogonal planes passing through the volume.
     Each plane uses a different texture map and therefore has different coloration.
     Create the first of the three planes. The filter vtkImageMapToColors
     maps the data through the corresponding lookup table created above.  The
     vtkImageActor is a type of vtkProp and conveniently displays an image on
     a single quadrilateral plane. It does this using texture mapping and as
     a result is quite fast. (Note: the input image has to be unsigned char
     values, which the vtkImageMapToColors produces.) Note also that by
     specifying the DisplayExtent, the pipeline requests data of this extent
     and the vtkImageMapToColors only processes a slice of data.
     */
    selCorSlice = selectedSlices[0];
    selSagSlice = selectedSlices[1];
    selAxSlice = selectedSlices[2];
    vtkSmartPointer<vtkImageMapToColors> BrainImg = vtkSmartPointer<vtkImageMapToColors>::New();
    BrainImg->SetInput(brain3D);
    BrainImg->SetLookupTable(LUT);
    BrainImg->Update();
    coronalActor = NULL;
    coronalActor = vtkSmartPointer<vtkImageActor>::New();
    coronalActor->SetInput(BrainImg->GetOutput());
    coronalActor->SetDisplayExtent(selCorSlice,selCorSlice, 0,nColsImg-1, 0,nSlicesImg-1);
    sagittalActor = NULL;
    sagittalActor = vtkSmartPointer<vtkImageActor>::New();
    sagittalActor->SetInput(BrainImg->GetOutput());
    sagittalActor->SetDisplayExtent(0,nRowsImg-1, selSagSlice,selSagSlice, 0,nSlicesImg-1);
    axialActor = NULL;
    axialActor = vtkSmartPointer<vtkImageActor>::New();
    axialActor->SetInput(BrainImg->GetOutput());
    axialActor->SetDisplayExtent(0,nRowsImg-1, 0,nColsImg-1, selAxSlice,selAxSlice);
    /* It is convenient to create an initial view of the data. The
       FocalPoint and Position form a vector direction. Later on
       (ResetCamera() method) this vector is used to position the camera
       to look at the data in this direction.
    */
    vtkSmartPointer<vtkCamera> aCamera = vtkSmartPointer<vtkCamera>::New();
    aCamera->SetViewUp (0, 0, 1);
    aCamera->SetPosition (0, 1, 0);
    aCamera->SetFocalPoint (0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    aCamera->Azimuth(30.0);
    aCamera->Elevation(30.0);
    // Set a background color for the renderer
    mainRenderer->SetBackground(0.2, 0.3, 0.4);
    // apply opacity
    coronalActor->SetOpacity(opValue/99.0);
    sagittalActor->SetOpacity(opValue/99.0);
    axialActor->SetOpacity(opValue/99.0);
    // Actors are added to the renderer
    mainRenderer->AddActor(coronalActor);
    mainRenderer->AddActor(sagittalActor);
    mainRenderer->AddActor(axialActor);
    // hide actors according to their appearance
    coronalActor->SetVisibility(enableCuts[0]);
    sagittalActor->SetVisibility(enableCuts[1]);
    axialActor->SetVisibility(enableCuts[2]);
    // An initial camera view is created.  The Dolly() method moves
    // the camera towards the FocalPoint, thereby enlarging the image.
    mainRenderer->SetActiveCamera(aCamera);
    mainRenderer->ResetCamera();
    aCamera->Dolly(1.5);
    /* Note that when camera movement occurs (as it does in the Dolly()
       method), the clipping planes often need adjusting. Clipping planes
       consist of two planes: near and far along the view direction. The
       near plane clips out objects in front of the plane; the far plane
       clips out objects behind the plane. This way only what is drawn
       between the planes is actually rendered.
     */
    mainRenderer->ResetCameraClippingRange ();

    //    QMenu* popupTest = new QMenu(VTKWidget);
    //    popupTest->addAction("Background White");
    //    popupTest->addAction("Background Black");
    //    popupTest->addAction("Stereo Rendering");

    //    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    //    vtkSmartPointer<vtkOrientationMarkerWidget> widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    //    widget->SetOrientationMarker( axes );
    //    widget->SetInteractor( VTKWidget->GetInteractor() );
    //    widget->SetViewport( 0.0, 0.0, 0.4, 0.4 );
    //    widget->SetEnabled( 1 );
    //    widget->InteractiveOff();
    //    mainRenderer->AddActor(axes);

    // Calling Render() directly on a vtkRenderer is strictly forbidden.
    // Only calling Render() on the vtkRenderWindow is a valid call.
    this->GetRenderWindow()->Render();
}

void MRIvtkWidget::changeCoronalSlice(int slice)
{
    if (selCorSlice == slice)
        return;
    selCorSlice = slice;
    coronalActor->SetDisplayExtent(selCorSlice,selCorSlice, 0,nColsImg-1, 0,nSlicesImg-1);
    this->GetRenderWindow()->Render();
}
void MRIvtkWidget::changeSagittalSlice(int slice)
{
    if (selSagSlice == slice)
        return;
    selSagSlice = slice;
    sagittalActor->SetDisplayExtent(0,nRowsImg-1, selSagSlice,selSagSlice, 0,nSlicesImg-1);
    this->GetRenderWindow()->Render();
}
void MRIvtkWidget::changeAxialSlice(int slice)
{
    if (selAxSlice == slice)
        return;
    selAxSlice = slice;
    axialActor->SetDisplayExtent(0,nRowsImg-1, 0,nColsImg-1, selAxSlice,selAxSlice);
    this->GetRenderWindow()->Render();
}

void MRIvtkWidget::changeSliceVisibility(ImageOrientation o,bool status)
{
    if (isEmpty())
        return;
    if (o == CORONAL)
        coronalActor->SetVisibility(status);
    else if (o == SAGITTAL)
        sagittalActor->SetVisibility(status);
    else if (o == AXIAL)
        axialActor->SetVisibility(status);
    this->update();
}

void MRIvtkWidget::updateOpacity(int value)
{    
    if (isEmpty())
        return;
    coronalActor->SetOpacity(value/99.0);
    sagittalActor->SetOpacity(value/99.0);
    axialActor->SetOpacity(value/99.0);
    this->GetRenderWindow()->Render();
}

void MRIvtkWidget::clearFibers()
{
    mainRenderer->RemoveActor(linesActor);
    linesActor = NULL;
    linesActor = vtkSmartPointer<vtkActor>::New();
    mainRenderer->RemoveActor2D(scalarBar);
    this->update();
}

void MRIvtkWidget::visualizeFibers(const FiberTracts &fibers,const QList<quint32> &selectedFibers,
                                   int skip,int colorMethod, int colorOrientation[3])
{

    uchar chosenColor[3] = {255,0,0};
    QVector3D tmp;
    // create points and colors
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetName("Colors");
    colors->SetNumberOfComponents(3);
    int x,y,z;
    // fill the points and colors variables
    quint64 idx;
    uchar *pixel;
    for (int i = 0;i<selectedFibers.size();i += skip) {
        idx = selectedFibers[i];
        for (int j = 0;j<fibers.getFiber(idx).size();j++) {
            points->InsertNextPoint(fibers.getFiber(idx).at(j).x(),
                                    fibers.getFiber(idx).at(j).y(),
                                    fibers.getFiber(idx).at(j).z());
            if(colorMethod==0) {        // directional
                if (j > 0 ) {
                    // the colorcode is defined as the difference vector
                    // between each two succesive points
                    tmp = (fibers.getFiber(idx).at(j) - fibers.getFiber(idx).at(j-1)).normalized();
                    chosenColor[colorOrientation[0]] = 255.0*qAbs(tmp.x()); // r
                    chosenColor[colorOrientation[1]] = 255.0*qAbs(tmp.y()); // g
                    chosenColor[colorOrientation[2]] = 255.0*qAbs(tmp.z()); // b
                    colors->InsertNextTupleValue(chosenColor);
                }
            }
            else if (colorMethod==1) {                      // solid
                // RGB
                chosenColor[0] = fibers.getColor(idx).red();
                chosenColor[1] = fibers.getColor(idx).green();
                chosenColor[2] = fibers.getColor(idx).blue();
                colors->InsertNextTupleValue(chosenColor);
            }
            else if (colorMethod==2 && !isEmpty()) {        // scalar weighted
                // red to yellow colormap vary from (255,0,0) to (255,255,0)
                x = qMax(qMin(qRound(fibers.getFiber(idx).at(j).x()),nRowsImg-1),0);
                y = qMax(qMin(qRound(fibers.getFiber(idx).at(j).y()),nColsImg-1),0);
                z = qMax(qMin(qRound(fibers.getFiber(idx).at(j).z()),nSlicesImg-1),0);
                pixel = static_cast<uchar*>(brain3D->GetScalarPointer(x,y,z));
                chosenColor[1] = 255.0 - pixel[0];            // map the red to the higher value
                colors->InsertNextTupleValue(chosenColor);
            }
        }
        // add last color tuple value in case of directional
        if (colorMethod==0)
            colors->InsertNextTupleValue(chosenColor);
    }
    // create lines
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    // Add the lines to the dataset
    int PtIndex = 0;
    for (int i = 0;i<selectedFibers.size(); i += skip) {
        idx = selectedFibers[i];
        lines->InsertNextCell(fibers.getFiber(idx).size());
        for (int j = 0;j<fibers.getFiber(idx).size();j++) {
            lines->InsertCellPoint(PtIndex);
            PtIndex++;
        }
    }

    // create polyData
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetLines(lines);
    polyData->GetPointData()->SetScalars(colors);

    vtkSmartPointer<vtkPolyDataMapper> LinesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    LinesMapper->SetInput(polyData);
    LinesMapper->SetScalarModeToUsePointFieldData();
    LinesMapper->SelectColorArray("Colors");

    mainRenderer->RemoveActor(linesActor);
    linesActor = NULL;
    linesActor = vtkSmartPointer<vtkActor>::New();
    float dx, dy, dz;
    fibers.getVoxelDimension(dx,dy,dz);
    linesActor->SetScale(dx,dy,dz);
    linesActor->SetMapper(LinesMapper);
    mainRenderer->AddActor(linesActor);

    // show a colorbar when the scalr mapping is used
    if (colorMethod==2 && !isEmpty()) {
        // create a red-yellow color table
        vtkSmartPointer<vtkLookupTable> hotLUT = vtkSmartPointer<vtkLookupTable>::New();
        hotLUT->SetTableRange (minVolumeValue, maxVolumeValue);
        hotLUT->SetHueRange(0.16,0);
        hotLUT->SetSaturationRange(1,1);
        hotLUT->SetValueRange(1,1);
        hotLUT->Build(); //effective built
        // create colorbar and control its size
        mainRenderer->RemoveActor2D(scalarBar);
        scalarBar = NULL;
        scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
        scalarBar->SetLookupTable(hotLUT);
        scalarBar->SetNumberOfLabels(2);
        scalarBar->SetMaximumWidthInPixels(50);
        scalarBar->SetMaximumHeightInPixels(250);
        scalarBar->SetDisplayPosition(this->width()-50,50);

        mainRenderer->AddActor2D(scalarBar);
    }
    else
        mainRenderer->RemoveActor2D(scalarBar);

    this->GetRenderWindow()->Render();

}

bool MRIvtkWidget::renderLabelVolume(const vec &labels,const QVector<QRgb> &cTable,
                                     MyProgressDialog *progress)
{
    if (labels[0] < 0 || labels[labels.n_elem-1] >= cTable.size())
        return false;
    progress->setLabelText("Processing ... ");
    progress->setRange(0,2*labels.n_elem);
    progress->setModal(true);
    progress->setFixedSize(progress->sizeHint());
    progress->show();

    // clear actor list
    // should be modified later as a function to remove actors from viewer
    labelsActors.clear();
    // the pipeline is following 3D Slicer ModelMaker plugin, it can be found at:
    // https://github.com/pieper/Slicer4/blob/master/Applications/CLI/ModelMaker.cxx
    // Apply Threshold To Cut Out Other Data
    vtkSmartPointer<vtkImageThreshold> threshold = vtkSmartPointer<vtkImageThreshold>::New();;
    threshold->SetOutValue(0);
    threshold->SetInput(brain3D);
    threshold->ReleaseDataFlagOn();
    vtkSmartPointer<vtkImageToStructuredPoints> imgToStrPnts = vtkSmartPointer<vtkImageToStructuredPoints>::New();
    imgToStrPnts->SetInputConnection(threshold->GetOutputPort());
    imgToStrPnts->ReleaseDataFlagOn();
    // create an isosurface, note that vtkMarchingCubes is better than
    // vtkContourFilter
    vtkSmartPointer<vtkMarchingCubes> mcubes = vtkSmartPointer<vtkMarchingCubes>::New();
    mcubes->SetInputConnection(imgToStrPnts->GetOutputPort());
    mcubes->ComputeNormalsOff();
    mcubes->ComputeGradientsOff();
    mcubes->ComputeScalarsOff();
    mcubes->GetOutput()->ReleaseDataFlagOn();
    // try to reduce the number of triangles
    vtkSmartPointer<vtkDecimatePro> decimator = vtkSmartPointer<vtkDecimatePro>::New();
    decimator->SetInputConnection(mcubes->GetOutputPort());
    decimator->SetFeatureAngle(60);
    decimator->SetMaximumError(1);
    decimator->SplittingOff();
    decimator->PreserveTopologyOn();
    decimator->SetTargetReduction(0.25);
    decimator->ReleaseDataFlagOn();
    // smooth the result of the iso vtkMarchingCubes
    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    smoother->SetInputConnection(decimator->GetOutputPort());
    smoother->SetNumberOfIterations(10);
    smoother->BoundarySmoothingOff();
    smoother->FeatureEdgeSmoothingOff();
    smoother->SetPassBand(0.1);
    smoother->GetOutput()->ReleaseDataFlagOn();
    // compute normals and point normals
    vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
    normals->SetInputConnection(smoother->GetOutputPort());
    normals->ComputePointNormalsOn(); // Compute Point Normals
    normals->SetFeatureAngle(60);
    normals->SetSplitting(true);      // Split Normals
    normals->ReleaseDataFlagOn();
    // stripper: create triangle out of polygons
    vtkSmartPointer<vtkStripper> stripper = vtkSmartPointer< vtkStripper >::New();
    stripper->SetInputConnection(normals->GetOutputPort());
    stripper->ReleaseDataFlagOn();
    // render isosurfaces and saves them
    QString rootDir = QDir::currentPath().append("/"), num;
    for (uint i = 0;i <labels.n_elem;i++) {
        // update the threshold according to the new label
        threshold->ThresholdBetween(labels(i),labels(i));
        mcubes->SetValue(0,labels(i));
        // try to write the data
        QString fileName = rootDir + num.setNum(i) + QString(".vtp");
        vtkSmartPointer< vtkPolyDataWriter > writer = vtkSmartPointer< vtkPolyDataWriter >::New();
        writer->SetInput(stripper->GetOutput());
        writer->SetFileName(fileName.toStdString().c_str());
        writer->Write();
        progress->setValue(i);
    }
    // render the labels
    progress->setLabelText("Rendering ... ");
    for (uint i = 0;i <labels.n_elem;i++) {
        vtkSmartPointer< vtkPolyDataReader > reader = vtkSmartPointer< vtkPolyDataReader >::New();
        QString fileName = rootDir + num.setNum(i) + QString(".vtp");
        reader->SetFileName(fileName.toStdString().c_str());
        // map read data
        vtkSmartPointer<vtkPolyDataMapper> isoMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        isoMapper->SetInput(reader->GetOutput());
        isoMapper->ScalarVisibilityOff();   // utilize actor's property I set
        isoMapper->ReleaseDataFlagOn();
        // create actor
        vtkSmartPointer<vtkActor> isoActor = vtkSmartPointer<vtkActor>::New();
        labelsActors.append(isoActor);
        labelsActors[i]->SetMapper(isoMapper);
        QColor c = cTable.at(labels(i));
        labelsActors[i]->GetProperty()->SetColor(c.redF(),c.greenF(),c.blueF());
        labelsActors[i]->GetProperty()->SetInterpolationToGouraud();
        labelsActors[i]->GetProperty()->ShadingOn();
        mainRenderer->AddActor(labelsActors[i]);
        this->GetRenderWindow()->Render();
        QFile::remove(fileName);
        progress->setValue(i+labels.n_elem);
    }
    return true;
}

void MRIvtkWidget::changeLabelColor(u32_vec idx,QColor c)
{
    if (!c.isValid())
        return;
    for (uint i =0; i<idx.size();i++)
        if (labelsActors.size() >= idx[i])
            labelsActors[idx[i]]->GetProperty()->SetColor(c.redF(),c.greenF(),c.blueF());
    this->update();
}

void MRIvtkWidget::updateLabelOpacity(u32_vec idx,int value)
{
    if (isLabelVolumeEmpty())
        return;
    for (uint i =0; i<idx.size();i++)
        if (labelsActors.size() >= idx[i])
            labelsActors[idx[i]]->GetProperty()->SetOpacity(value/100.0);
    this->update();
}

void MRIvtkWidget::changeLabelVisbility(u32_vec idx,bool status)
{
    if (isLabelVolumeEmpty())
        return;
    for (uint i =0; i<idx.size();i++)
        if (labelsActors.size() >= idx[i])
            labelsActors[idx[i]]->SetVisibility(status);
    this->update();
}

void MRIvtkWidget::clearLabelVolumes()
{
    if (isLabelVolumeEmpty())
        return;
    // remove actors
    for (int i =0; i<labelsActors.size();i++)
        mainRenderer->RemoveActor(labelsActors[i]);
    this->update();
    // clear variables, assuming it will delete the actors
    labelsActors.clear();
}
