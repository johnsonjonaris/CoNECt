#include "cvtkWidget.h"


CvtkWidget::CvtkWidget(QWidget *parent): QVTKWidget(parent)
{
    mainRenderer = vtkSmartPointer<vtkRenderer>::New();
    this->GetRenderWindow()->AddRenderer(mainRenderer);
    selected = false;
    xs = -1, ys = -1, edgeIdx = -1;
}

/**
  * Input:
  * cm:     connectome
  * nvp:    node visualization parameters
  * evp:    edge visualization parameters
  */
void CvtkWidget::visualizeConnectome(const Connectome &cm,
                                     const VisualizationParameters &nvp,
                                     const VisualizationParameters &evp)
{
    if (cm.NW.n_rows != cm.NW.n_cols || cm.NW.n_rows != cm.centroids.n_rows)
        return;
    mainRenderer->GetViewProps()->RemoveAllItems();
    mainRenderer->Clear();
    // visualize nodes and edges
//    visualizeBrain(cm);
    visualizeNodes(cm, nvp);
    updateNodesLabels(cm, nvp);
    visualizeEdges(cm, evp);
    // Set a background color for the renderer
    mainRenderer->SetBackground(.2, .3, .4);
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
    // An initial camera view is created.  The Dolly() method moves
    // the camera towards the FocalPoint, thereby enlarging the image.
    mainRenderer->SetActiveCamera(aCamera);
    mainRenderer->ResetCamera();
    aCamera->Dolly(1.5);

    mainRenderer->ResetCameraClippingRange();
    this->GetRenderWindow()->Render();
}

void CvtkWidget::visualizeBrain(const Connectome &cm)
{
    uint    r = cm.getHeader().nR,
            c = cm.getHeader().nC,
            s = cm.getHeader().nS;
    vtkSmartPointer<vtkImageData> brain3D = vtkSmartPointer<vtkImageData>::New();
    brain3D->SetDimensions(r,c,s);
    brain3D->SetNumberOfScalarComponents(1);
    brain3D->SetSpacing(cm.getHeader().dx,cm.getHeader().dy,cm.getHeader().dz);
    brain3D->SetOrigin(0,0,0);
    brain3D->SetScalarTypeToUnsignedChar();
    brain3D->AllocateScalars();
    uchar *pixel;
    for (int z = 0; z < s; ++z)
        for (int y = 0; y < c; ++y)
            for (int x = 0; x < r; ++x) {
                pixel = static_cast<uchar*>(brain3D->GetScalarPointer(x,y,z));
                pixel[0] = uchar( cm.getMask().at(x,y,z) );
            }

    vtkSmartPointer<vtkImageToStructuredPoints> imgToStrPnts =
            vtkSmartPointer<vtkImageToStructuredPoints>::New();
    imgToStrPnts->SetInput(brain3D);
    imgToStrPnts->ReleaseDataFlagOn();
    vtkSmartPointer<vtkMarchingCubes> mcubes = vtkSmartPointer<vtkMarchingCubes>::New();
    mcubes->SetInputConnection(imgToStrPnts->GetOutputPort());
    mcubes->SetValue(0,1);
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
    decimator->SetTargetReduction(0.2);
    decimator->ReleaseDataFlagOn();
    // smooth the result of the iso vtkMarchingCubes
    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother =
            vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
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
    // render isosurface
    vtkSmartPointer<vtkPolyDataMapper> isoMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    isoMapper->SetInput(stripper->GetOutput());
    isoMapper->ScalarVisibilityOff();   // utilize actor's property I set
    isoMapper->ReleaseDataFlagOn();
    // create actor
    brainActor = vtkSmartPointer<vtkActor>::New();
    brainActor->SetMapper(isoMapper);
    QColor cc = QColor(Qt::lightGray);
    brainActor->GetProperty()->SetColor(cc.redF(),cc.greenF(),cc.blueF());
    brainActor->GetProperty()->SetOpacity(0.2);
    mainRenderer->AddActor(brainActor);
    this->GetRenderWindow()->Render();
}

void CvtkWidget::visualizeNodes(const Connectome &cm,
                                const VisualizationParameters &nvp)
{
    // clear nodes if existing
    if (!nodeActorList.isEmpty()) {
        for (int i = 0; i< nodeActorList.size();++i)
            mainRenderer->RemoveActor(nodeActorList.at(i));
        nodeActorList.clear();
    }
    int nRegions = cm.NW.n_rows;
    float x=0,y=0,z=0;
    // create a bunch of balls
    double  dx = cm.header.dx,
            dy = cm.header.dy,
            dz = cm.header.dz;
    for (int i=0; i<nRegions;++i) {
        x = cm.centroids(i,0)*dx;
        y = cm.centroids(i,1)*dy;
        z = cm.centroids(i,2)*dz;
        vtkSmartPointer<vtkSphereSource> sp = vtkSmartPointer<vtkSphereSource>::New();
        sp->SetPhiResolution(10);
        sp->SetThetaResolution(20);
        sp->SetCenter(x,y,z);
        // control size
        double size = nvp.sScale;
        if (nvp.sType == EQUAL)
            size *= nvp.size;
        else
            size *= cm.localMetrics(nvp.sWeightIdx,i);
        sp->SetRadius(size);

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(sp->GetOutputPort());
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        // control color
        QColor c;
        if (nvp.cType == SAME)
            c = nvp.cList.first();
        else if (nvp.cType == MODULAR)
            if (nvp.cModularIdx == 0)
                c = nvp.cList[cm.localMetrics(9,i)];
        else
            c = (cm.localMetrics(nvp.cThresholdIdx,i)<nvp.cThreshold)?
                        nvp.cList.first():nvp.cList.last();
        actor->GetProperty()->SetColor(c.redF(),c.greenF(),c.blueF());
        if (selected && i == xs || i == ys) {
            actor->GetProperty()->SetAmbient(0.25);
            actor->GetProperty()->SetSpecular(0.25);
        }
        mainRenderer->AddActor(actor);
        nodeActorList.append(actor);
        // free memory
        sp = NULL;
        mapper = NULL;
    }
    this->GetRenderWindow()->Render();
}

void CvtkWidget::updateNodesColor(const Connectome &cm,
                                  const VisualizationParameters &nvp)
{
    // clear nodes if existing
    if (nodeActorList.isEmpty())
        return;
    int nRegions = cm.NW.n_rows;
    // create a bunch of balls
    for (int i=0; i<nRegions;++i) {
        // control color
        QColor c;
        if (nvp.cType == SAME)
            c = nvp.cList.first();
        else if (nvp.cType == MODULAR) {
            if (nvp.cModularIdx == 0)
                c = nvp.cList[cm.localMetrics(9,i)];
        }
        else
            c = (cm.localMetrics(nvp.cThresholdIdx,i)<nvp.cThreshold)?
                        nvp.cList.first():nvp.cList.last();
        nodeActorList[i]->GetProperty()->SetColor(c.redF(),c.greenF(),c.blueF());
        if (selected && i == xs || i == ys) {
            nodeActorList[i]->GetProperty()->SetAmbient(0.25);
            nodeActorList[i]->GetProperty()->SetSpecular(0.25);
        }
    }
    this->GetRenderWindow()->Render();
}

void CvtkWidget::updateNodesLabels(const Connectome &cm,
                                   const VisualizationParameters &nvp)
{
    if (!labelActorList.isEmpty()) {
        for (int i = 0; i< labelActorList.size();++i)
            mainRenderer->RemoveActor(labelActorList.at(i));
        labelActorList.clear();
    }
    if (nvp.dType == NONE)
        return;
    int nRegions = cm.NW.n_rows;
    float x=0,y=0,z=0;
    // create a bunch of balls
    double  dx = cm.header.dx,
            dy = cm.header.dy,
            dz = cm.header.dz;

    for (int i=0; i<nRegions;++i) {
        // prepare coordinates
        x = cm.centroids(i,0)*dx;
        y = cm.centroids(i,1)*dy;
        z = cm.centroids(i,2)*dz;
        double size = nvp.sScale;
        if (nvp.sType == EQUAL)
            size *= nvp.size;
        else
            size *= cm.localMetrics(nvp.sWeightIdx,i);
        // to plot or not
        if (nvp.dType == ALL ||
                (nvp.dType == THRESHOLDS &&
                 cm.localMetrics(nvp.dThresholdIdx,i) >= nvp.dThreshold )) {

            vtkSmartPointer<vtkVectorText> text = vtkSmartPointer<vtkVectorText>::New();
            text->SetText(cm.names.at(i).toLocal8Bit().data());
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(text->GetOutputPort());
            vtkSmartPointer<vtkActor> textActor = vtkSmartPointer<vtkActor>::New();
            textActor->SetPosition(x+size,y+size,z+size);
            textActor->SetMapper(mapper);
            textActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
            mainRenderer->AddActor(textActor);
            labelActorList.append(textActor);
            mapper = NULL;
            text = NULL;
        }
    }
    this->GetRenderWindow()->Render();
}

void CvtkWidget::visualizeEdges(const Connectome &cm,
                                const VisualizationParameters &evp)
{
    // clear edges if existing
    if (!edgeActorList.isEmpty()) {
        for (int i = 0; i< edgeActorList.size();++i)
            mainRenderer->RemoveActor(edgeActorList.at(i));
        edgeActorList.clear();
    }
    int nRegions = cm.NW.n_rows;
    displayedEdges = zeros<uchar_mat>(nRegions,nRegions);
    edgeIdx = -1;
    if (evp.dType == NONE)
        return;
    float x=0,y=0,z=0, x2=0,y2=0,z2=0;
    double  dx = cm.header.dx,
            dy = cm.header.dy,
            dz = cm.header.dz;
    // create lines
    for (int i=0; i<nRegions;++i)
        for (int j=i+1;j<nRegions;++j) {
            if (evp.dType == THRESHOLDS) {
                if (evp.dThresholdIdx == 0 && cm.NW(i,j)<evp.dThreshold)
                    continue;
                else if (evp.dThresholdIdx == 1 && cm.edgeBetweeness(i,j)<evp.dThreshold)
                    continue;
            }
            if (cm.NW(i,j) != 0) {
                x = cm.centroids(i,0)*dx;
                y = cm.centroids(i,1)*dy;
                z = cm.centroids(i,2)*dz;

                x2 = cm.centroids(j,0)*dx;
                y2 = cm.centroids(j,1)*dy;
                z2 = cm.centroids(j,2)*dz;

                vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
                lineSource->SetPoint1(x,y,z);
                lineSource->SetPoint2(x2,y2,z2);

                // Create a tube (cylinder) around the line
                vtkSmartPointer<vtkTubeFilter> tubeFilter = vtkSmartPointer<vtkTubeFilter>::New();
                tubeFilter->SetInputConnection(lineSource->GetOutputPort());
                // control size
                double size = evp.sScale;
                if (evp.sType == EQUAL)
                    size *= evp.size;
                else if (evp.sWeightIdx == 0)
                    size *= cm.NW(i,j);
                else if (evp.sWeightIdx == 1)
                    size *= cm.getEdgeBetweeness().at(i,j);
                tubeFilter->SetRadius(size); //default is .1
                tubeFilter->SetNumberOfSides(50);
                // Create a mapper and actor
                vtkSmartPointer<vtkPolyDataMapper> tubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                tubeMapper->SetInputConnection(tubeFilter->GetOutputPort());
                vtkSmartPointer<vtkActor> tubeActor = vtkSmartPointer<vtkActor>::New();
                tubeActor->SetMapper(tubeMapper);
                QColor c;
                if (evp.cType == SAME)
                    c = evp.cList.first();
                else if (evp.cType == MODULAR)
                    if (evp.cModularIdx == 0)
                        c = evp.cList[cm.localMetrics(9,i)];
                else
                    c = (cm.localMetrics(evp.cThresholdIdx,i)<evp.cThreshold)?
                                evp.cList.first():evp.cList.last();
                tubeActor->GetProperty()->SetColor(c.redF(),c.greenF(),c.blueF());
                if (i == xs && j == ys  && selected) {
                    tubeActor->GetProperty()->SetAmbient(0.25);
                    tubeActor->GetProperty()->SetSpecular(0.25);
                    // retain index
                    edgeIdx = edgeActorList.size();
                }
                mainRenderer->AddActor(tubeActor);
                edgeActorList.append(tubeActor);
                // mark it as visualized
                displayedEdges(i,j) = 1;
            }
        }
    this->GetRenderWindow()->Render();
}

void CvtkWidget::updateEdgesColor(const Connectome &cm,
                                  const VisualizationParameters &evp)
{
    // clear edges if existing
    if (edgeActorList.isEmpty())
        return;
    int nRegions = cm.NW.n_rows;
    // create lines
    int cc = 0;
    for (int i=0; i<nRegions;++i)
        for (int j=i+1;j<nRegions;++j) {
            if (evp.dType == THRESHOLDS) {
                if (evp.dThresholdIdx == 0 && cm.NW(i,j)<evp.dThreshold)
                    continue;
                else if (evp.dThresholdIdx == 1 && cm.edgeBetweeness(i,j)<evp.dThreshold)
                    continue;
            }
            if (cm.NW(i,j) != 0) {
                QColor c;
                if (evp.cType == SAME)
                    c = evp.cList.first();
                else if (evp.cType == MODULAR)
                    if (evp.cModularIdx == 0)
                        c = evp.cList[cm.localMetrics(9,i)];
                else
                    c = (cm.localMetrics(evp.cThresholdIdx,i)<evp.cThreshold)?
                                evp.cList.first():evp.cList.last();
                edgeActorList[cc++]->GetProperty()->SetColor(c.redF(),c.greenF(),c.blueF());
                if (i == xs && j == ys && selected) {
                    edgeActorList[cc]->GetProperty()->SetAmbient(0.25);
                    edgeActorList[cc]->GetProperty()->SetSpecular(0.25);
                }
            }
        }
    this->GetRenderWindow()->Render();
}

/**
  * xx and yy are entries in the nodeActorList.
  * for a single node, yy is set to default = -1.
  * for an edge, both xx and yy are needed.
  */
void CvtkWidget::select(int xx, int yy)
{
    if (nodeActorList.isEmpty())
        return;
    // deselect old sphere
    deselect();
    selected = true;
    xs = qMin(xx,yy), ys = qMax(xx,yy);
    if (xs>-1) {
        nodeActorList[xs]->GetProperty()->SetAmbient(0.25);
        nodeActorList[xs]->GetProperty()->SetSpecular(0.25);
    }
    if (ys>-1) {
        nodeActorList[ys]->GetProperty()->SetAmbient(0.25);
        nodeActorList[ys]->GetProperty()->SetSpecular(0.25);
    }
    if (xs>-1 && ys>-1 && displayedEdges(xs,ys) == 1) {
        edgeIdx = -1;
        int n = displayedEdges.n_rows;
        // ironiclly, for loop is faster than an algorithm here
        for (int i =0;i<n;++i) {
            int j=i+1;
            for (;j<n;++j) {
                if (displayedEdges(i,j)) ++edgeIdx;
                if (xs == i && ys == j) break;
            }
            if (xs == i && ys == j) break;
        }
        edgeActorList[edgeIdx]->GetProperty()->SetAmbient(0.25);
        edgeActorList[edgeIdx]->GetProperty()->SetSpecular(0.25);

    }
    this->GetRenderWindow()->Render();
}

void CvtkWidget::deselect()
{
    if (nodeActorList.isEmpty() || !selected)
        return;
    // deselect old sphere
    if (xs>-1) {
        nodeActorList[xs]->GetProperty()->SetAmbient(0);
        nodeActorList[xs]->GetProperty()->SetSpecular(0);
    }
    if (ys>-1) {
        nodeActorList[ys]->GetProperty()->SetAmbient(0);
        nodeActorList[ys]->GetProperty()->SetSpecular(0);
    }
    if (edgeIdx > -1) {//xs>-1 && ys>-1 && displayedEdges(xs,ys) == 1
        edgeActorList[edgeIdx]->GetProperty()->SetAmbient(0);
        edgeActorList[edgeIdx]->GetProperty()->SetSpecular(0);
    }
    this->GetRenderWindow()->Render();
    selected = false;
    xs = -1, ys = -1, edgeIdx = -1;
}
// manipulate brain mask
void CvtkWidget::updateBrainMaskOpacity(int value)
{
    if (brainActor == NULL)
        return;
    brainActor->GetProperty()->SetOpacity(value/99.0);
    this->GetRenderWindow()->Render();
}

void CvtkWidget::updateBrainMaskVisibility(bool status)
{
    if (brainActor == NULL)
        return;
    brainActor->SetVisibility(status);
    this->GetRenderWindow()->Render();
}

void CvtkWidget::clear()
{
    // clear 3D View
    QColor bg = this->parentWidget()->palette().window().color().rgb();
    mainRenderer->SetBackground(bg.redF(),bg.greenF(),bg.blueF());
    mainRenderer->GetViewProps()->RemoveAllItems();
    mainRenderer->Clear();
    mainRenderer->ResetCamera();
    this->update();
    nodeActorList.clear();
    edgeActorList.clear();
    displayedEdges.clear();
    brainActor = NULL;
}

