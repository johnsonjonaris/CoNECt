#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    createCustomWidgets();
    createToolBar();
    createStatusBar();
    create3D();
    connectSignals();
    initalize();
}

void MainWindow::createCustomWidgets()
{
    // add custom Views
    // add custom diffusion widgets
    VectorsView = new View(VectorFrame);
    verticalLayout_19->addWidget(VectorsView);

    CoronalView = new View(CorFrame);
    verticalLayout_16->addWidget(CoronalView);
    CoronalView->setStyleSheet("border: 2px solid red");
    SagittalView = new View(SagFrame);
    verticalLayout_17->addWidget(SagittalView);
    SagittalView->setStyleSheet("border: 2px solid green");
    AxialView = new View(AxFrame);
    verticalLayout_18->addWidget(AxialView);
    AxialView->setStyleSheet("border: 2px solid blue");
    // add custom connectome controls
    CnctView = new ConnectomeView(ConnectomeFrame);
    horizontalLayout_11->addWidget(CnctView);

    // add custom controls
    // add two sided slider adapted from Qxt
    FiberLengthSlider = new QxtSpanSlider(Qt::Horizontal,FiberManagementBox);
    FiberLengthSlider->setSpan(0,1);
    FiberLengthSlider->setHandleMovementMode(QxtSpanSlider::NoOverlapping);
    LeftSliderLabel = new QLabel("",FiberManagementBox);
    RightSliderLabel = new QLabel("",FiberManagementBox);
    RightSliderLabel->setAlignment(Qt::AlignRight);
    SliderHBoxLayout = new QHBoxLayout();
    SliderHBoxLayout->setSpacing(6);
    SliderHBoxLayout->addWidget(LeftSliderLabel);
    SliderHBoxLayout->addWidget(RightSliderLabel);
    verticalLayout->addLayout(SliderHBoxLayout);
    verticalLayout->addWidget(FiberLengthSlider);
    // add the labels list
    LabelsList = new TableTree(LabelsControlBox);
    LabelsList->setColumnCount(5);
    LabelsList->header()->setVisible(false);
    LabelsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    LabelsList->setEditableColumns(QVector<quint16>()<<2);
    verticalLayout_9->addWidget(LabelsList);
}
void MainWindow::createStatusBar()
{
    StatusBars = new QStatusBar(this);
    StatusBars->setMaximumHeight(20);
    this->setStatusBar(StatusBars);
    //set min/max sizes
    LocationLabel = new QLabel("X:9999, Y:9999, Z:9999 = (10000: AAAAAAAAAAAAAAAAAAAAAAAAAAAAA)");
    LocationLabel->setAlignment(Qt::AlignLeft);
    LocationLabel->setMinimumSize(LocationLabel->sizeHint());
    LocationLabel->setText("");
    LocationLabel->setIndent(3);

    FileInfo = new QLabel("Threshold: 0.075, TOD res: 2.2 X 2.2 X 2.2");
    FileInfo->setAlignment(Qt::AlignLeft);
    FileInfo->setMinimumSize(FileInfo->sizeHint());
    FileInfo->setText("");

    ActVolInfo = new QLabel("Active Vol res: 2.2 X 2.2 X 2.2");
    ActVolInfo->setAlignment(Qt::AlignLeft);
    ActVolInfo->setMinimumSize(FileInfo->sizeHint());
    ActVolInfo->setText("");

    StatusBars->addWidget(LocationLabel);
    StatusBars->addWidget(FileInfo,1);
    StatusBars->addWidget(ActVolInfo,1);
}
void MainWindow::createToolBar()
{
    MainToolBar = new QToolBar(this);
    this->addToolBar(Qt::TopToolBarArea, MainToolBar);
    MainToolBar->setIconSize(QSize(50,50));
    MainToolBar->addAction(actionWizard);
    MainToolBar->addSeparator();
    MainToolBar->addAction(actionLoadModelFile);
    MainToolBar->addAction(actionLoadImage);
    MainToolBar->addAction(actionLoadFiber);
    MainToolBar->addAction(actionLoadConnectome);
    MainToolBar->addSeparator();
    MainToolBar->addAction(actionGenerateFibers);
    MainToolBar->addAction(actionGenerateConnectome);
    MainToolBar->addSeparator();
    MainToolBar->addAction(actionCoronalView);
    MainToolBar->addAction(actionSagitalView);
    MainToolBar->addAction(actionAxialView);
    MainToolBar->addAction(actionVectorView);
    MainToolBar->addAction(action3DView);
    MainToolBar->addAction(actionResetViews);
    MainToolBar->addSeparator();
}
void MainWindow::connectSignals()
{
    // connect actions
    connect(actionWizard,SIGNAL(triggered()),this,SLOT(onWizardButtonPress()));
    connect(actionLoadModelFile,SIGNAL(triggered()),this,SLOT(openModel()));
    connect(actionLoadImage,SIGNAL(triggered()),this,SLOT(openImage()));
    connect(actionLoadFiber,SIGNAL(triggered()),this,SLOT(openFiber()));
    connect(actionCloseAll,SIGNAL(triggered()),this,SLOT(closeAll()));
    connect(actionExit,SIGNAL(triggered()),this,SLOT(close()));
    // connect views actions
    connect(actionCoronalView,SIGNAL(triggered()),this,SLOT(showCoronalOnly()));
    connect(actionSagitalView,SIGNAL(triggered()),this,SLOT(showSagittalOnly()));
    connect(actionAxialView,SIGNAL(triggered()),this,SLOT(showAxialOnly()));
    connect(actionVectorView,SIGNAL(triggered()),this,SLOT(showVectorOnly()));
    connect(action3DView,SIGNAL(triggered()),this,SLOT(show3DOnly()));
    connect(actionResetViews,SIGNAL(triggered()),this,SLOT(resetViews()));
    // connect Help
    connect(actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    // connect change tab action and splitter motion
    connect(DisplayTab,SIGNAL(currentChanged(int)),this,SLOT(tab1ChangeAction(int)));
    connect(ControlTab,SIGNAL(currentChanged(int)),this,SLOT(tab2ChangeAction(int)));
    connect(splitterH,SIGNAL(splitterMoved(int,int)),this,SLOT(resizeWhenSizeChanged()));
    connect(splitterV1,SIGNAL(splitterMoved(int,int)),this,SLOT(resizeWhenSizeChanged()));
    connect(splitterV2,SIGNAL(splitterMoved(int,int)),this,SLOT(resizeWhenSizeChanged()));
    // connect mouse motion events in first tab to their handlers
    connect(VectorsView,SIGNAL(mouseMoved(QMouseEvent*)),
            this,SLOT(handleMouseMoveVector(QMouseEvent*)));
    connect(CoronalView,SIGNAL(mouseMoved(QMouseEvent*)),
            this,SLOT(handleMouseMoveCoronal(QMouseEvent*)));
    connect(SagittalView,SIGNAL(mouseMoved(QMouseEvent*)),
            this,SLOT(handleMouseMoveSagittal(QMouseEvent*)));
    connect(AxialView,SIGNAL(mouseMoved(QMouseEvent*)),
            this,SLOT(handleMouseMoveAxial(QMouseEvent*)));
    // connect(CoronalView,SIGNAL(keyPressed(QKeyEvent*)),this,SLOT(handleKeyPressCoronal(QKeyEvent*)));
    // connect mouse press events in first tab to their handles
    connect(CoronalView,SIGNAL(mouseReleased(QMouseEvent*)),
            this,SLOT(handleMousePressCoronal(QMouseEvent*)));
    connect(SagittalView,SIGNAL(mouseReleased(QMouseEvent*)),
            this,SLOT(handleMousePressSagittal(QMouseEvent*)));
    connect(AxialView,SIGNAL(mouseReleased(QMouseEvent*)),
            this,SLOT(handleMousePressAxial(QMouseEvent*)));
    // connect mouse press/release and paint to their handlers
    connect(VectorsView,SIGNAL(mousePressed(QMouseEvent*)),
            this,SLOT(handleMousePressVector(QMouseEvent*)));
    connect(VectorsView,SIGNAL(mouseReleased(QMouseEvent*)),
            this,SLOT(handleMouseReleaseVector(QMouseEvent*)));
    connect(VectorsView,SIGNAL(paintEv(QPaintEvent*)),
            this,SLOT(handlePaintEventVector(QPaintEvent*)));
    // cut direction change in the vector view
    connect(CoronalVectorButton,SIGNAL(clicked()),this,SLOT(changeCutDirection()));
    connect(SagitalVectorButton,SIGNAL(clicked()),this,SLOT(changeCutDirection()));
    connect(AxialVectorButton,SIGNAL(clicked()),this,SLOT(changeCutDirection()));
    connect(VectorSliceSpinner,SIGNAL(valueChanged(int)),this,SLOT(updateMainSlice(int)));
    // gradient table actions
    connect(Normal, SIGNAL(clicked(bool)), this, SLOT(updateNormal(bool)));
    connect(InvertX, SIGNAL(clicked(bool)), this, SLOT(updateInvertX()));
    connect(InvertY, SIGNAL(clicked(bool)), this, SLOT(updateInvertY()));
    connect(InvertZ, SIGNAL(clicked(bool)), this, SLOT(updateInvertZ()));
    connect(SwapXY,SIGNAL(clicked(bool)),this,SLOT(updateSwapXY()));
    connect(SwapXZ,SIGNAL(clicked(bool)),this,SLOT(updateSwapXZ()));
    connect(SwapYZ,SIGNAL(clicked(bool)),this,SLOT(updateSwapYZ()));
    connect(ShowTableButton,SIGNAL(clicked()),this,SLOT(showTable()));
    // connect Image controls
    connect(ImageComboBox,SIGNAL(activated(int)),this,SLOT(changeImage(int)));
    connect(CoronalSpinner,SIGNAL(valueChanged(int)),this,SLOT(onCoronalSpinnerChange(int)));
    connect(SagittalSpinner,SIGNAL(valueChanged(int)),this,SLOT(onSagittalSpinnerChange(int)));
    connect(AxialSpinner,SIGNAL(valueChanged(int)),this,SLOT(onAxialSpinnerChange(int)));
    connect(OpacitySlider,SIGNAL(valueChanged(int)),this,SLOT(updateOpacity(int)));
    connect(CoronalBox,SIGNAL(clicked(bool)),this,SLOT(enable3DCoronal(bool)));
    connect(SagittalBox,SIGNAL(clicked(bool)),this,SLOT(enable3DSagittal(bool)));
    connect(AxialBox,SIGNAL(clicked(bool)),this,SLOT(enable3DAxial(bool)));
    connect(OpenImgBut,SIGNAL(clicked()),this,SLOT(openImage()));
    connect(CloseImgBut,SIGNAL(clicked()),this,SLOT(closeImage()));
    connect(SaveImgBut,SIGNAL(clicked()),this,SLOT(saveImage()));
    // connect zoom buttons
    connect(ZoomInButton,SIGNAL(toggled(bool)),this,SLOT(zoomInPressed(bool)));
    connect(ZoomOutButton,SIGNAL(toggled(bool)),this,SLOT(zoomOutPressed(bool)));
    // LUT change
    connect(LUT_Box,SIGNAL(currentIndexChanged(int)),this,SLOT(changeLUT(int)));
    connect(Open_LUT_Button,SIGNAL(clicked()),this,SLOT(openLUT()));
    connect(Delete_LUT_Button,SIGNAL(clicked()),this,SLOT(deleteLUT()));
    // tractography
    connect(actionGenerateFibers,SIGNAL(triggered()),this,SLOT(generateFibers()));
    connect(DisplayNone,SIGNAL(clicked()),this,SLOT(changeFiberDisplayState()));
    connect(DisplayAll,SIGNAL(clicked()),this,SLOT(changeFiberDisplayState()));
    connect(DisplaySelected,SIGNAL(clicked()),this,SLOT(changeFiberDisplayState()));
    connect(SaveFibersButton,SIGNAL(clicked()),this,SLOT(saveFiber()));
    connect(ClearFibersButton,SIGNAL(clicked()),this,SLOT(clearFibers()));
    // connect ROI drawing buttons
    connect(FreeHand,SIGNAL(toggled(bool)),this,SLOT(freehandROIPressed(bool)));
    connect(OvalROI,SIGNAL(toggled(bool)),this,SLOT(ovalROIPressed(bool)));
    connect(RectROI,SIGNAL(toggled(bool)),this,SLOT(rectROIPressed(bool)));
    connect(ClearROI_Button,SIGNAL(clicked()),this,SLOT(clearROIsPressed()));
    connect(RecolorFibersButton,SIGNAL(clicked()),this,SLOT(fiberColorPalettePressed()));
    connect(UndoROI_Button,SIGNAL(clicked()),this,SLOT(undoLastROI()));
    connect(Add_VOI_LabelButton,SIGNAL(clicked()),this,SLOT(onAdd_VOI_LabelButtonPress()));
    // fiber visualization
    connect(ColorMethod,SIGNAL(currentIndexChanged(int)),this,SLOT(colorMethodChanged(int)));
    connect(FiberColorOrient,SIGNAL(currentIndexChanged(int)),this,SLOT(fiberColorOrientChanged(int)));
    connect(SkipSlider,SIGNAL(valueChanged(int)),this,SLOT(skipFiberSliderMoving(int)));
    connect(SkipSlider,SIGNAL(sliderReleased()),this,SLOT(skipFiberSliderChanged()));
    // fiber management
    connect(StatsButton,SIGNAL(clicked()),this,SLOT(fiberStats()));
    connect(FiberLengthSlider,SIGNAL(lowerValueChanged(int)),this,SLOT(lengthSliderMoved()));
    connect(FiberLengthSlider,SIGNAL(upperValueChanged(int)),this,SLOT(lengthSliderMoved()));
    connect(FiberLengthSlider,SIGNAL(sliderReleased()),this,SLOT(lengthSliderChanged()));
    // labels
    connect(RenderButton,SIGNAL(clicked()),this,SLOT(renderButPressed()));
    connect(ClearLabelsButton,SIGNAL(clicked()),this,SLOT(clearLabelsButPress()));
    connect(LabelsList,SIGNAL(itemSelectionChanged()),this,SLOT(labelItemPressed()));
    connect(LabelsList,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(labelItemDoubleClicked(QTreeWidgetItem*,int)));
    connect(VisibilityStatusCheck,SIGNAL(clicked(bool)),this,SLOT(changeLabelVisbility(bool)));
    connect(LabelColorButton,SIGNAL(clicked()),this,SLOT(changeLabelColor()));
    connect(LabelOpacitySpinner,SIGNAL(valueChanged(int)),this,SLOT(changeLabelOpacity(int)));
    // connectome stuff
    connect(actionLoadConnectome,SIGNAL(triggered()),this,SLOT(openConnectome()));
    connect(CnctView,SIGNAL(mouseReleased(QMouseEvent*)),
            this,SLOT(handleMousePressConnectome(QMouseEvent*)));
    connect(CnctView,SIGNAL(mouseMoved(QMouseEvent*)),
            this,SLOT(handleMouseMoveConnectome(QMouseEvent*)));
    // Nodes visualization
    connect(NoLabelCheck,SIGNAL(clicked()),this,SLOT(onNodeLabelChange()));
    connect(AllLabelCheck,SIGNAL(clicked()),this,SLOT(onNodeLabelChange()));
    connect(LabelThresholdCheck,SIGNAL(clicked()),this,SLOT(onNodeLabelThresholdMetricChange()));
    connect(LabelThresholdSpinner,SIGNAL(valueChanged(double)),this,SLOT(onNodeLabelChange()));
    connect(LabelThresholdBox,SIGNAL(activated(int)),this,SLOT(onNodeLabelThresholdMetricChange()));

    connect(EqualNodeSizeCheck,SIGNAL(clicked()),this,SLOT(nodeAutoSizeScale()));
    connect(WeightedNodeSizeCheck,SIGNAL(clicked()),this,SLOT(nodeAutoSizeScale()));
    connect(EqualNodeSizeSpinner,SIGNAL(valueChanged(double)),this,SLOT(onNodeSizeChange()));
    connect(ScaleNodeSizeSpinner,SIGNAL(valueChanged(double)),this,SLOT(onNodeSizeChange()));
    connect(WeightedNodeSizeBox,SIGNAL(activated(int)),this,SLOT(nodeAutoSizeScale()));

    connect(SameNodeColorCheck,SIGNAL(clicked()),this,SLOT(onNodeColorChange()));
    connect(ModularNodeColorCheck,SIGNAL(clicked()),this,SLOT(onNodeColorChange()));
    connect(ModularNodeColorButton,SIGNAL(clicked()),this,SLOT(onNodeModularColorButtonPress()));
    connect(ThresholdNodeColorCheck,SIGNAL(clicked()),this,SLOT(onNodeColorThresholdMetricChange()));
    connect(ThresholdNodeColorBeforeSpinner,SIGNAL(valueChanged(double)),
            this,SLOT(onNodeColorChange()));
    connect(SameNodeColorButton,SIGNAL(clicked()),this,SLOT(onNodeSameColorButtonPress()));
    connect(ThresholdNodeColorBeforeButton,SIGNAL(clicked()),
            this,SLOT(onNodeThresholdBeforeColorButtonPress()));
    connect(ThresholdNodeColorAfterButton,SIGNAL(clicked()),
            this,SLOT(onNodeThresholdAfterColorButtonPress()));
    connect(ThresholdNodeColorBox,SIGNAL(activated(int)),
            this,SLOT(onNodeColorThresholdMetricChange()));

    // Edge visualization
    connect(AllEdgeCheck,SIGNAL(clicked()),this,SLOT(onEdgeDisplayChange()));
    connect(NoEdgeCheck,SIGNAL(clicked()),this,SLOT(onEdgeDisplayChange()));
    connect(ThresholdEdgeCheck,SIGNAL(clicked()),this,SLOT(onEdgeDisplayThresholdMetricChange()));
    connect(ThresholdEdgeSpinner,SIGNAL(valueChanged(double)),this,SLOT(onEdgeDisplayChange()));
    connect(ThresholdEdgeBox,SIGNAL(activated(int)),this,SLOT(onEdgeDisplayThresholdMetricChange()));

    connect(EqualEdgeSizeCheck,SIGNAL(clicked()),this,SLOT(edgeAutoSizeScale()));
    connect(WeightedEdgeSizeCheck,SIGNAL(clicked()),this,SLOT(edgeAutoSizeScale()));
    connect(EqualEdgeSizeSpinner,SIGNAL(valueChanged(double)),this,SLOT(onEdgeSizeChange()));
    connect(ScaleEdgeSizeSpinner,SIGNAL(valueChanged(double)),this,SLOT(onEdgeSizeChange()));
    connect(WeightedEdgeSizeBox,SIGNAL(activated(int)),this,SLOT(edgeAutoSizeScale()));

    connect(SameEdgeColorCheck,SIGNAL(clicked()),this,SLOT(onEdgeColorChange()));
    connect(ModularEdgeColorCheck,SIGNAL(clicked()),this,SLOT(onEdgeColorChange()));
    connect(ModularEdgeColorButton,SIGNAL(clicked()),this,SLOT(onEdgeModularColorButtonPress()));
    connect(ThresholdEdgeColorCheck,SIGNAL(clicked()),this,SLOT(onEdgeColorThresholdMetricChange()));
    connect(ThresholdEdgeColorBeforeSpinner,SIGNAL(valueChanged(double)),
            this,SLOT(onEdgeColorChange()));
    connect(SameEdgeColorButton,SIGNAL(clicked()),this,SLOT(onEdgeSameColorButtonPress()));
    connect(ThresholdEdgeColorBeforeButton,SIGNAL(clicked()),
            this,SLOT(onEdgeThresholdBeforeColorButtonPress()));
    connect(ThresholdEdgeColorAfterButton,SIGNAL(clicked()),
            this,SLOT(onEdgeThresholdAfterColorButtonPress()));
    connect(ThresholdEdgeColorBox,SIGNAL(activated(int)),
            this,SLOT(onEdgeColorThresholdMetricChange()));

    connect(DisplayMetricsButton,SIGNAL(clicked()),this,SLOT(displayMetrics()));
    connect(SaveMetricsButton,SIGNAL(clicked()),this,SLOT(onSaveMetricsButtonPress()));
    connect(ConnectomeMaskCheck,SIGNAL(clicked(bool)),
            this,SLOT(onConnectomeMaskVisibilityChange(bool)));
    connect(ConnectomeMaskOpacitySlider,SIGNAL(valueChanged(int)),
            this,SLOT(onConnectomeMaskOpacityChange(int)));
    connect(actionGenerateConnectome,SIGNAL(triggered()),this,SLOT(generateConnectome()));
}
void MainWindow::create3D()
{
    VTKWidget = new MRIvtkWidget(ThreeD);
    verticalLayout_38->addWidget(VTKWidget);

    CVTKWidget = new CvtkWidget(Conectome3D);
    verticalLayout_13->addWidget(CVTKWidget);
}
void MainWindow::initializeLUT()
{
    // clear any loaded LUT
    cTable.clear();
    cTables.clear();
    cTablesLabels.clear();
    // grayscale
    for ( int i = 0; i < 256; i++ )
        cTable.append(qRgb( i, i, i ));
    cTables.append(cTable);
    cTablesLabels.append(QStringList()<<" ");
    // FreeSurfer
    readLUT(QDir::currentPath()+"/FS_LUT.txt");
    allowLUTupdate = true;
}
void MainWindow::initalize()
{
    // reset menu options
    actionLoadModelFile->setEnabled(true);
    actionLoadImage->setEnabled(true);
    actionLoadFiber->setEnabled(false);
    actionCloseAll->setEnabled(false);
    actionGenerateFibers->setEnabled(false);
    actionGenerateConnectome->setEnabled(false);
    // initialize GUI
    DisplayTab->setCurrentIndex(0);     // choose first tab
    ControlTab->setCurrentIndex(0);     // choose first tab
    // Vector
    resetVectorControls();
    enableVectorControls(false);
    allowMainUpdate = true;
    // Image
    resetImageControls();
    enableImageControls(false);
    // LUT
    Load_LUT_Box->setEnabled(false);
    enableLabelControls(false);
    // fiber
    resetFiberControls();
    enableFiberControls(false);
    resetSize = true;
    // Connectome
    resetConnectomeControls();
    enableConnectomeControls(false);
}

void MainWindow::resetImageControls()
{
    // initialize volumes info matrix
    loadedImgsInfo.set_size(0,10);
    // set status bar stuff
    LocationLabel->setText("");
    ActVolInfo->setText("");
    // activeVol Box
    ImageComboBox->clear();
    CoronalBox->setChecked(true);
    AxialBox->setChecked(true);
    SagittalBox->setChecked(true);
    allowCorUpdate = true;
    allowSagUpdate = true;
    allowAxUpdate = true;
    allowSychronize = true;
    CoronalSpinner->clear();
    SagittalSpinner->clear();
    AxialSpinner->clear();
    OpacitySlider->setValue(99);
    UncheckZoomButton->setChecked(true);
    // 2D View
    CoronalView->scene()->clear();
    SagittalView->scene()->clear();
    AxialView->scene()->clear();
    cTable.clear();
    // initialize 3D
    VTKWidget->clear();
    // initialize LUT
    initializeLUT();
    allowLUTupdate = false;
    LUT_Box->setCurrentIndex(0);
    allowLUTupdate = true;
    // assure the LUT menu have only two options
    if (LUT_Box->count() > 1)
        for (int i = LUT_Box->count()-1;i>1;i--)
            LUT_Box->removeItem(i);
    // clear variables
    activeVol.clear();
    activeImgIsLabel = false;
    // reset image imfo
    nRowsImg = nColsImg = nSlicesImg = nVoxImg = 0;
    minActVol = maxActVol = 0;
    dxImg = dyImg = dzImg = dxByDyImg = dzByDxImg = dzByDyImg = 0.0;
    // no img files loaded by type
    nLoadedUchar = nLoadedShort = nLoadedInt = 0;
    nLoadedFloat = nLoadedDouble = nLoadedImg = 0;
    currentImage = -1;
}
void MainWindow::resetVectorControls()
{
    // reset image
    // clear variables
    dm.clear();
    Rx.clear();
    Ry.clear();
    Rz.clear();
    // set status bar stuff
    LocationLabel->setText("");
    FileInfo->setText("");
    // vector box
    CoronalVectorButton->setChecked(true);
    allowMainUpdate = false;
    VectorSliceSpinner->setValue(0);
    allowMainUpdate = true;
    VectorSliceSpinner->clear();
    // gradient box
    InvertX->setChecked(false);
    InvertY->setChecked(false);
    InvertZ->setChecked(false);
    Normal->setChecked(true);
    // 2D View
    VectorsView->scene()->clear();

}
void MainWindow::resetFiberControls()
{
    // clear visualization
    VTKWidget->clearFibers();
    // set display to no fibers
    DisplayNone->setChecked(true);
    // fiber ROI
    // clear variables
    freeHandROIs.clear();
    ovalROIs.clear();
    rectROIs.clear();
    ROIorder.clear();
    filteringActions.clear();
    ROI_Info.set_size(0,4);
    while (!ROIs.isEmpty())
        delete ROIs.takeLast();
    ROIs.clear();
    // initialize ROI
    freehandROI_IsShown     = false;
    ovalROI_IsShown         = false;
    rectROI_IsShown         = false;
    UncheckROI_Button->setChecked(true);
    AND->setChecked(true);
    UndoROI_Button->setEnabled(false);
    // fiber color
    fiberColor = Qt::red;   
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,fiberColor);
    RecolorFibersButton->setPalette(pal);
    RecolorFibersButton->setEnabled(false);
    // clear variables
    fibers.clear();
    selectedFibersList.clear();
    currentSelectedFibers.clear();
    // reset length slider
    FiberLengthSlider->setSpan(0,100);
    FiberLengthSlider->setLowerValue(0);
    FiberLengthSlider->setUpperValue(1);
    LeftSliderLabel->setText("");
    RightSliderLabel->setText("");
    // coloring and filtering
    ColorMethod->setCurrentIndex(0);
    FiberColorOrient->setCurrentIndex(0);
    fiberColorOrientation[0] = 1; // R = y
    fiberColorOrientation[1] = 0; // G = x
    fiberColorOrientation[2] = 2; // B = z
    // tractography and fiber management
    SkipSlider->setValue(75);
    SkipPercLabel->setText("75%");
    // clear VOI list
    Label_VOI_List->clear();
    ROI_VOI_List->clear();
}
void MainWindow::enableImageControls(bool status)
{
    DiffusionView->setEnabled(status);
    ActiveVolBox->setEnabled(status);
    CorFrame->setEnabled(status);
    SagFrame->setEnabled(status);
    AxFrame->setEnabled(status);
    ThreeD->setEnabled(status);
}
void MainWindow::enableVectorControls(bool status)
{
    VectorSliceBox->setEnabled(status);
    GradientOrientationBox->setEnabled(status && dm.type() == TDF);
    VectorFrame->setEnabled(status);
}
void MainWindow::enableFiberControls(bool status)
{
    // tractography and fiber management
    FiberDisplayBox->setEnabled(status);
    FiberROI_Box->setEnabled(status);
    FiberManagementBox->setEnabled(status);
    FiberVisualizationBox->setEnabled(status);
}
void MainWindow::enableLabelControls(bool status)
{
    RenderButton->setEnabled(status);
    ClearLabelsButton->setEnabled(status);
    LabelsControlBox->setEnabled(status);
    Label_ROI_Control_Box->setEnabled(status && !fibers.isEmpty());
}

// connectome stuff
void MainWindow::resetConnectomeControls()
{
    // node
    NoLabelCheck->setChecked(true);
    LabelThresholdSpinner->setValue(0.0);
    LabelThresholdBox->setCurrentIndex(0);
    EqualNodeSizeSpinner->setValue(2.5);
    ScaleNodeSizeSpinner->setValue(1.0);
    SameNodeColorCheck->setChecked(true);
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,Qt::red);
    SameNodeColorButton->setPalette(pal);
    ModularNodeColorBox->setCurrentIndex(0);
    ThresholdNodeColorBox->setCurrentIndex(0);
    ThresholdNodeColorBeforeButton->setPalette(pal);
    pal.setColor(QPalette::Active,QPalette::Button,Qt::blue);
    ThresholdNodeColorAfterButton->setPalette(pal);
    ThresholdNodeColorBeforeSpinner->setValue(0.0);
    // Edges
    AllEdgeCheck->setChecked(true);
    ThresholdEdgeSpinner->setValue(0.0);
    EqualEdgeSizeSpinner->setValue(0.5);
    WeightedEdgeSizeBox->setCurrentIndex(0);
    ScaleEdgeSizeSpinner->setValue(1.0);
    SameEdgeColorCheck->setChecked(true);
    pal.setColor(QPalette::Active,QPalette::Button,Qt::gray);
    SameEdgeColorButton->setPalette(pal);
    ModularEdgeColorBox->setCurrentIndex(0);
    ThresholdEdgeColorBox->setCurrentIndex(0);
    ThresholdEdgeColorBeforeButton->setPalette(pal);
    pal.setColor(QPalette::Active,QPalette::Button,Qt::blue);
    ThresholdEdgeColorAfterButton->setPalette(pal);
    ThresholdEdgeColorBeforeSpinner->setValue(0.0);
    // visualization
    CVTKWidget->clear();
    CnctView->clear();
    ConnectomeMaskCheck->setChecked(true);
    ConnectomeMaskOpacitySlider->setValue(20);
}

void MainWindow::enableConnectomeControls(bool status)
{
    // controls
    NodesBox->setEnabled(status);
    EdgesBox->setEnabled(status);
    GraphMetricsBox->setEnabled(status);
    // visualization
    CnctView->setEnabled(status);
    CVTKWidget->setEnabled(status);
    ConnectomeMaskBox->setEnabled(status);
}

// define different views
void MainWindow::resetViews()
{
    QList<int> currentSizes;
    currentSizes.append(DisplayTab->width()*3/5.0);
    currentSizes.append(DisplayTab->width()*2/5.0);
    splitterH->setSizes(currentSizes);
    currentSizes.clear();
    currentSizes.append(DisplayTab->height()/3);
    currentSizes.append(DisplayTab->height()/3);
    currentSizes.append(DisplayTab->height()/3);
    splitterV1->setSizes(currentSizes);
    currentSizes.clear();
    currentSizes.append(DisplayTab->height()/2);
    currentSizes.append(DisplayTab->height()/2);
    splitterV2->setSizes(currentSizes);
    resetSize = true;
    updateLabelsSize();
    if (!fibers.isEmpty() && ColorMethod->currentIndex()== SCALAR)
        VTKWidget->updateScalarBarPosition();
    DisplayTab->setCurrentIndex(0);
}
void MainWindow::showCoronalOnly()
{
    QList<int> currentSizes;
    currentSizes.append(0);
    currentSizes.append(DisplayTab->width());
    splitterH->setSizes(currentSizes);
    currentSizes.clear();
    currentSizes.append(DisplayTab->height());
    currentSizes.append(0);
    currentSizes.append(0);
    splitterV1->setSizes(currentSizes);
    resetSize = true;
    updateCoronalSize();
    resetSize = false;
    DisplayTab->setCurrentIndex(0);
}
void MainWindow::showSagittalOnly()
{
    QList<int> currentSizes;
    currentSizes.append(0);
    currentSizes.append(DisplayTab->width());
    splitterH->setSizes(currentSizes);
    currentSizes.clear();
    currentSizes.append(0);
    currentSizes.append(DisplayTab->height());
    currentSizes.append(0);
    splitterV1->setSizes(currentSizes);
    resetSize = true;
    updateSagittalSize();
    resetSize = false;
    DisplayTab->setCurrentIndex(0);
}
void MainWindow::showAxialOnly()
{
    QList<int> currentSizes;
    currentSizes.append(0);
    currentSizes.append(DisplayTab->width());
    splitterH->setSizes(currentSizes);
    currentSizes.clear();
    currentSizes.append(0);
    currentSizes.append(0);
    currentSizes.append(DisplayTab->height());
    splitterV1->setSizes(currentSizes);
    resetSize = true;
    updateAxialSize();
    resetSize = false;
    DisplayTab->setCurrentIndex(0);
}
void MainWindow::showVectorOnly()
{
    QList<int> currentSizes;
    currentSizes.append(DisplayTab->width());
    currentSizes.append(0);
    splitterH->setSizes(currentSizes);
    currentSizes.clear();
    currentSizes.append(DisplayTab->height());
    currentSizes.append(0);
    splitterV2->setSizes(currentSizes);
    resetSize = true;
    updateVectorSize();
    resetSize = false;
    DisplayTab->setCurrentIndex(0);
}
void MainWindow::show3DOnly()
{
    QList<int> currentSizes;
    currentSizes.append(DisplayTab->width());
    currentSizes.append(0);
    splitterH->setSizes(currentSizes);
    currentSizes.clear();
    currentSizes.append(0);
    currentSizes.append(DisplayTab->height());
    splitterV2->setSizes(currentSizes);
    if (!fibers.isEmpty() && ColorMethod->currentIndex()== SCALAR)
        VTKWidget->updateScalarBarPosition();
    DisplayTab->setCurrentIndex(0);
}

// control vector table
void MainWindow::updateNormal(bool checked)
{
    Normal->setChecked(true);
    if (checked) {
        InvertX->setChecked(false);
        InvertY->setChecked(false);
        InvertZ->setChecked(false);
        SwapXY->setChecked(false);
        SwapXZ->setChecked(false);
        SwapYZ->setChecked(false);
        dm.q = dm.originalQ;
        Rx = dm.Rx;
        Ry = dm.Ry;
        Rz = dm.Rz;
        updateColorSlice();
        updateVectors();
    }
}
void MainWindow::updateInvertX()
{
    Normal->setChecked(false);
    dm.q.row(0) = - dm.q.row(0);
    updateVectors();
}
void MainWindow::updateInvertY()
{
    Normal->setChecked(false);
    dm.q.row(1) = - dm.q.row(1);
    updateVectors();
}
void MainWindow::updateInvertZ()
{
    Normal->setChecked(false);
    dm.q.row(2) = - dm.q.row(2);
    updateVectors();
}
void MainWindow::updateSwapXY()
{
    Normal->setChecked(false);
    dm.q.swap_rows(0,1);
    Rx.swap(Ry);
    updateColorSlice();
    updateVectors();
}
void MainWindow::updateSwapXZ()
{
    Normal->setChecked(false);
    dm.q.swap_rows(0,2);
    Rx.swap(Rz);
    updateColorSlice();
    updateVectors();
}
void MainWindow::updateSwapYZ()
{
    Normal->setChecked(false);
    dm.q.swap_rows(1,2);
    Rz.swap(Ry);
    updateColorSlice();
    updateVectors();
}
void MainWindow::showTable()
{
    ShowQTableDialog *w = new ShowQTableDialog(dm.q,this);
    w->exec();
    delete w;
}

void MainWindow::tab1ChangeAction(int selected)
{}
void MainWindow::tab2ChangeAction(int selected)
{
    RenderButton->setEnabled(bool(selected == 2 &&
                                  activeImgIsLabel &&
                                  VTKWidget->isLabelVolumeEmpty()));
}

void MainWindow::generateFibers()
{
    // start generating all brain fibers
    TrackFibersDialog w(this);
    QStringList list;
    for (uint i = 1; i<nLoadedImg+1;i++)
        list.append(ImageComboBox->itemText(i));
    w.setLoadedImagesList(list);
    if (dm.type() == DTI) {
        // Enable FACT
        w.TrackingMethodBox->setItemData(0,33,Qt::UserRole-1);
        // Disable TDF
        w.TrackingMethodBox->setItemData(1,0,Qt::UserRole-1);
        w.TrackingMethodBox->setItemData(2,0,Qt::UserRole-1);
        w.TrackingMethodBox->setCurrentIndex(0);
    }
    else if (dm.type() == TDF) {
        // Disable FACT
        w.TrackingMethodBox->setItemData(0,0,Qt::UserRole-1);
        // Enable TDF
        w.TrackingMethodBox->setItemData(1,33,Qt::UserRole-1);
        w.TrackingMethodBox->setItemData(2,33,Qt::UserRole-1);
        w.TrackingMethodBox->setCurrentIndex(1);
    }

    if (!w.exec())
        return;
    // get tracking options
    TrackingParameters tp = w.tp;
    tp.fiberColor           = fiberColor;
    // clear current fibers
    if (!fibers.isEmpty())
        QMessageBox::warning(0, "Warning!", "This operation will ommit current fibers.");
    clearFibers();
    // check tracking method
    bool success;
    MyProgressDialog *prog = new MyProgressDialog(this);
    prog->disableMain();
    if (w.TrackingMethodBox->currentIndex()==0)            // FACT
        success = dm.FACT_DTI(tp,fibers,prog);
    else if (w.TrackingMethodBox->currentIndex() == 1) {   // TOD Deterministic
    }
    else if (w.TrackingMethodBox->currentIndex() == 2)     // TOD Probabilistic
        success = dm.probablistic_TOD_Tracking(tp,fibers,prog);
    delete prog;
    if (success && !fibers.isEmpty()) {                 // check for cancel
        // activate tractography controls
        enableFiberControls(true);
        DisplayNone->setChecked(true);
        FiberROI_Box->setEnabled(false);
        FiberManagementBox->setEnabled(false);
        FiberVisualizationBox->setEnabled(false);
        // update lenggth filter
        FiberLengthSlider->setMaximum(qFloor(fibers.maxLength()));
        FiberLengthSlider->setUpperPosition(FiberLengthSlider->maximum());
        LeftSliderLabel->setText("0");
        RightSliderLabel->setText(QString::number(qFloor(fibers.maxLength())));
        // initialize currentSelectedFibers
        currentSelectedFibers.clear();
        for (int i = 0; i<fibers.size();i++)
            currentSelectedFibers.append(i);
        visFibers();
    }
    else
        clearFibers();
    fill_VOI_List();
}
void MainWindow::changeFiberDisplayState()
{
      // activate tractography
    FiberROI_Box->setEnabled(DisplaySelected->isChecked());
    RecolorFibersButton->setEnabled(DisplaySelected->isChecked() &&
                                    (ColorMethod->currentIndex() == SOLID));
    FiberManagementBox->setEnabled(!DisplayNone->isChecked());
    FiberVisualizationBox->setEnabled(!DisplayNone->isChecked());

    if (DisplaySelected->isChecked() && !selectedFibersList.isEmpty()) {
        // Display ROI Selected fibers if exist
        currentSelectedFibers = selectedFibersList.last();
    }
    else { // Display ALL and NONE
        currentSelectedFibers.clear();
        currentSelectedFibers.reserve(fibers.size());
        for (int i = 0; i<fibers.size();i++)
            currentSelectedFibers.append(i);
        UncheckROI_Button->setChecked(true);
    }
    visFibers();
}

// Fiber visualization controls
void MainWindow::colorMethodChanged(int newMethod)
{
    RecolorFibersButton->setEnabled((newMethod == SOLID) && DisplaySelected->isChecked());
    FiberColorOrient->setEnabled((newMethod == DIRECTIONAL));
    visFibers();
}
void MainWindow::fiberColorOrientChanged(int newOrient)
{
    /*
      The main target is to have red (left - right), green (ant - post),
      blue (sup - inf)
      X is asumed to be (ant-post), Y is assumed to be (left - right),
      and Z is assumed to be (sup - inf)
      */
    if (newOrient == 0) { // RGB
        fiberColorOrientation[0] = 1; // R = y
        fiberColorOrientation[1] = 0; // G = x
        fiberColorOrientation[2] = 2; // B = z
    }
    else if (newOrient == 1) { //RBG
        fiberColorOrientation[0] = 1; // R = y
        fiberColorOrientation[1] = 2; // B = z
        fiberColorOrientation[2] = 0; // G = x
    }
    else if (newOrient == 2) { //GRB
        fiberColorOrientation[0] = 0; // G = x
        fiberColorOrientation[1] = 1; // R = y
        fiberColorOrientation[2] = 2; // B = z

    }
    else if (newOrient == 3) { //GBR
        fiberColorOrientation[0] = 0; // G = x
        fiberColorOrientation[1] = 2; // B = z
        fiberColorOrientation[2] = 1; // R = y

    }
    else if (newOrient == 4) { //BRG
        fiberColorOrientation[0] = 2; // B = z
        fiberColorOrientation[1] = 1; // R = y
        fiberColorOrientation[2] = 0; // G = x

    }
    else if (newOrient == 5) { //BGR
        fiberColorOrientation[0] = 2; // B = z
        fiberColorOrientation[1] = 0; // G = x
        fiberColorOrientation[2] = 1; // R = y
    }
    visFibers();
}
void MainWindow::fiberColorPalettePressed()
{
    if (fibers.isEmpty() || currentSelectedFibers.isEmpty() ||
            (selectedFibersList.isEmpty() && DisplaySelected->isChecked())) {
        QMessageBox::warning(this,"CoNECt","No active fibers, nothing to color.");
        return;
    }
    QColor c = QColorDialog::getColor(Qt::red, this, "Select Color");
    if (c.isValid()) {
        fiberColor = c;
        QPalette pal;
        pal.setColor(QPalette::Active,QPalette::Button,fiberColor);
        RecolorFibersButton->setPalette(pal);
        // should apply length control before coloring
        QList<quint32> list = currentSelectedFibers;
        fibers.filterFibersByLength(list,FiberLengthSlider->lowerValue(),
                                    FiberLengthSlider->upperValue());
        for (int i = 0; i < list.size(); i++)
            fibers.setColor(fiberColor,list[i]);
        visFibers();
    }
}

void MainWindow::closeAll()
{
    // clean any loaded volumes from memory
    clearLoadedImages();
    initalize();
}
void MainWindow::openModel()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open Model", ".",
                                                    QString("Model files (*.tod *.hdr *.nii *.dat)\n"
                                                            "TOD files (*.tod)\n"
                                                            "DTI files (*.hdr *.nii *.dat)\n"
                                                            "All files (*.*)\n"));
    if (fileName.isEmpty())
        return;
    QFileInfo file(fileName);
    MyProgressDialog *prog = new MyProgressDialog(this);
    prog->disableCancel();
    prog->disableMain();
    if (file.suffix() == "tod") {
        if (!dm.readTOD(fileName,prog)) {
            delete prog;
            return;
        }
        // update status bar
        FileInfo->setText(QString("%1: %2, Model res: %3 X %4 X %5 ")
                          .arg(file.fileName()).arg(dm.th)
                          .arg(dm.dx).arg(dm.dy).arg(dm.dz));
        // save b0 info
        // datatype,Dx,Dy,Dz,nRows,nCols,nSlices,locationInItsOwnTypeList,isLabel,LUT
        // the TOD b0 should be made a uchar later to save memory
        loadedImgs.doubleImg.append(dm.b0);
        loadedImgsInfo.insert_rows(nLoadedImg,1);
        loadedImgsInfo(nLoadedImg,0) = DT_DOUBLE;
        loadedImgsInfo(nLoadedImg,1) = dm.dx;
        loadedImgsInfo(nLoadedImg,2) = dm.dy;
        loadedImgsInfo(nLoadedImg,3) = dm.dz;
        loadedImgsInfo(nLoadedImg,4) = dm.nRows;
        loadedImgsInfo(nLoadedImg,5) = dm.nCols;
        loadedImgsInfo(nLoadedImg,6) = dm.nSlices;
        loadedImgsInfo(nLoadedImg,7) = nLoadedDouble++;
        loadedImgsInfo(nLoadedImg,8) = false;   // not label
        loadedImgsInfo(nLoadedImg,9) = 0;       //grayscale
        nLoadedImg++;
        // acquire color background
        Rx = dm.Rx; Ry = dm.Ry; Rz = dm.Rz;
        // assign b0 to the activeVol
        ImageComboBox->addItem("Model b0");
    }
    else if (file.suffix() == "odf") {

    }
    else {
        QList<fcube> ADC,metrics;
        if (!dm.readDTI(fileName,ADC,metrics,prog) || dm.isEmpty()) {
            delete prog;
            return;
        }
        // update status bar
        FileInfo->setText(QString("%1: Model res: %3 X %4 X %5 ")
                          .arg(file.fileName())
                          .arg(dm.dx).arg(dm.dy).arg(dm.dz));
        // save metrics
        loadedImgs.floatImg.append(ADC);
        loadedImgs.floatImg.append(metrics);
        loadedImgsInfo.insert_rows(nLoadedImg,13);
        for (int i=0;i<13;++i) {
            // datatype,Dx,Dy,Dz,nRows,nCols,nSlices,locationInItsOwnTypeList,isLabel,LUT
            loadedImgsInfo(nLoadedImg,0) = DT_FLOAT;
            loadedImgsInfo(nLoadedImg,1) = dm.dx;
            loadedImgsInfo(nLoadedImg,2) = dm.dy;
            loadedImgsInfo(nLoadedImg,3) = dm.dz;
            loadedImgsInfo(nLoadedImg,4) = dm.nRows;
            loadedImgsInfo(nLoadedImg,5) = dm.nCols;
            loadedImgsInfo(nLoadedImg,6) = dm.nSlices;
            loadedImgsInfo(nLoadedImg,7) = nLoadedFloat++;
            loadedImgsInfo(nLoadedImg,8) = false;       // not label
            loadedImgsInfo(nLoadedImg,9) = 0;           //grayscale
            ++nLoadedImg;
        }
        // update images menu
        ImageComboBox->addItem("Tensor - Dxx");
        ImageComboBox->addItem("Tensor - Dyy");
        ImageComboBox->addItem("Tensor - Dzz");
        ImageComboBox->addItem("Tensor - Dxy");
        ImageComboBox->addItem("Tensor - Dxz");
        ImageComboBox->addItem("Tensor - Dyz");
        ImageComboBox->addItem("Eigen Value 0");
        ImageComboBox->addItem("Eigen Value 1");
        ImageComboBox->addItem("Eigen Value 2");
        ImageComboBox->addItem("Anisotropy - MD");
        ImageComboBox->addItem("Anisotropy - FA");
        ImageComboBox->addItem("Anisotropy - RA");
        ImageComboBox->addItem("Anisotropy - VR");

        // acquire background
        Rx = conv_to<uchar_cube>::from(dm.Mask%abs(dm.eVecPy)*255.0);
        Ry = conv_to<uchar_cube>::from(dm.Mask%abs(dm.eVecPx)*255.0);
        Rz = conv_to<uchar_cube>::from(dm.Mask%abs(dm.eVecPz)*255.0);
    }
    delete prog;
    // actions and menus
    actionLoadModelFile->setEnabled(false);
    actionLoadImage->setEnabled(true);
    actionLoadFiber->setEnabled(true);
    actionCloseAll->setEnabled(true);
    actionGenerateFibers->setEnabled(true);
    actionGenerateConnectome->setEnabled(true);
    // activate controls
    enableImageControls(true);
    enableVectorControls(true);
    DisplayTab->setCurrentIndex(0);
    // update GUI according to model file
    VectorSliceSpinner->setMaximum(dm.nRows-1);
    ImageComboBox->setCurrentIndex(0);
    dxImg = dm.dx; dyImg = dm.dy; dzImg = dm.dz;
    // update new volume information
    changeImage(0);
    resetViews();
}

// wizard stuff
void MainWindow::onWizardButtonPress()
{
    CoNECtWizard *wizard = new CoNECtWizard(this);
    if (!wizard->exec()) {
        delete wizard;
        return;
    }
    QChar sep = QDir::separator();
    QString log;
    // process the wizard input
    bool isDW = wizard->field("intro.dw").toBool();
    bool isIndividual = wizard->field("intro.singleSubject").toBool();
    // prepare progress dialog
    int processed = 0, skipped = 0, failed = 0;
    if (isDW) {
        // perfomr visual inspection
        // get wizard data
        // data source, type and save folder, same for the group
        QString src = wizard->field("page1dw.dataSource").toString();
        src = (isIndividual)?QFileInfo(src).absolutePath():src;
        src = src.replace("/",sep);
        if (src.at(src.size()-1) == sep) src.truncate(src.size()-1);
        QString baseName = QFileInfo(wizard->field("page1dw.sourceBaseName").toString()).baseName();
        ImageFileType inType = static_cast<ImageFileType>(wizard->field("page1dw.dataType").toUInt());
        ImageFileType outType = static_cast<ImageFileType>(wizard->field("page1dw.saveMetricFormat").toUInt()+1);
        bool isDICOM = (inType == DICOM);
        inType = (isDICOM)?outType:inType;
        DiffusionModelType model = static_cast<DiffusionModelType>(wizard->field("page1dw.imagingModel").toUInt());
        QString sinc = wizard->field("page1dw.saveFolder").toString();
        if (sinc.at(sinc.size()-1) == sep) sinc.truncate(sinc.size()-1);
        // DWI parameters
        float bValue = wizard->field("page1dw.bValue").toFloat();
        int nLevel = wizard->field("page2dw.noiseLevel").toInt();
        nLevel = (nLevel == 0)?50:nLevel;
        QList<mat> *tableList = new QList<mat>();
        uint gTableIndex = wizard->field("page1dw.gTable").toUInt();
        Page1DW::readGradientTablesFile(tableList,NULL);
        mat gTable = tableList->at(gTableIndex);
        tableList->clear();
        delete tableList;
        int expNImgs = gTable.n_rows;
        qDebug()<<"I am here";
        // preprocessing
        bool doECC = wizard->field("page2dw.doECC").toBool();
        // post processing
        bool computeMetrics = (wizard->field("page1dw.computeMetricsDTI").toBool() && (model == DTI)) ||
                (wizard->field("page1dw.computeMetricsTDF").toBool() && (model == TDF));
        // tractography
        bool computeTracts = wizard->field("page2dw.doTractography").toBool();
        TrackingParameters *tp = NULL;
        if (computeTracts) {
            tp = new TrackingParameters();
            wizard->getTractographyParameters(tp);
            tp->fiberColor = fiberColor;
        }
        QStringList subjects = QDir(src).entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
        // if data source is dicom transform it into output format
        QDir opDir(sinc);
        if (isDICOM) {
            uint nS = (isIndividual)?1:subjects.size();
            MyProgressDialog *prog = new MyProgressDialog(this);
            prog->disableCancel();
            prog->disableMain();
            prog->setLabelText(QString("Converting DICOM for Subject XXX of %1.").arg(nS));
            prog->setRange(0,nS);
            prog->setModal(true);
            prog->setFixedSize(prog->sizeHint());
            prog->setValue(0);
            prog->show();
            log += "------------------ Conversion To DICOM Begin ------------------\n\n";
            QString inFolder, saveFolder;
            for (uint s=0; s<nS;++s) {
                prog->setLabelText(QString("Converting DICOM for Subject %1 of %2.").
                                   arg(s+1).arg(nS));
                inFolder    = (isIndividual)?src:(src + sep + subjects[s]);
                saveFolder  = (isIndividual)?sinc:(sinc + sep + subjects[s]);
                if (!QDir(saveFolder).exists()) { opDir.mkdir(subjects[s]); }
                log += (isIndividual)?("Converting from DICOM ... \n"):
                                      ("Converting from DICOM for subject " + subjects[s] + "\n");
                if (!DICOM2NII(inFolder,saveFolder,(outType == NIFTI),baseName)) {
                    log += "Conversion from DICOM failed \n";
//                    if (isIndividual) { delete prog; return;}
                    continue;
                }
                prog->setValue(prog->value()+1);
            }
            log += "------------------ Conversion To DICOM End ------------------\n\n";
            src = sinc;
            delete prog;
        }
        // inpect data
        DataInspectionDialog *w = new DataInspectionDialog(this);
        w->init(isIndividual,src,baseName,inType,expNImgs);
        if (!w->exec()) { delete w; return; }
        ucolvec subjectsStatus = w->subjectsStatus;
        w->hide();
        delete w;
        uint nSubj = (isIndividual)?1:(subjectsStatus.size());
        // inspection accepted, proceed with generation
        // need to do a progress dialog with two progress bars
        MyProgressDialog *globProg = new MyProgressDialog(this);
        globProg->disableCancel();
        globProg->setModal(true);
        globProg->setMainLabelText(QString("Computing for Subject XXX of %1.").arg(nSubj));
        globProg->setRangeMain(0,sum(subjectsStatus));
        globProg->setFixedSize(globProg->sizeHint());
        globProg->setValueMain(0);
        globProg->show();
        QString dwiFile, saveFolder;
        log += "------------------ DWI Image Processing Begin ------------------\n";
        for (uint s = 0;s<nSubj;++s) {
            globProg->setMainLabelText(QString("Computing for Subject %1 of %2.")
                                       .arg(s+1).arg(nSubj));
            // prepare in dwi file and output folder
            // in case of multiple subjects, check if valid proceed, if not bypass
            // create output folder
            if (!isIndividual && !subjectsStatus(s)) {
                log += "Skipping subject " + subjects[s] + "\n";
                ++skipped;
                continue;
            }
            dwiFile = (isIndividual)?(src + sep + baseName):(src + sep + subjects[s] + sep + baseName);
            saveFolder = (isIndividual)?sinc:(sinc + sep + subjects[s]);
            if (!QDir(saveFolder).exists()) { opDir.mkdir(subjects[s]); }
            if (model == DTI) {
                    log += (isIndividual)?("\nDTI model extraction ... \n"):
                                          ("\nDTI model extraction for subject " + subjects[s] +" :\n");
                if (!processDTI_DWI_Subject(dwiFile,inType,gTable,bValue,nLevel,
                                            computeMetrics,tp,outType,saveFolder, log,
                                            globProg)) {
                    log  += "Non complete subject.\n";
                    ++failed;
                    continue;
                }
                ++processed;
                log += (isIndividual)?("Processing successful.\n"):
                                      ("Processing successful for " + subjects[s] + "\n");
            }
            else if (model == TDF) {

            }
            globProg->setValueMain(globProg->valueMain()+1);
        }
        delete globProg;
        log += "\n------------------ DWI Image Processing End ------------------\n\n";
    }
    else {
        QString fiberSrc = wizard->field("page1bna.fiberSourceFolder").toString();
        if (fiberSrc.at(fiberSrc.size()-1) == sep) fiberSrc.truncate(fiberSrc.size()-1);
        QString labelSrc = wizard->field("page1bna.labelSourceFolder").toString();
        if (labelSrc.at(labelSrc.size()-1) == sep) labelSrc.truncate(labelSrc.size()-1);
        QString b0Src = wizard->field("page1bna.b0SourceFolder").toString();
        if (b0Src.at(b0Src.size()-1) == sep) b0Src.truncate(b0Src.size()-1);
        QString namesFile = wizard->field("page1bna.namesSourceFile").toString();
        QString T1Src = wizard->field("page1bna.T1SouceFolder").toString();
        if (T1Src.at(T1Src.size()-1) == sep) T1Src.truncate(T1Src.size()-1);
        QString sinc = wizard->field("page1bna.saveFolder").toString();
        if (sinc.at(sinc.size()-1) == sep) sinc.truncate(sinc.size()-1);

        QString fiberBN = QFileInfo(wizard->field("page1bna.fiberBaseName").toString()).baseName();
        QString labelBN = QFileInfo(wizard->field("page1bna.labelBaseName").toString()).baseName();
        QString b0BN = QFileInfo(wizard->field("page1bna.b0BaseName").toString()).baseName();
        QString T1BN = QFileInfo(wizard->field("page1bna.T1BaseName").toString()).baseName();

        bool isFreesurfer = wizard->field("page1bna.isFreesurfer").toBool();
        bool computeMetrics = wizard->field("page1bna.computeMetrics").toBool();

        FiberFileType fiberType = static_cast<FiberFileType>(wizard->field("page1bna.fiberFileType").toUInt());
        ImageFileType labelType = static_cast<ImageFileType>(wizard->field("page1bna.labelFileType").toUInt()+1);
        ImageFileType b0Type = static_cast<ImageFileType>(wizard->field("page1bna.b0FileType").toUInt()+1);
        ImageFileType T1Type = static_cast<ImageFileType>(wizard->field("page1bna.T1FileType").toUInt()+1);

        QStringList subjects = QDir(fiberSrc).entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
        uint nS = (isIndividual)?1:subjects.size();

        MyProgressDialog *globProg = new MyProgressDialog(this);
        globProg->disableCancel();
        globProg->setModal(true);
        globProg->setMainLabelText(QString("Generating connectome for Subject XXX of %1.").arg(nS));
        globProg->setRangeMain(0,nS);
        globProg->setFixedSize(globProg->sizeHint());
        globProg->setValueMain(0);
        globProg->show();

        Connectome *cmm = new Connectome();
        FiberTracts *fb = new FiberTracts;
        DiffusionModelDimension dmp;
        AnalyzeHeader header;
        s16_cube label;
        QString fiberFile, labelFile, saveFolder, nwSaveFile, metricSaveFile, topProgText;
        QDir opDir = QDir(sinc);
        log += "------------------ Connectome Generation Begin ------------------\n";
        bool error;
        for (uint s = 0; s< nS;++s) {
            error = false;
            globProg->setMainLabelText(QString("Generating connectome for Subject %1 of %2.").
                                        arg(s+1).arg(nS));
            log += (isIndividual)?("\nStart Connectome generation ... \n"):
                                  ("\nConnectome generation for subject " + subjects[s] + "\n");
            fiberFile = (isIndividual)?fiberSrc:(fiberSrc + sep + subjects[s] + sep + fiberBN);
            fiberFile += (!isIndividual)?((fiberType == CONECT)?(".fib"):(".dat")):("");
            labelFile = (isIndividual)?(labelSrc):(labelSrc + sep + subjects[s] + sep + labelBN);
            saveFolder = (isIndividual)?(""):(sinc + sep + subjects[s] + sep);
            if (!isIndividual && !QDir(saveFolder).exists()) { opDir.mkdir(subjects[s]); }
            nwSaveFile = (isIndividual)?(sinc + sep + "Network.cnct"):(saveFolder + "Network.cnct");
            metricSaveFile = (isIndividual)?(sinc + sep + "Metrics.txt"):(saveFolder + "Metrics.txt");
            log += "Reading label " + labelFile + "\n";
            int r = cmm->readLabel(labelFile,label,header,labelType,globProg);
            // handle different error cases later in a log
            // 1    error reading image
            // 2    number of volumes > 1
            // 3    non label or unsuported data type
            if (r == 1)         log += "Error reading label image, skipping subject.\n";
            else if (r == 2)    log += "File contains more than one image, skipping subject.\n";
            else if (r == 3)    log += "Non-lablel image or unsupported data type, skipping subject.\n";
            if (r>0)            error = true;
            dmp = getParameters(header);
            log += "Reading fiber file " + fiberFile + "\n";
            if(!error && !fb->readFiber(fiberFile,dmp,globProg,false)) {
                log += "Error reading fiber file, skipping subject.\n";
                error = true;
            }
            log += "Generating connectome ... \n";
            if (!error && !cmm->generate(label,*fb,isFreesurfer,globProg)) {
                log += "Error generating connectome, skipping subject.\n";
                error = true;
            }
            if (!error && !isFreesurfer && !cmm->readNames(namesFile)) {
                log += "Error reading label names file, skipping subject.\n";
                error = true;
            }
            globProg->setLabelText("Saving connectome ... ");
            log += "Saving connectome at " + nwSaveFile + "\n";
            if (!error && !cmm->write(nwSaveFile,true)) {
                log += "Error writing file, skipping subject.\n";
                error = true;
            }

            if (!error & computeMetrics) {
                log += "Computing graph metrics ... \n";
                globProg->setLabelText("Computing graph metrics ... ");
                cmm->computeMetrics();
                log += "Saving metrics at " + metricSaveFile + "\n";
                globProg->setLabelText("Saving graph metrics ... ");
                if (!cmm->writeMetrics(metricSaveFile,true)) {
                    log += "Error saving.\n";
                    error = true;
                }
            }
            if (error)  ++failed;
            else        ++processed;
            cmm->clear();
            globProg->setValueMain(globProg->valueMain()+1);
        }
        log += "------------------ Connectome Generation End ------------------\n";
        delete globProg;
        delete cmm;
        delete fb;
    }
    delete wizard;
    log += "\n \n ------------------ SUMMARY ------------------\n";
    log += QString("Number of processed subjects : %1 \n").arg(processed);
    log += QString("Number of skipped subjects : %1 \n").arg(skipped);
    log += QString("Number of failed subjects : %1 \n").arg(failed);
    ReportDialog *w  = new ReportDialog(log,this);
    w->exec();
    return;
}

bool MainWindow::processDTI_DWI_Subject(const QString &dwiFile,
                                        ImageFileType inType,
                                        const mat &gTable,
                                        float bValue,
                                        int nLevel,
                                        bool getMetrics,
                                        TrackingParameters *tp,
                                        ImageFileType outType,
                                        const QString &outFolder,
                                        QString &log,
                                        MyProgressDialog *prog)
{
    // open DWI file
    QList<s16_cube> dwi;
    AnalyzeHeader header;
    QChar sep = QDir::separator();
    log += "Reading DWI " + dwiFile + "\n";
    if (!DiffusionModel::readDWI(dwiFile,inType,dwi,header,prog)) {
        log += "Error reading DWI data.\n";
        return false;
    }
    // generate ADC
    QList<fcube> ADC;
    log += "Computing DTI model ... \n";
    if (!DiffusionModel::computeDTIModel(dwi,gTable,bValue,nLevel,ADC,prog)) {
        log += "Error computing DTI model.\n";
        return false;
    }
    // save output data
    quint32 dim[] = {ADC[0].n_rows,ADC[0].n_cols,ADC[0].n_slices,6};
    float pixDim[] = {header.dime.pixdim[1],header.dime.pixdim[2],header.dime.pixdim[3]};
    AnalyzeHeader outH = prepareHeader(dim,DT_FLOAT,pixDim,outType);
    QString sf = outFolder +sep+"Tensor";
    log += "Saving Tensor in " + sf + "\n";
    if (!saveImages(sf,outType,outH,ADC)) {
        log += "Error saving tensor file.\n";
        return false;
    }
    DiffusionModel *DM = NULL;
    if (tp)  DM = new DiffusionModel();
    // compute metrics if required and save it afterward
    if (getMetrics) {
        // save all metrics
        QStringList names;
        names << "eVal1" << "eVal2" << "eVal3" << "FA" << "RA" << "VR" << "MD";
        QList<fcube> metrics;
        // if tracking is required create a model and fill it
        log += "Computing metrics ... \n";
        if (tp) DM->setDTI(ADC,metrics,header,prog);
        else    DiffusionModel::computeDTIStuff(ADC,metrics,prog);

        prog->setLabelText("Saving metrics ... ");
        prog->setRange(0,names.size());
        prog->setModal(true);
        prog->setValue(0);
        prog->setFixedSize(prog->sizeHint());
        prog->show();

        outH.dime.dim[4] = 1;
        log += "Saving Metrics ... \n";
        for (uint i=0;i<names.size();++i) {
            sf = outFolder+sep+names[i];
            QList<fcube> out;
            out.append(metrics[i]);
            log += "Saving " + names[i] + " at " + sf + "\n";
            if (!saveImages(sf,outType,outH,out)) {
                log += "Error saving " + names[i] + "\n";
                if (DM) delete DM;
                return false;
            }
            prog->setValue(prog->value()+1);
        }
    }
    if (tp) {
        log += "Performing Tractography ... \n";
        // prepare model if no metrics where computed
        if (!getMetrics) {
            log += "Computing eigen vectors and FA ... \n";
            QList<fcube> metrics;
            // if tracking is required create a model and fill it
            if (tp) DM->setDTI(ADC,metrics,header,prog);
            metrics.clear();
        }
        FiberTracts tracts;
        if(!DM->FACT_DTI(*tp,tracts,prog)) {
            log += "Error while computing tractography.\n";
            delete DM; return false;
        }
        sf = outFolder+sep+"Fibers.fib";
        QList<quint32> idx;
        idx.reserve(tracts.size());
        for (uint i=0;i<tracts.size();++i)
            idx.append(i);
        log += "Saving tracts at " + sf + "\n";
        if (!tracts.writeFiber(sf,idx,prog)) {
            log += "Error saving tracts.\n";
            delete DM; return false;
        }
    }
    return true;
}

bool MainWindow::DICOM2NII(const QString &inFolder,
                           const QString &outFolder,
                           bool toNii,
                           QString &out)
{
    // dicom2nii place
    QChar sep = QDir::separator();
    QString dcm2nii = "\""+QDir::currentPath()+sep+"dcm2nii\" -a y -d n -e n " +
            "-f y -g n -i n -m n -p n -r n -v y ";
    if (toNii)  dcm2nii += "-n y -o ";      // options
    else        dcm2nii += "-n n -o ";      // options
    if (!QDir(inFolder).exists() || !QDir(outFolder).exists())
        return false;
    // create a new folder to assure output location
    QString workDir = outFolder+sep+"tmp"+sep;
    if (!QDir(workDir).exists()) { QDir(outFolder).mkdir("tmp"); }
    QString cmd = dcm2nii+workDir+" ";
    // need first file in the folder
    QStringList files = QDir(inFolder).entryList(QDir::Files | QDir::NoDotAndDotDot
                                                 | QDir::NoSymLinks,QDir::Name);
    if (files.isEmpty()) return false;
    cmd += inFolder + sep + files.first();
    // make sure correct separator exists
#ifdef Q_OS_WIN
    cmd.replace("/","\\");
#endif
    qDebug()<<cmd;
    int r = QProcess::execute(cmd);
    qDebug()<<r;
    if (r != 0) { return false;}
    // get the name of the converted file and remove it
    QStringList filesList = QDir(workDir).entryList(QDir::Files | QDir::NoDotAndDotDot |
                                                  QDir::NoSymLinks,QDir::Name);
    if (filesList.isEmpty()) { return false; }
    // search for the bvec and bval files, if found note their names
    QDir d(workDir);
    QString fileName, source, target, base, ext;
    foreach(fileName, filesList) {
        base = QFileInfo(fileName).baseName();
        ext = QFileInfo(fileName).suffix();
        if (ext == "bvec" || ext == "bval")
            break;
    }
    // copy files to outFolder, if they already exist, replace them
    foreach(fileName, filesList) {
        ext = QFileInfo(fileName).suffix();
        source = workDir+fileName;
        target = outFolder+sep+"DW_Image."+ext;
        if (QFileInfo(fileName).baseName() == base) {
            if (QDir(outFolder).exists("DW_Image."+ext))
                QDir(outFolder).remove("DW_Image."+ext);
            d.rename(source,target);
        }
        else
            QDir(workDir).remove(fileName);
    }
    QDir(outFolder).rmdir(workDir);
    out = "DW_Image";
    return true;
}

// Handle Image Control
void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Image", ".",
                                                    "All files (*.*)\n");
    if (fileName.isEmpty())
        return;
    OpenImageDialog w(fileName,this);
    if (!w.exec())
        return;
    // read analyze
    AnalyzeHeader header;
    MyProgressDialog *progress = new MyProgressDialog(this);
    progress->disableMain();
    progress->disableCancel();
    // start by reading header data
    ImageFileType ft = w.fileType();
    QString fn = QFileInfo(fileName).absolutePath()+"/"+QFileInfo(fileName).baseName();
    QString imgFileName = (ft == RAW)?fileName:(ft == ANALYZE)?(fn+".img"):(fn+".nii");
    if (ft == RAW)
        // get file information data in form of a header
        w.getHeader(header);
    else {
        QString headerFileName = (ft == ANALYZE)?(fn+".hdr"):(fn+".nii");
        if (!readHeader(headerFileName, header, ft))
            return;
    }
    // Acquire information from the header
    uint    dt              = header.dime.datatype,
            nVolsImg        = header.dime.dim[4];

    // read data according to its type
    if (dt == DT_BINARY || dt == DT_UNSIGNED_CHAR) {
        QList< Cube<unsigned char> > out = QList< Cube<unsigned char> >();
        if (readImage(imgFileName,ft,false,progress,header,out)) {
            if (!w.orientation.isEmpty())
                orientCubes(w.orientation, progress,header,out);
            loadedImgs.ucharImg.append(out);
            out.clear();
        } else return;
    }
    else if (dt == DT_SIGNED_SHORT) {
        QList< Cube<short> > out = QList< Cube<short> >();
        if (readImage(imgFileName,ft,false,progress,header,out)) {
            if (!w.orientation.isEmpty())
                orientCubes(w.orientation, progress,header,out);
            loadedImgs.shortImg.append(out);
            out.clear();
        } else return;

    }
    else if (dt == DT_SIGNED_INT) {
        QList< Cube<s32> > out = QList< Cube<s32> >();
        if(readImage(imgFileName,ft,false,progress,header,out)) {
            if (!w.orientation.isEmpty())
                orientCubes(w.orientation, progress,header,out);
            loadedImgs.intImg.append(out);
            out.clear();
        } else return;
    }
    else if (dt == DT_FLOAT) {
        QList< Cube<float> > out = QList< Cube<float> >();
        if (readImage(imgFileName,ft,false,progress,header,out)) {
            if (!w.orientation.isEmpty())
                orientCubes(w.orientation, progress,header,out);
            loadedImgs.floatImg.append(out);
            out.clear();
        } else return;
    }
    else if (dt == DT_DOUBLE) {
        QList< Cube<double> > out = QList< Cube<double> >();
        if (readImage(imgFileName,ft,false,progress,header,out)) {
            if (!w.orientation.isEmpty())
                orientCubes(w.orientation, progress,header,out);
            loadedImgs.doubleImg.append(out);
            out.clear();
        } else return;
    }
    progress->setValue(progress->value()+1);
    // store data information
    // datatype,Dx,Dy,Dz,nRows,nCols,nSlices,locationInItsOwnTypeList,isLabel,LUT
    loadedImgsInfo.insert_rows(nLoadedImg,nVolsImg);
    QFileInfo fi(fileName);
    QString imgName;
    for (uint i=0; i<nVolsImg;i++) {
        loadedImgsInfo(nLoadedImg+i,0) = dt;
        loadedImgsInfo(nLoadedImg+i,1) = header.dime.pixdim[1];
        loadedImgsInfo(nLoadedImg+i,2) = header.dime.pixdim[2];
        loadedImgsInfo(nLoadedImg+i,3) = header.dime.pixdim[3];
        loadedImgsInfo(nLoadedImg+i,4) = header.dime.dim[1];
        loadedImgsInfo(nLoadedImg+i,5) = header.dime.dim[2];
        loadedImgsInfo(nLoadedImg+i,6) = header.dime.dim[3];
        if (dt == DT_BINARY || dt == DT_UNSIGNED_CHAR)
            loadedImgsInfo(nLoadedImg+i,7) = nLoadedUchar++;
        else if (dt == DT_SIGNED_SHORT)
            loadedImgsInfo(nLoadedImg+i,7) = nLoadedShort++;
        else if (dt == DT_SIGNED_INT)
            loadedImgsInfo(nLoadedImg+i,7) = nLoadedInt++;
        else if (dt == DT_FLOAT)
            loadedImgsInfo(nLoadedImg+i,7) = nLoadedFloat++;
        else if (dt == DT_DOUBLE)
            loadedImgsInfo(nLoadedImg+i,7) = nLoadedDouble++;
        loadedImgsInfo(nLoadedImg+i,8) = uint(w.LabelCheck->isChecked());
        loadedImgsInfo(nLoadedImg+i,9) = 0; // initialize first with 0 = grayscale
        // add entry in menu
        imgName = fi.baseName();
        if (nVolsImg > 1)
            imgName = imgName + " - " + QString::number(i);
        ImageComboBox->addItem(imgName);
    }
    nLoadedImg += nVolsImg;
    delete progress;
    // first image to load without a model
    if (dm.isEmpty() && currentImage == -1) {
        // disable model icon
        actionLoadModelFile->setEnabled(false);
        actionCloseAll->setEnabled(true);
        enableImageControls(true);
    }
    ImageComboBox->setCurrentIndex(nLoadedImg-nVolsImg);
    changeImage(nLoadedImg-nVolsImg);
}
void MainWindow::closeImage()
{
    // don't remove first image of the model
    if (!dm.isEmpty() && currentImage == 0) {
        QMessageBox::warning(this,"Warning","Can not delete the model image");
        return;
    }
    // the volume to be removed is the currentImage
    uint idx = loadedImgsInfo(currentImage,7),
            dt = loadedImgsInfo(currentImage,0);
    // remove from memory
    if (dt == DT_BINARY || dt == DT_UNSIGNED_CHAR) {
        loadedImgs.ucharImg[idx].reset();
        loadedImgs.ucharImg.removeAt(idx);
        nLoadedUchar--;
    }
    else if (dt == DT_SIGNED_SHORT) {
        loadedImgs.shortImg[idx].reset();
        loadedImgs.shortImg.removeAt(idx);
        nLoadedShort--;
    }
    else if (dt == DT_SIGNED_INT) {
        loadedImgs.intImg[idx].reset();
        loadedImgs.intImg.removeAt(idx);
        nLoadedInt--;
    }
    else if (dt == DT_FLOAT) {
        loadedImgs.floatImg[idx].reset();
        loadedImgs.floatImg.removeAt(idx);
        nLoadedFloat--;
    }
    else if (dt == DT_DOUBLE) {
        loadedImgs.doubleImg[idx].reset();
        loadedImgs.doubleImg.removeAt(idx);
        nLoadedDouble--;
    }
    // update information matrix
    loadedImgsInfo.shed_row(currentImage);
    nLoadedImg--;
    for (uint i = currentImage;i<loadedImgsInfo.n_rows;i++)
        if (loadedImgsInfo(i,0) == dt)
            loadedImgsInfo(i,7)--;
    // remove from menu,
    ImageComboBox->setCurrentIndex(qMax(currentImage-1,0));
    ImageComboBox->removeItem(currentImage);
    // update volume, note we set currentImage to -1 in order to account for
    // the case when removing the first image
    int oldImg = currentImage;
    currentImage = -1;
    if (nLoadedImg == 0)
        initalize();
    else
        changeImage(qMax(oldImg-1,0));
}
void MainWindow::saveImage()
{
    if (nLoadedImg == 0)
        return;
    QStringList list;
    for (uint i = 0; i<nLoadedImg;i++)
        list.append(ImageComboBox->itemText(i));
    SaveImageDialog w(list,loadedImgsInfo,this);
    if(w.exec()) {
        // prepare data to be saved
        // get volume information
        // datatype,dx,dy,dz,locationInItsOwnTypeList
        uint loc, idx = w.volsToBeSaved.first();
        ImageFileType ft = w.fileType();
        quint32 nR = loadedImgsInfo(idx,4),
                nC = loadedImgsInfo(idx,5),
                nS = loadedImgsInfo(idx,6),
                dim[] = {nR,nC,nS,w.volsToBeSaved.size()};
        float   dx = loadedImgsInfo(idx,1),
                dy = loadedImgsInfo(idx,2),
                dz = loadedImgsInfo(idx,3),
                pixDim[] = {dx,dy,dz};
        ImageDataType dt = static_cast<ImageDataType>(int(loadedImgsInfo(idx,0)));
        // prepare header
        AnalyzeHeader header = prepareHeader(dim,dt,pixDim,ft);
        // ask for the file
        QString fileName;
        if (ft == ANALYZE)
            fileName = QFileDialog::getSaveFileName(this,"Save Analyze7.5 Image", ".", "(*.hdr)");
        else
            fileName = QFileDialog::getSaveFileName(this,"Save NIFTI Image", ".", "(*.nii)");
        //check for cancel press
        if (fileName.isEmpty())
            return;
        // prepare data and write file
        // get data according to its type
        if ((dt == DT_BINARY) || (dt == DT_UNSIGNED_CHAR)) {
            QList<uchar_cube> data = QList<uchar_cube>();
            for (int i = 0;i<w.volsToBeSaved.size();i++) {
                loc = loadedImgsInfo(w.volsToBeSaved.at(i),7);
                data.append(loadedImgs.ucharImg[loc]);
                header.dime.glmin = qMin(header.dime.glmin,int(data.last().min()));
                header.dime.glmax = qMax(header.dime.glmax,int(data.last().max()));
            }
            saveImages(fileName,ft,header,data);
        }
        else if (dt == DT_SIGNED_SHORT) {
            QList<s16_cube> data = QList<s16_cube>();
            for (int i = 0;i<w.volsToBeSaved.size();i++) {
                loc = loadedImgsInfo(w.volsToBeSaved.at(i),7);
                data.append(loadedImgs.shortImg[loc]);
                header.dime.glmin = qMin(header.dime.glmin,int(data.last().min()));
                header.dime.glmax = qMax(header.dime.glmax,int(data.last().max()));
            }
            saveImages(fileName,ft,header,data);
        }
        else if (dt == DT_SIGNED_INT) {
            QList<s32_cube> data = QList<s32_cube>();
            for (int i = 0;i<w.volsToBeSaved.size();i++) {
                loc = loadedImgsInfo(w.volsToBeSaved.at(i),7);
                data.append(loadedImgs.intImg[loc]);
                header.dime.glmin = qMin(header.dime.glmin,int(data.last().min()));
                header.dime.glmax = qMax(header.dime.glmax,int(data.last().max()));
            }
            saveImages(fileName,ft,header,data);
        }
        else if (dt == DT_FLOAT) {
            QList<fcube> data = QList<fcube>();
            for (int i = 0;i<w.volsToBeSaved.size();i++) {
                loc = loadedImgsInfo(w.volsToBeSaved.at(i),7);
                data.append(loadedImgs.floatImg[loc]);
                header.dime.glmin = qMin(header.dime.glmin,int(data.last().min()));
                header.dime.glmax = qMax(header.dime.glmax,int(data.last().max()));
            }
            saveImages(fileName,ft,header,data);
        }
        else if (dt == DT_DOUBLE) {
            QList<cube> data = QList<cube>();
            for (int i = 0;i<w.volsToBeSaved.size();i++) {
                loc = loadedImgsInfo(w.volsToBeSaved.at(i),7);
                data.append(loadedImgs.doubleImg[loc]);
                header.dime.glmin = qMin(header.dime.glmin,int(data.last().min()));
                header.dime.glmax = qMax(header.dime.glmax,int(data.last().max()));
            }
            saveImages(fileName,ft,header,data);
        }
    }
}
// this function will be called either by the ImageComboBox
// when the user chooses a different image, or programmatically
// in case of opening a model, opening/closing an image
void MainWindow::changeImage(int newVol)
{
    // needed for initialization
    if (nLoadedImg == 0 || newVol < 0 ||
            newVol >= nLoadedImg || newVol == currentImage)
        return;
    currentImage = newVol;
    // get selected volume
    activeVol.clear();
    // get volume and its information
    quint32 dt = loadedImgsInfo(newVol,0),
            idx = loadedImgsInfo(newVol,7);
    if ((dt == DT_BINARY) || (dt == DT_UNSIGNED_CHAR))
        activeVol = conv_to<cube>::from(loadedImgs.ucharImg[idx]);
    else if (dt == DT_SIGNED_SHORT)
        activeVol = conv_to<cube>::from(loadedImgs.shortImg[idx]);
    else if (dt == DT_SIGNED_INT)
        activeVol = conv_to<cube>::from(loadedImgs.intImg[idx]);
    else if (dt == DT_FLOAT)
        activeVol = conv_to<cube>::from(loadedImgs.floatImg[idx]);
    else if (dt == DT_DOUBLE)
        activeVol = loadedImgs.doubleImg[idx];
    dxImg = loadedImgsInfo(newVol,1);
    dyImg = loadedImgsInfo(newVol,2);
    dzImg = loadedImgsInfo(newVol,3);
    activeImgIsLabel = loadedImgsInfo(newVol,8);
    activeImageChanged();
    fill_VOI_List();
}
void MainWindow::activeImageChanged()
{
    // This function assumes a new Active volume was chosen
    int newVolIdx = ImageComboBox->currentIndex();
    // get volume dimension and information
    nRowsImg = activeVol.n_rows;
    nColsImg = activeVol.n_cols;
    nSlicesImg = activeVol.n_slices;
    dxByDyImg = dxImg/dyImg;
    dzByDxImg = dzImg/dxImg;
    dzByDyImg = dzImg/dyImg;
    minActVol = activeVol.min();
    maxActVol = activeVol.max();
    // select slices
    selCorSlice = qFloor(nRowsImg/2);
    selSagSlice = qFloor(nColsImg/2);
    selAxSlice = qFloor(nSlicesImg/2);
    // update status bar
    ActVolInfo->setText(QString("Active Image res: %1 X %2 X %3")
                        .arg(dxImg).arg(dyImg).arg(dzImg));
    // update spinners without updating slices
    allowCorUpdate = false;
    allowSagUpdate = false;
    allowAxUpdate = false;
    CoronalSpinner->setMaximum(nRowsImg-1);
    SagittalSpinner->setMaximum(nColsImg-1);
    AxialSpinner->setMaximum(nSlicesImg-1);
    allowCorUpdate = true;
    allowSagUpdate = true;
    allowAxUpdate = true;
    // clean 3D area
    // update LUT, if LUT value change, it will call synchviews
    allowLUTupdate = false;
    if (LUT_Box->currentIndex() != loadedImgsInfo(newVolIdx,9))
        LUT_Box->setCurrentIndex(loadedImgsInfo(newVolIdx,9));
    allowLUTupdate = true;
    // fill the 3D volume with activeVol and plot orthogonal cuts
    VTKWidget->setVolume(activeVol,(!activeImgIsLabel ||
                                    (loadedImgsInfo(newVolIdx,9)==0)),
                         dxImg,dyImg,dzImg,minActVol,maxActVol);
    draw3Ddata();
    // disable synch first to save time since each one of the spinners will
    // try to sych after updating the slice
    allowSychronize = false;
    CoronalSpinner->setValue(selCorSlice);
    SagittalSpinner->setValue(selSagSlice);
    AxialSpinner->setValue(selAxSlice);
    allowSychronize = true;
    resetSize = true;
    synchronizeViews();
    // enable LUT option if appropriate
    Load_LUT_Box->setEnabled(activeImgIsLabel);
}
void MainWindow::clearLoadedImages()
{
    if (nLoadedImg > 0) {
        uint dt, idx;
        for (uint i=0;i<nLoadedImg; i++) {
            dt = loadedImgsInfo(i,0);
            idx = loadedImgsInfo(i,7);
            if (dt == DT_BINARY || dt == DT_UNSIGNED_CHAR)
                loadedImgs.ucharImg[idx].clear();
            else if (dt == DT_SIGNED_SHORT)
                loadedImgs.shortImg[idx].clear();
            else if (dt == DT_SIGNED_INT)
                loadedImgs.intImg[idx].clear();
            else if (dt == DT_FLOAT)
                loadedImgs.floatImg[idx].clear();
            else if (dt == DT_DOUBLE)
                loadedImgs.doubleImg[idx].clear();
        }
        loadedImgs.ucharImg.clear();
        loadedImgs.shortImg.clear();
        loadedImgs.intImg.clear();
        loadedImgs.floatImg.clear();
        loadedImgs.doubleImg.clear();
        // no img files loaded by type            
        nLoadedUchar = nLoadedShort = nLoadedInt = 0;
        nLoadedFloat = nLoadedDouble = nLoadedImg = 0;
    }
}

// handle zoom buttons
// note the ROI flags are updated through the UncheckROI_Button behavior
void MainWindow::zoomInPressed(bool status) { UncheckROI_Button->setChecked(status); }
void MainWindow::zoomOutPressed(bool status) { UncheckROI_Button->setChecked(status); }

// LUT stuff
void MainWindow::openLUT()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open color LUT", ".",
                                                    QString("text LUT files (*.txt)\n"
                                                       "All files (*.*)\n"));
    if (!fileName.isEmpty()) {
        if (readLUT(fileName))
            LUT_Box->insertItem(LUT_Box->count(),
                                QFileInfo(fileName).baseName());
    }
}
bool MainWindow::readLUT(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Warning!", QString("Cannot read file %1:\n%2.")
                             .arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    // read the file
    QTextStream in(&file);
    QStringList fields,tmpLabels;
    QVector<QRgb> tmp;
    QList<int> tmp2;
    bool ok1, ok2, ok3, ok4;
    int maxLabel = 0;
    while(!in.atEnd()) {
        // read line and ignore empty lines or starting lines with #
        QString line = in.readLine();
        if (line.isEmpty() || line[0] == '#')
            continue;
        // parse text, expected 5 fields
        // No. Label Name:  R   G   B   A
        fields = line.split(QRegExp("(\\s+)"), QString::SkipEmptyParts);
        if (fields.size() < 5) {
            QMessageBox::critical(this, "Error", "Can't read LUT.");
            return false;
        }
        // use the parsed texts
        tmp2.append(fields.at(0).toInt(&ok1));      // scalar value
        tmpLabels.append(fields[1]);                // label name
        tmp.append(qRgb(fields.at(2).toInt(&ok2),   // color
                        fields.at(3).toInt(&ok3),
                        fields.at(4).toInt(&ok4)));
        if (!ok1 || !ok2 || !ok3 || !ok4) {
            QMessageBox::critical(this, "Error", "Can't read LUT.");
            return false;
        }
        // keep the max label value
        maxLabel = qMax(maxLabel,tmp2.last());
    }
    file.close();
    // create the colortable, we must create
    // an array starting with 0 to the max scalar value
    QStringList tmpLabels2;
    QVector<QRgb> tmpLUT;
    // create LUT and label lists and initialize them with zeros and unkown
    for (int i=0;i<maxLabel+1;i++) {
        tmpLUT.append(qRgb(0,0,0));
        tmpLabels2.append("Unkown");
    }
    // fill the arrays with the data read from the file
    int idx;
    for (int i=0;i<tmp2.size();i++) {
        idx = tmp2[i];                    // location in the LUT
        tmpLUT[idx] = tmp[i];             // color
        tmpLabels2[idx] = tmpLabels[i];   // label name
    }
    cTables.append(tmpLUT);
    cTablesLabels.append(tmpLabels2);
    return true;
}
void MainWindow::changeLUT(int sel)
{

    /*
      need to fix the problem when the chosen LUT is for labels and the
      label file have values > LUT
      */

    cTable = cTables[sel];   // update color LUT
    if (!allowLUTupdate)
        return;
    loadedImgsInfo(currentImage,9) = sel; // update LUT choice
    // clean 3D area
    VTKWidget->clear();
    // fill the 3D volume with activeVol
    VTKWidget->setVolume(activeVol,(sel==0),dxImg,dyImg,dzImg,
                         minActVol,maxActVol);
    draw3Ddata();
    synchronizeViews();         // update views
    fill_VOI_List();
}
void MainWindow::deleteLUT()
{
    // ignore when first two LUT are chosen
    if (LUT_Box->currentIndex() < 2)
        return;
    int LUT_ToBeRemoved = LUT_Box->currentIndex();
    // remove LUT from menu
    LUT_Box->setCurrentIndex(0);
    LUT_Box->removeItem(LUT_ToBeRemoved);
    //  update volumes info, volumes using same LUT configure them grayscale
    for (uint i = 0; i<nLoadedImg; i++) {
        if (loadedImgsInfo(i,9) == LUT_ToBeRemoved)
            loadedImgsInfo(i,9) = 0;
        else if (loadedImgsInfo(i,9) > LUT_ToBeRemoved)
            loadedImgsInfo(i,9) = loadedImgsInfo(i,9) -1;
    }
}
// fiber stuff
void MainWindow::openFiber()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Fiber File", ".",
                                                    QString("Fiber files (*.fib *.dat)\n"
                                                            "TractCreator fiber files (*.fib)\n"
                                                            "DTIStudio fiber files (*.dat)\n"));
    if (fileName.isEmpty())
        return;
    QFileInfo fi(fileName);
    if ( fi.suffix() == "dat" || fi.suffix() == "fib") {
        DiffusionModelDimension dmp = {
            dm.nRows,dm.nCols,dm.nSlices,
            dm.dx,dm.dy,dm.dz
        };
        // check for existing fibers and ask to append
        bool append = false;
        if (!fibers.isEmpty())
            append = (QMessageBox::Yes == QMessageBox(QMessageBox::Warning, "CoNECt",
                                                      "Append new fibers to existing ones?",
                                                      QMessageBox::Yes|QMessageBox::No).exec());
        MyProgressDialog *prog = new MyProgressDialog(this);
        prog->disableCancel();
        prog->disableMain();
        if(fibers.readFiber(fileName,dmp,prog,append)) {
            enableFiberControls(true);
            DisplayNone->setChecked(true);
            FiberROI_Box->setEnabled(false);
            FiberManagementBox->setEnabled(false);
            // update length filter
            FiberLengthSlider->setMaximum(qCeil(fibers.maxLength()));
            FiberLengthSlider->setUpperPosition(FiberLengthSlider->maximum());
            LeftSliderLabel->setText("0");
            RightSliderLabel->setText(QString::number(qCeil(fibers.maxLength())));
            FiberVisualizationBox->setEnabled(false);
            // clear ROI -> includes visFibers
            clearROIs();
        }
        delete prog;
    }
    fill_VOI_List();
}
void MainWindow::saveFiber()
{
    if (fibers.isEmpty()) {
        QMessageBox::warning(this, "Save Fibers!", "No Fibers to save.");
        return;
    }
    // construct a dialog to choose between Selected or All fibers
    QDialog *w = new QDialog(this);
    w->setWindowTitle("Save Fibers");
    w->setWindowModality(Qt::ApplicationModal);
    QVBoxLayout *vl = new QVBoxLayout(w);
    vl->setSpacing(6);
    vl->setContentsMargins(11, 11, 11, 11);
    QRadioButton *rb1 = new QRadioButton("All Fibers",w),
                *rb2 = new QRadioButton("Selected Fibers",w);
    rb1->setChecked(true);
    rb2->setEnabled(!selectedFibersList.isEmpty());
    vl->addWidget(rb1);
    vl->addWidget(rb2);
    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Cancel|
                                                QDialogButtonBox::Ok,
                                                Qt::Horizontal,w);
    bb->setCenterButtons(true);
    vl->addWidget(bb);
    connect(bb, SIGNAL(accepted()), w, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), w, SLOT(reject()));
    w->setFixedSize(w->sizeHint());
    // get user input
    if (w->exec()) {
        QString fileName = QFileDialog::getSaveFileName(this,"Save Fiber File", ".", "(*.fib)");
        if (fileName.isEmpty()) //check for cancel press
            return;
        // assure it is the right extension
        QFileInfo fi(fileName);
        if (fi.suffix() != "fib")
            fileName.append(".fib");
        // prepare list to be saved
        QList<quint32> saveIdx;
        if (rb1->isChecked()) {
            saveIdx.reserve(fibers.size());
            for(int i = 0; i<fibers.size();i++)
                saveIdx.append(i);
        }
        else
            saveIdx = selectedFibersList.last();
        fibers.filterFibersByLength(saveIdx,FiberLengthSlider->lowerValue(),
                                    FiberLengthSlider->upperValue());
        MyProgressDialog *prog = new MyProgressDialog(this);
        prog->disableCancel();
        prog->disableMain();
        if (!fibers.writeFiber(fileName,saveIdx,prog)) {
            QFile::remove(fileName);
            QMessageBox::critical(this, "Warning!", QString("Cannot write file %1.")
                                  .arg(fileName));
        }
        delete prog;
    }
}

// Change the images when chosing another slice direction
void MainWindow::changeCutDirection()
{
    allowMainUpdate = false; // prevent any update in the slice until done    
    if(CoronalVectorButton->isChecked()) {
        selModelSlice = qMin(selCorSlice,dm.nRows-1);
        VectorSliceSpinner->setMaximum(dm.nRows-1);
    }
    else if(SagitalVectorButton->isChecked()) {
        selModelSlice = qMin(selSagSlice,dm.nCols-1);
        VectorSliceSpinner->setMaximum(dm.nCols-1);
    }
    else if(AxialVectorButton->isChecked()) {
        selModelSlice = qMin(selAxSlice,dm.nSlices-1);
        VectorSliceSpinner->setMaximum(dm.nSlices-1);
    }
    VectorSliceSpinner->setValue(selModelSlice);
    updateColorSlice(); // update mainColoredImage
    updateVectors();    // update vectors directions in the main slice
    allowMainUpdate = true;
    // update Labels size according to current situation
    resetSize = true;
    updateVectorSize();
    resetSize = false;
}
// update main slice when scrolling
void MainWindow::updateMainSlice(int slice)
{
    if (!allowMainUpdate || dm.isEmpty())
        return;
    // update slice
    selModelSlice = slice;
    // update spinners and Scenes
    if(CoronalVectorButton->isChecked()) {
        selCorSlice = qMin(selModelSlice,nRowsImg-1);
        allowCorUpdate = false;
        CoronalSpinner->setValue(selCorSlice);
        allowCorUpdate = true;
    }
    else if(SagitalVectorButton->isChecked()) {
        selSagSlice = qMin(selModelSlice,nColsImg-1);
        allowSagUpdate = false;
        SagittalSpinner->setValue(selSagSlice);
        allowSagUpdate = true;
    }
    else if(AxialVectorButton->isChecked()) {
        selAxSlice = qMin(selModelSlice,nSlicesImg-1);
        allowAxUpdate = false;
        AxialSpinner->setValue(selAxSlice);
        allowAxUpdate = true;
    }
    synchronizeViews();
}
// update images
/*
    Note that, we made uchar_mat b outside the if condition in order to make
    the memory of the b mat available outside the if condition. When the declaration
    of b was inside the if condition, it used to cause crashing when adding the tmp
    QImage to the view.
 */
void MainWindow::updateCoronalSlice()
{
    // update 2D
    QImage tmp;
    uchar_mat b;
    if (LUT_Box->currentIndex() == 0) {
        mat a = 255.0*((activeVol(span(selCorSlice),span::all,span::all)
                        -minActVol)/(maxActVol-minActVol));
        b = conv_to<uchar_mat>::from(a);
        tmp = QImage(b.memptr(),nColsImg,nSlicesImg,nColsImg,QImage::Format_Indexed8);
        tmp.setColorTable(cTable);
    }
    else {

        tmp = QImage(nColsImg, nSlicesImg,QImage::Format_RGB32);
        uint *pixel = (uint *) tmp.bits();
        for (uint j=0; j<nSlicesImg;++j)
            for (uint i=0; i<nColsImg;++i)
                pixel[i+j*nColsImg] = cTable.at( activeVol(selSagSlice,i,j) );
//                tmp.setPixel(i,j,cTable.at( int(activeVol(selCorSlice,i,j)) ));
    }
    QPen pen = QPen(Qt::DashLine);
    pen.setWidthF(0.2);
    CoronalView->scene()->clear();
    CoronalView->scene()->addPixmap(QPixmap::fromImage(tmp));
    pen.setColor(Qt::blue);
    CoronalView->scene()->addLine(QLineF(0,selAxSlice+0.5,nColsImg,selAxSlice+0.5),pen);
    pen.setColor(Qt::green);
    CoronalView->scene()->addLine(QLineF(selSagSlice+0.5,0,selSagSlice+0.5,nSlicesImg),pen);
    CoronalView->setSceneRect(0,0,tmp.width(),tmp.height());
    // update 3D
    VTKWidget->changeCoronalSlice(selCorSlice);
}
void MainWindow::updateSagittalSlice()
{
    // update 2D
    QImage tmp;
    uchar_mat b;
    if (LUT_Box->currentIndex() == 0) {
        mat a = 255.0*((activeVol(span::all,span(selSagSlice),span::all)
                        -minActVol)/(maxActVol-minActVol));
        b = conv_to<uchar_mat>::from(a);
        tmp = QImage(b.memptr(),nRowsImg,nSlicesImg,nRowsImg,QImage::Format_Indexed8);
        tmp.setColorTable(cTable);
    }
    else {
        tmp = QImage(nRowsImg, nSlicesImg,QImage::Format_RGB32);
        uint *pixel = (uint *) tmp.bits();
        for (uint j=0; j<nSlicesImg;++j)
            for (uint i=0; i<nRowsImg;++i)
                // this technique is much faster (4 times) than setPixel
                pixel[i+j*nRowsImg] = cTable.at( activeVol(i,selSagSlice,j) );
//                 tmp.setPixel(i,j,cTable.at( activeVol(i,selSagSlice,j) ));
    }
    QPen pen = QPen(Qt::DashLine);
    pen.setWidthF(0.2);
    SagittalView->scene()->clear();
    SagittalView->scene()->addPixmap(QPixmap::fromImage(tmp));
    pen.setColor(Qt::blue);
    SagittalView->scene()->addLine(QLineF(0,selAxSlice+0.5,nRowsImg,selAxSlice+0.5),pen);
    pen.setColor(Qt::red);
    SagittalView->scene()->addLine(QLineF(selCorSlice+0.5,0,selCorSlice+0.5,nSlicesImg),pen);
    SagittalView->setSceneRect(0,0,tmp.width(),tmp.height());
    // update 3D
    VTKWidget->changeSagittalSlice(selSagSlice);
}
void MainWindow::updateAxialSlice()
{
    // update 2D
    QImage tmp;
    uchar_mat b;
    if (LUT_Box->currentIndex() == 0) {
        mat a = 255.0*((activeVol(span::all,span::all,span(selAxSlice))
                        -minActVol)/(maxActVol-minActVol));
        b = strans(conv_to<uchar_mat>::from(a));
        tmp = QImage(b.memptr(),nColsImg,nRowsImg,nColsImg,QImage::Format_Indexed8);
        tmp.setColorTable(cTable);
    }
    else {
        tmp = QImage(nColsImg,nRowsImg, QImage::Format_RGB32);
        uint *pixel = (uint *) tmp.bits();
        for (uint j=0; j<nRowsImg;++j)
            for (uint i=0; i<nColsImg;++i)
                pixel[i+j*nColsImg] = cTable.at( activeVol(j,i,selAxSlice) );
//                tmp.setPixel(i,j,cTable.at( activeVol(j,i,selAxSlice) ));
    }
    QPen pen = QPen(Qt::DashLine);
    pen.setWidthF(0.2);
    AxialView->scene()->clear();
    AxialView->scene()->addPixmap(QPixmap::fromImage(tmp));
    pen.setColor(Qt::red);
    AxialView->scene()->addLine(QLineF(0,selCorSlice+0.5,nColsImg,selCorSlice+0.5),pen);
    pen.setColor(Qt::green);
    AxialView->scene()->addLine(QLineF(selSagSlice+0.5,0,selSagSlice+0.5,nRowsImg),pen);
    AxialView->setSceneRect(0,0,tmp.width(),tmp.height());
    // update 3D
    VTKWidget->changeAxialSlice(selAxSlice);
}

// update spinners
void MainWindow::onCoronalSpinnerChange(int SelSlice)
{
    if (!allowCorUpdate)
        return;
    selCorSlice = SelSlice;
    if (CoronalVectorButton->isChecked() && !dm.isEmpty()) {
        selModelSlice = qMin(selCorSlice,dm.nRows-1);
        allowMainUpdate = false;
        VectorSliceSpinner->setValue(selCorSlice);
        allowMainUpdate = true;
    }
    synchronizeViews();
}
void MainWindow::onSagittalSpinnerChange(int SelSlice)
{
    if (!allowSagUpdate)
        return;
    selSagSlice = SelSlice;
    if (SagitalVectorButton->isChecked()  && !dm.isEmpty()) {
        selModelSlice = qMin(selSagSlice,dm.nCols-1);
        allowMainUpdate = false;
        VectorSliceSpinner->setValue(selSagSlice);
        allowMainUpdate = true;
    }
    synchronizeViews();
}
void MainWindow::onAxialSpinnerChange(int SelSlice)
{
    if (!allowAxUpdate)
        return;
    selAxSlice = SelSlice;
    if (AxialVectorButton->isChecked() && !dm.isEmpty()) {
        selModelSlice = qMin(selAxSlice,dm.nSlices-1);
        allowMainUpdate = false;
        VectorSliceSpinner->setValue(selAxSlice);
        allowMainUpdate = true;
    }
    synchronizeViews();
}

// update slices sizes when slices change
// for the 3 images, they are already mirrored during creation
// the vector image should be mirrored upside down, that is why we send a negative factor
void MainWindow::updateCoronalSize()
{
    if (CoronalView->scene()->isActive() && resetSize)
        CoronalView->resetSize(dzByDyImg);
}
void MainWindow::updateSagittalSize()
{
    if (SagittalView->scene()->isActive() && resetSize)
        SagittalView->resetSize(dzByDxImg);
}
void MainWindow::updateAxialSize()
{
    if (AxialView->scene()->isActive() && resetSize)
        AxialView->resetSize(dxByDyImg);
}
void MainWindow::updateVectorSize()
{
    if (!dm.isEmpty() && resetSize) {
        if (CoronalVectorButton->isChecked())
            VectorsView->resetSize(-dm.dz/dm.dy);
        else if (SagitalVectorButton->isChecked())
            VectorsView->resetSize(-dm.dz/dm.dx);
        else if (AxialVectorButton->isChecked())
            VectorsView->resetSize(-dm.dx/dm.dy);
    }
}
void MainWindow::updateLabelsSize()
{
    updateCoronalSize();
    updateSagittalSize();
    updateAxialSize();
    updateVectorSize();
    // leave the user the control on zoom
    resetSize = false;
}
void MainWindow::synchronizeViews()
{
    // this function updates the vector slice
    // as well as the 3D orthogonal cuts (inlcuding the 3D)
    if (!allowSychronize)
        return;
    if (!dm.isEmpty()) {
        updateColorSlice();     // update mainColoredImage
        updateVectors();        // update vectors directions in the main slice
    }
    if (nLoadedImg > 0) {
        updateCoronalSlice();   // update coronal slice
        updateSagittalSlice();  // update sagital slice
        updateAxialSlice();     // update axialActor slice
        updateLabelsSize();     // update Labels size according to current situation
    }
}
void MainWindow::resizeWhenSizeChanged()
{
    // update labels sizes when one of the splitters move
    // if the view is smaller than the best fit, correct that
    // if the user is zooming, don't change
    resetSize = (!CoronalView->horizontalScrollBar()->isVisible()
                 && !CoronalView->verticalScrollBar()->isVisible());
    updateCoronalSize();
    resetSize = (!SagittalView->horizontalScrollBar()->isVisible()
                 && !SagittalView->verticalScrollBar()->isVisible());
    updateSagittalSize();
    resetSize = (!AxialView->horizontalScrollBar()->isVisible()
                 && !AxialView->verticalScrollBar()->isVisible());
    updateAxialSize();
    resetSize = (!VectorsView->horizontalScrollBar()->isVisible() &&
                 !VectorsView->verticalScrollBar()->isVisible());
    updateVectorSize();
    resetSize = false;
    // update scalar bar position
    if (!fibers.isEmpty() && ColorMethod->currentIndex()== SCALAR)
        VTKWidget->updateScalarBarPosition();
}

void MainWindow::clearROIs()
{
    // clear variables
    freeHandROIs.clear();
    ovalROIs.clear();
    rectROIs.clear();
    ROIorder.clear();
    filteringActions.clear();
    ROI_Info.set_size(0,4);
    // remove ROI from scene
    for (int i = 0; i< ROIs.size(); i++)
        VectorsView->scene()->removeItem(ROIs.at(i));
    while (!ROIs.isEmpty())
        delete ROIs.takeLast();
    ROIs.clear();
    // clear selected tracts
    selectedFibersList.clear();
    currentSelectedFibers.clear();
    for (int i = 0; i<fibers.size();i++)
        currentSelectedFibers.append(i);
    // disable undo
    UndoROI_Button->setEnabled(false);
    // clear ROI-VOI List
    ROI_VOI_List->clear();
    visFibers();
}

// Manage mouse motion over slice
void MainWindow::handleMouseMoveCoronal(QMouseEvent *ev)
{
    // get the implification factor
    QPointF pt = CoronalView->mapToScene(ev->pos());
    if (!CoronalView->sceneRect().contains(pt)) {
        LocationLabel->setText("Out of frame.");
        return;
    }
    quint32 x = selCorSlice,
            y = qFloor(pt.x()),
            z = qFloor(pt.y());
    y = qMin(y,nColsImg-1); z = qMin(z,nSlicesImg-1);
    double tmp = activeVol(x,y,z);
    int idx = LUT_Box->currentIndex();
    if (!activeImgIsLabel || idx == 0)
        LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4)")
                               .arg(x).arg(y).arg(z).arg(tmp));
    else if (activeImgIsLabel && !cTablesLabels.at(idx).isEmpty()) {
        if (tmp < cTablesLabels.at(idx).size() && double(int(tmp)) == tmp)
            LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4: %5)")
                                   .arg(x).arg(y).arg(z).arg(tmp)
                                   .arg(cTablesLabels.at(idx).at(int(tmp))));
        else
            LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4: Invalid Label)")
                                   .arg(x).arg(y).arg(z).arg(tmp));
    }
}
void MainWindow::handleMouseMoveSagittal(QMouseEvent *ev)
{
    // get the implification factor
    QPointF pt = SagittalView->mapToScene(ev->pos());
    if (!SagittalView->sceneRect().contains(pt)) {
        LocationLabel->setText("Out of frame.");
        return;
    }
    quint32 x = qFloor(pt.x()),
            y = selSagSlice,
            z = qFloor(pt.y());
    x = qMin(x,nRowsImg-1); z = qMin(z,nSlicesImg-1);
    double tmp = activeVol(x,y,z);
    int idx = LUT_Box->currentIndex();
    if (!activeImgIsLabel || idx == 0)
        LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4)")
                               .arg(x).arg(y).arg(z).arg(tmp));
    else if (activeImgIsLabel && !cTablesLabels.at(idx).isEmpty()) {
        if (tmp < cTablesLabels.at(idx).size() && double(int(tmp)) == tmp)
            LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4: %5)")
                                   .arg(x).arg(y).arg(z).arg(tmp)
                                   .arg(cTablesLabels.at(idx).at(int(tmp))));
        else
            LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4: Invalid Label)")
                                   .arg(x).arg(y).arg(z).arg(tmp));
    }
}
void MainWindow::handleMouseMoveAxial(QMouseEvent *ev)
{
    // get the implification factor
    QPointF pt = AxialView->mapToScene(ev->pos());
    if (!AxialView->sceneRect().contains(pt)) {
        LocationLabel->setText("Out of frame.");
        return;
    }
    quint32 x = qFloor(pt.y()),
            y = qFloor(pt.x()),
            z = selAxSlice;
    x = qMin(x,nRowsImg-1); y = qMin(y,nColsImg-1);
    double tmp = activeVol(x,y,z);
    int idx = LUT_Box->currentIndex();
    if (!activeImgIsLabel || idx == 0)
        LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4)")
                               .arg(x).arg(y).arg(z).arg(tmp));
    else if (activeImgIsLabel && !cTablesLabels.at(idx).isEmpty()) {
        if (tmp < cTablesLabels.at(idx).size() && double(int(tmp)) == tmp)
            LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4: %5)")
                                   .arg(x).arg(y).arg(z).arg(tmp)
                                   .arg(cTablesLabels.at(idx).at(int(tmp))));
        else
            LocationLabel->setText(QString("X: %1, Y: %2, Z: %3 = (%4: Invalid Label)")
                                   .arg(x).arg(y).arg(z).arg(tmp));
    }
}
// Manage keyboard press
void MainWindow::handleKeyPressCoronal( QKeyEvent *ev )
{
    switch(ev->key())
    {
    case Qt::Key_Up:
        if (selAxSlice < dm.Mask.n_slices-2) {
            selAxSlice++;
            updateAxialSlice();
        }
    case Qt::Key_Down:
        if (selAxSlice > 0) {
            selAxSlice--;
            updateAxialSlice();
        }
    case Qt::Key_Left:
        if (selSagSlice < dm.Mask.n_cols-2) {
            selSagSlice++;
            updateSagittalSlice();
        }
    case Qt::Key_Right:
        if (selSagSlice > 0) {
            selSagSlice--;
            updateSagittalSlice();
        }
    }
}
// Manage mouse press over slice
void MainWindow::handleMousePressCoronal(QMouseEvent *ev)
{
    if (ev->button() == Qt::MidButton)
    {
        if (ZoomInButton->isChecked())
            CoronalView->scale(1.1,1.1);
        else if (ZoomOutButton->isChecked())
            CoronalView->scale(1/1.1,1/1.1);
        else {
            QPointF pt = CoronalView->mapToScene(ev->pos());
            selSagSlice = qFloor(pt.x());
            selAxSlice = qFloor(pt.y());
            // update perpendicular slices
            SagittalSpinner->setValue(selSagSlice);
            AxialSpinner->setValue(selAxSlice);
        }
    }
}
void MainWindow::handleMousePressSagittal(QMouseEvent *ev)
{
    if (ev->button() == Qt::MidButton)
    {
        if (ZoomInButton->isChecked())
            SagittalView->scale(1.1,1.1);
        else if (ZoomOutButton->isChecked())
            SagittalView->scale(1/1.1,1/1.1);
        else {
            QPointF pt = SagittalView->mapToScene(ev->pos());
            selCorSlice = qFloor(pt.x());
            selAxSlice = qFloor(pt.y());
            // update perpendicular slices
            CoronalSpinner->setValue(selCorSlice);
            AxialSpinner->setValue(selAxSlice);
        }
    }
}
void MainWindow::handleMousePressAxial(QMouseEvent *ev)
{
    if (ev->button() == Qt::MidButton)
    {
        if (ZoomInButton->isChecked())
            AxialView->scale(1.1,1.1);
        else if (ZoomOutButton->isChecked())
            AxialView->scale(1/1.1,1/1.1);
        else {
            QPointF pt = AxialView->mapToScene(ev->pos());
            selCorSlice = qFloor(pt.y());
            selSagSlice = qFloor(pt.x());
            // update perpendicular slices
            CoronalSpinner->setValue(selCorSlice);
            SagittalSpinner->setValue(selSagSlice);
        }
    }
}
// Manage main slice view
void MainWindow::updateColorSlice()
{
    if (dm.isEmpty())
        return;
    // get current slice and orientation
    if(CoronalVectorButton->isChecked()) {
        mainColoredImage = QImage(dm.nCols, dm.nSlices,QImage::Format_RGB888);
        mainColoredImage.fill(Qt::black);
        for (uint i=0; i<dm.nCols;i++)
            for (uint j=0; j<dm.nSlices;j++)
                if (dm.Mask(selModelSlice,i,j))
                    mainColoredImage.setPixel(i,j,qRgb(int( Ry(selModelSlice,i,j) ),
                                                       int( Rx(selModelSlice,i,j) ),
                                                       int( Rz(selModelSlice,i,j) ) ) );
    }
    else if(SagitalVectorButton->isChecked()) {
        mainColoredImage = QImage(dm.nRows, dm.nSlices,QImage::Format_RGB888);
        mainColoredImage.fill(Qt::black);
        for (uint i=0; i<dm.nRows;i++)
            for (uint j=0; j<dm.nSlices;j++)
                if (dm.Mask(i,selModelSlice,j))
                    mainColoredImage.setPixel(i,j,qRgb(int( Ry(i,selModelSlice,j) ),
                                                       int( Rx(i,selModelSlice,j) ),
                                                       int( Rz(i,selModelSlice,j) ) ) );
    }
    else if(AxialVectorButton->isChecked()) {
        mainColoredImage = QImage(dm.nCols, dm.nRows,QImage::Format_RGB888);
        mainColoredImage.fill(Qt::black);
        for (uint i=0; i<dm.nCols;i++)
            for (uint j=0; j<dm.nRows;j++)
                if (dm.Mask(j,i,selModelSlice))
                    mainColoredImage.setPixel(i,j,qRgb(int( Ry(j,i,selModelSlice) ),
                                                       int( Rx(j,i,selModelSlice) ),
                                                       int( Rz(j,i,selModelSlice) ) ) );
    }
    mainColoredImage = mainColoredImage.transformed(QTransform(1,0,0,0,-1,0,0,0,1));
}

// update the vectors in the MainView
void MainWindow::updateVectors()
{
    if (dm.isEmpty())
        return;
    if (dm.type() == TDF)
        updateVectorsTOD();
    else if (dm.type() == DTI)
        updateVectorsDTI();

}

void MainWindow::updateVectorsDTI()
{
    if (dm.isEmpty() || dm.type() != DTI)
        return;
    /*
    float delta = 0.5;

    // now draw vectors
    ROIs.clear(); // needed for changes in Gradient table
    VectorsView->scene()->clear();
    QVarLengthArray<QLineF> lineData;
    float posX, posY, negX, negY;
    if(CoronalVectorButton->isChecked())
    {
        for (int x = 0; x < mainColoredImage.width(); x++) {
            for (int y = 0; y < mainColoredImage.height(); y++) {
                if (dm.Mask(selModelSlice,x,y)) {
                    posX = delta*dm.eVecPx(selModelSlice,x,y); negX = posX;
                    posY = delta*dm.eVecPz(selModelSlice,x,y); negY = posY;
                    lineData.append(QLineF(-negX+0.5,negY+0.5,posX+0.5,-posY+0.5 ) );
                }
                VectorsView->scene()->addItem(new Vectors(
                                                   QColor(mainColoredImage.pixel(x, y)),
                                                   x, y, lineData));
                lineData.clear();
            }
        }
    }
    else if(SagitalVectorButton->isChecked())
    {
        for (int x = 0; x < mainColoredImage.width(); x++) {
            for (int y = 0; y < mainColoredImage.height(); y++) {
                if (dm.Mask(x,selModelSlice,y)) {
                    posX = delta*dm.eVecPy(x,selModelSlice,y); negX = posX;
                    posY = delta*dm.eVecPz(x,selModelSlice,y); negY = posY;
                    lineData.append(QLineF(-negX+0.5,negY+0.5,posX+0.5,-posY+0.5 ) );
                }
                VectorsView->scene()->addItem(new Vectors(
                                                   QColor(mainColoredImage.pixel(x, y)),
                                                   x, y, lineData));
                lineData.clear();
            }
        }
    }
    else if(AxialVectorButton->isChecked())
    {
        for (int x = 0; x < mainColoredImage.width(); x++) {
            for (int y = 0; y < mainColoredImage.height(); y++) {
                if (dm.Mask(x,y,selModelSlice)) {
                    posX = delta*dm.eVecPx(x,y,selModelSlice); negX = posX;
                    posY = delta*dm.eVecPy(x,y,selModelSlice); negY = posY;
                    lineData.append(QLineF(-negX+0.5,negY+0.5,posX+0.5,-posY+0.5 ) );
                }
                VectorsView->scene()->addItem(new Vectors(
                                                   QColor(mainColoredImage.pixel(x, y)),
                                                   x, y, lineData));
                lineData.clear();
            }
        }
    }
    */
    // now draw vectors
    VectorsView->scene()->clear();
    // needed for changes in Gradient table
    while (!ROIs.isEmpty())
        ROIs.removeLast();
    VectorsView->scene()->addPixmap(QPixmap::fromImage(mainColoredImage));
    VectorsView->scene()->setSceneRect(0,0,
                                        mainColoredImage.width(),
                                        mainColoredImage.height());
}

void MainWindow::updateVectorsTOD()
{
    if (dm.isEmpty() || dm.type() != TDF)
        return;
    /*
    nTenSlice:        corresponds to one slice from n_tensors
    qIdxSlice:        corresponds to one slice from dm.qIndex
    maskSlice:        corresponds to one slice from mask
    TODSlice:         corresponds to one slice from TOD
    jMax:             max value in the X axis for the selected slice
    kMax:             max value in the Y axis for the selected slice
    */
    // get current slice and orientation
    int     jMax = mainColoredImage.width(),
            kMax = mainColoredImage.height(),
            loc;
    uchar_mat maskSlice, nTenSlice;
    uchar_cube qIdxSlice;
    cube TODSlice;
    qIdxSlice.set_size(jMax,kMax,dm.maxNTens);
    TODSlice.set_size(jMax,kMax,dm.maxNTens);
    if(CoronalVectorButton->isChecked())
    {
        maskSlice = dm.Mask(span(selModelSlice),span::all,span::all);
        //        maskSlice = strans(maskSlice); //removed due to update in ARMADILLO
        nTenSlice = dm.nTensors(span(selModelSlice),span::all,span::all);
        //        nTenSlice = strans(nTenSlice); //removed due to update in ARMADILLO
        uint j = selModelSlice;
        for (uint l=0; l<dm.nSlices;l++)
            for (uint k=0; k<dm.nCols;k++)
                for (uint i=0; i<dm.maxNTens;i++) {
                    loc = dm.nRows*dm.nCols*dm.maxNTens*(dm.nSlices-1-l) + dm.nRows*dm.maxNTens*k +
                            dm.maxNTens*j + i;
                    qIdxSlice(k,l,i) = dm.qIndex(loc);
                    TODSlice(k,l,i) = dm.TOD(loc);
                }
    }
    else if(SagitalVectorButton->isChecked())
    {
        maskSlice = dm.Mask(span::all,span(selModelSlice),span::all);
        nTenSlice = dm.nTensors(span::all,span(selModelSlice),span::all);
        uint k = selModelSlice;
        for (uint l=0; l<dm.nSlices;l++)
            for (uint i=0; i<dm.maxNTens;i++)
                for (uint j=0; j<dm.nRows;j++) {
                    loc = dm.nRows*dm.nCols*dm.maxNTens*(dm.nSlices-1-l) + dm.nRows*dm.maxNTens*k +
                            dm.maxNTens*j + i;
                    qIdxSlice(j,l,i) = dm.qIndex(loc);
                    TODSlice(j,l,i) = dm.TOD(loc);
                }
    }
    else if(AxialVectorButton->isChecked())
    {
        maskSlice = strans(dm.Mask.slice(selModelSlice));
        nTenSlice = strans(dm.nTensors.slice(selModelSlice));
        uint l = selModelSlice;
        for (uint k=0; k<dm.nCols;k++)
            for (uint i=0; i<dm.maxNTens;i++)
                for (uint j=0; j<dm.nRows;j++) {
                    loc = dm.nRows*dm.nCols*dm.maxNTens*l + dm.nRows*dm.maxNTens*k +
                            dm.maxNTens*(dm.nRows-1-j) + i;
                    qIdxSlice(k,j,i) = dm.qIndex(loc);
                    TODSlice(k,j,i) = dm.TOD(loc);
                }
    }
    maskSlice = fliplr(maskSlice);
    nTenSlice = fliplr(nTenSlice);

    mat delta = zeros<mat>(jMax,kMax);
    cube qTmp1(jMax,kMax,dm.maxNTens);
    cube qTmp2(jMax,kMax,dm.maxNTens);
    cube qTmp3(jMax,kMax,dm.maxNTens);
    cube LocPosX(jMax,kMax,dm.maxNTens);
    cube LocPosY(jMax,kMax,dm.maxNTens);
    cube LocNegX(jMax,kMax,dm.maxNTens);
    cube LocNegY(jMax,kMax,dm.maxNTens);
    for (uint m = 0; m<dm.maxNTens;m++)
    {
        delta = (0.7)*TODSlice.slice(m)%maskSlice;
        // populating q_tmp from q with q_tmp
        for (int i = 0;i<jMax;i++)
            for (int j = 0;j<kMax;j++) {
                qTmp1(i,j,m) = dm.q(0,(qIdxSlice(i,j,m)));
                qTmp2(i,j,m) = dm.q(1,(qIdxSlice(i,j,m)));
                qTmp3(i,j,m) = dm.q(2,(qIdxSlice(i,j,m)));
            }
        // location computation for the lines,
        if (AxialVectorButton->isChecked()) {
            LocPosX.slice(m) = delta%qTmp2.slice(m)+0.5;
            LocNegX.slice(m) = -delta%qTmp2.slice(m)+0.5;
            LocPosY.slice(m) = -delta%qTmp1.slice(m)+0.5;
            LocNegY.slice(m) = delta%qTmp1.slice(m)+0.5;
        }
        else if (SagitalVectorButton->isChecked()) {
            LocPosX.slice(m) = delta%qTmp1.slice(m)+0.5;
            LocNegX.slice(m) = -delta%qTmp1.slice(m)+0.5;
            LocPosY.slice(m) = -delta%qTmp3.slice(m)+0.5;
            LocNegY.slice(m) = delta%qTmp3.slice(m)+0.5;
        }
        else if (CoronalVectorButton->isChecked()) {
            LocPosX.slice(m) = delta%qTmp2.slice(m)+0.5;
            LocNegX.slice(m) = -delta%qTmp2.slice(m)+0.5;
            LocPosY.slice(m) = -delta%qTmp3.slice(m)+0.5;
            LocNegY.slice(m) = delta%qTmp3.slice(m)+0.5;
        }
    }
    // now draw vectors
    VectorsView->scene()->clear();
    VectorsView->scene()->addPixmap(QPixmap::fromImage(mainColoredImage));
    // needed for changes in Gradient table
    while (!ROIs.isEmpty())
        ROIs.removeLast();
    int nTen;
    QVarLengthArray<QLineF> lineData;
    for (int x = 0; x < jMax; x++) {
        for (int y = 0; y < kMax; y++) {
            nTen = nTenSlice(x,y);
            if (nTen > 0) {
                for (int s = 0; s < nTen; s++) {
                    lineData.append(QLineF(LocNegX.slice(s).at(x,y),
                                           LocNegY.slice(s).at(x,y),
                                           LocPosX.slice(s).at(x,y),
                                           LocPosY.slice(s).at(x,y) ) );
                }
            }
            VectorsView->scene()->addItem(new Vectors(x, y, lineData));
            lineData.clear();
        }
    }
    VectorsView->scene()->setSceneRect(0,0,
                                        mainColoredImage.width(),
                                        mainColoredImage.height());
}
// handle mouse motion press in the MainView -> ROI drawing
void MainWindow::handleMousePressVector(QMouseEvent *ev)
{
    if (ev->button() == Qt::MidButton) {
        if (ZoomInButton->isChecked())
            VectorsView->scale(1.1,1.1);
        else if (ZoomOutButton->isChecked())
            VectorsView->scale(1/1.1,1/1.1);
    }
    else if (ev->button() == Qt::LeftButton && !UncheckROI_Button->isChecked()) {
        QPointF pt = VectorsView->mapToScene(ev->pos());
        if (!VectorsView->scene()->sceneRect().contains(pt))
            return;
        VectorsView->setDragMode(QGraphicsView::NoDrag);
        VectorsView->viewport()->setCursor(Qt::CrossCursor);
        if (RectROI->isChecked() || OvalROI->isChecked()) {
            rectROI_IsShown = RectROI->isChecked();
            ovalROI_IsShown = OvalROI->isChecked();
            rectOvalROI.setTopLeft(pt);
            rectOvalROI.setBottomRight(pt);
        }
        else if (FreeHand->isChecked()) {
            freehandROI_IsShown = true;
            freehandPath.clear();
            freehandPath.append(pt);
        }
        VectorsView->viewport()->update();
    }
}
// handle mouse motion over MainView
void MainWindow::handleMouseMoveVector(QMouseEvent* ev)
{
    // note mapToScene maps from GUI to data space
    QPointF pt = VectorsView->mapToScene(ev->pos());

    if (!VectorsView->scene()->sceneRect().contains(pt)) {
        LocationLabel->setText("Out of frame.");
        return;
    }
    double x,y,z;
    if (CoronalVectorButton->isChecked()) {
        x = selModelSlice;
        y = qFloor(pt.x());
        z = dm.nSlices-1-qFloor(pt.y());
    }
    else if (SagitalVectorButton->isChecked()) {
        x = qFloor(pt.x());
        y = selModelSlice;
        z = dm.nSlices-1-qFloor(pt.y());
    }
    else if (AxialVectorButton->isChecked()) {
        x = dm.nRows-1-qFloor(pt.y());
        y = qFloor(pt.x());
        z = selModelSlice;
    }
    LocationLabel->setText(QString("X: %1, Y: %2, Z: %3").arg(x).arg(y).arg(z));
    // ROI selection
    if (rectROI_IsShown||ovalROI_IsShown) {
        rectOvalROI.setBottomRight(pt);
        VectorsView->viewport()->update();
    }
    else if (freehandROI_IsShown) {
        freehandPath.append(pt);
        VectorsView->viewport()->update();
    }
}
// handle mouse motion release in the MainView
void MainWindow::handleMouseReleaseVector(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton && !UncheckROI_Button->isChecked()) {
        QPointF pt = VectorsView->mapToScene(ev->pos());
        QPen pen;
        pen.setWidthF(0.1);
        pen.setColor(AND->isChecked()?Qt::white:Qt::yellow);
        pen.setStyle(Qt::SolidLine);
//        QGraphicsItem *item;
        roiItem *item = new roiItem(ROIorder.size());

        if (rectROI_IsShown || ovalROI_IsShown || freehandROI_IsShown) {
            // save information about the ROI
            // shape,type,slice,cut
            ROI_Info.insert_rows(ROI_Info.n_rows,1);
            ROI_Info(ROI_Info.n_rows-1,1) = (AND->isChecked()?AND_ROI:NOT_ROI);
            ROI_Info(ROI_Info.n_rows-1,2) = selModelSlice;
            if (CoronalVectorButton->isChecked())
                ROI_Info(ROI_Info.n_rows-1,3) = CORONAL;
            else if (SagitalVectorButton->isChecked())
                ROI_Info(ROI_Info.n_rows-1,3) = SAGITTAL;
            else if (AxialVectorButton->isChecked())
                ROI_Info(ROI_Info.n_rows-1,3) = CORONAL;
        }
        if (rectROI_IsShown || ovalROI_IsShown ) {
            rectOvalROI = rectOvalROI.normalized();
            if (rectOvalROI.width() < 1 || rectOvalROI.height() < 1) {
                rectROI_IsShown = false;
                ovalROI_IsShown = false;
                ROI_Info.shed_row(ROI_Info.n_rows-1);
                return;
            }
            if (rectROI_IsShown) {
                rectROIs.append(rectOvalROI);
                ROIorder.append(RECT); // rectangle
                item->setInfo(AND->isChecked()?AND_ROI:NOT_ROI,RECT,rectOvalROI);
//                item = VectorsView->scene()->addRect(rectOvalROI,pen,QBrush(Qt::NoBrush));
                ROI_Info(ROI_Info.n_rows-1,0) = RECT;
            }
            else {
                ovalROIs.append(rectOvalROI);
                ROIorder.append(OVAL); // oval
                item->setInfo(AND->isChecked()?AND_ROI:NOT_ROI,OVAL,rectOvalROI);
//                item = VectorsView->scene()->addEllipse(rectOvalROI,pen,QBrush(Qt::NoBrush));
                ROI_Info(ROI_Info.n_rows-1,0) = OVAL;
            }
        }
        else if (freehandROI_IsShown) {
            freehandPath.append(pt);
            freehandPath.append(freehandPath.first());
            freeHandROIs.append(freehandPath);
            VectorsView->update(freehandPath.boundingRect().toRect());
            ROIorder.append(FREEHAND); // freehand
            ROI_Info(ROI_Info.n_rows-1,0) = FREEHAND;
            item->setInfo(AND->isChecked()?AND_ROI:NOT_ROI,FREEHAND,freehandPath);

//            item = VectorsView->scene()->addPolygon(freehandPath,pen,QBrush(Qt::NoBrush));
        }
        VectorsView->scene()->addItem(item);
        if (rectROI_IsShown || ovalROI_IsShown || freehandROI_IsShown) {

            item->setZValue(2);
            ROIs.append(item);
            // attach a text label to each item indicating the number
//            QGraphicsTextItem  *txtItem = new
//                    QGraphicsTextItem(QString::number(ROIorder.size()),item);
//            txtItem->setZValue(3);
//            txtItem->setFont(QFont("Times", 1, QFont::Bold));
//            txtItem->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
//            txtItem->setDefaultTextColor(pen.color());
//            txtItem->setTransform(QTransform(1,0,0,0,-1,0,0,0,1),true);
//            txtItem->setPos(10,10);

            // restore view dragging and cursor to + sign
            VectorsView->setDragMode(QGraphicsView::ScrollHandDrag);
            VectorsView->viewport()->setCursor(Qt::CrossCursor);

            freehandROI_IsShown = false;
            rectROI_IsShown = false;
            ovalROI_IsShown = false;

            filterFibersByROI();
        }

    }
}
// draw ROI event according to mouse motion
void MainWindow::handlePaintEventVector(QPaintEvent *)
{
    QStylePainter painter(VectorsView->viewport());
    painter.setPen(QPen(Qt::white, 1));
    if (rectROI_IsShown)
        painter.drawRect(VectorsView->mapFromScene(rectOvalROI.
                                                    normalized()).boundingRect());
    else if (ovalROI_IsShown)
        painter.drawEllipse(VectorsView->mapFromScene(rectOvalROI.
                                                       normalized()).boundingRect());
    else if (freehandROI_IsShown)
        painter.drawPolyline(VectorsView->mapFromScene(freehandPath));
    update();
}
// undo last ROI
void MainWindow::undoLastROI()
{
    if (filteringActions.isEmpty())
        return;
    // if VOI_ACTION, no graphic change need to be done
    if (filteringActions.last() == ROI_ACTION) {
        // remove ROI Graphics from scene
        if (!ROIs.isEmpty()) {
            VectorsView->scene()->removeItem(ROIs.last());
            delete ROIs.takeLast();
        }
        // update variables
        uchar lastROI = ROIorder.last();
        ROIorder.removeLast();
        if ( lastROI == FREEHAND)
            freeHandROIs.removeLast();
        else if (lastROI == OVAL)
            ovalROIs.removeLast();
        else if (lastROI == RECT)
            rectROIs.removeLast();
        ROI_Info.shed_row(ROI_Info.n_rows-1);
    }
    filteringActions.removeLast();
    // disable button for last one
    if (filteringActions.isEmpty())
        UndoROI_Button->setEnabled(false);
    // restore last action
    selectedFibersList.removeLast();
    if (!selectedFibersList.isEmpty())
        currentSelectedFibers = selectedFibersList.last();
    else {
        currentSelectedFibers.clear();
        currentSelectedFibers.reserve(fibers.size());
        for (int i = 0; i<fibers.size();++i)
            currentSelectedFibers.append(i);
    }
    // remove last entry from ROI-VOI List
    ROI_VOI_List->takeTopLevelItem(ROI_VOI_List->topLevelItemCount()-1);
    visFibers();
}
// handle ROI buttons
void MainWindow::freehandROIPressed(bool status) { UncheckZoomButton->setChecked(status); }
void MainWindow::ovalROIPressed(bool status) { UncheckZoomButton->setChecked(status); }
void MainWindow::rectROIPressed(bool status) { UncheckZoomButton->setChecked(status); }

void MainWindow::clearFibers()
{
    resetFiberControls();
    enableFiberControls(false);
}
void MainWindow::visFibers()
{
    if (fibers.isEmpty() ||
            SkipSlider-> value() == 100 ||
            DisplayNone->isChecked() ||
            currentSelectedFibers.isEmpty() ||
            (selectedFibersList.isEmpty() && DisplaySelected->isChecked()) ) {
        // draw nothing
        VTKWidget->clearFibers();
        return;
    }
    // always apply length filter before visualizing
    fibers.filterFibersByLength(currentSelectedFibers,FiberLengthSlider->lowerValue(),
                                FiberLengthSlider->upperValue());
    // check if the number of fibers is too much
    if (currentSelectedFibers.size()*((100.0-SkipSlider->value())/100.0) > 50000.0) {
        int newVal = qFloor (50000.0/currentSelectedFibers.size()*100.0);
        SkipSlider->setValue(100-newVal);
    }
    int skip = 100.0/(100.0-SkipSlider->value());
    VTKWidget->visualizeFibers(fibers,currentSelectedFibers,skip,
                               ColorMethod->currentIndex(),fiberColorOrientation);
}
// handle 3D volume
void MainWindow::draw3Ddata()
{
    if (nLoadedImg == 0)
        return;
    int t[] = {selCorSlice,selSagSlice,selAxSlice};
    bool b[] = {CoronalBox->isChecked(),SagittalBox->isChecked(),AxialBox->isChecked()};
    VTKWidget->drawCurrentVolume(cTable,(LUT_Box->currentIndex() > 0),
                                 t,OpacitySlider->value(),b);
}
void MainWindow::enable3DCoronal(bool choice) { VTKWidget->changeSliceVisibility(CORONAL,choice); }
void MainWindow::enable3DSagittal(bool choice) { VTKWidget->changeSliceVisibility(SAGITTAL,choice); }
void MainWindow::enable3DAxial(bool choice) { VTKWidget->changeSliceVisibility(AXIAL,choice); }
void MainWindow::updateOpacity(int value) { if (nLoadedImg > 0) VTKWidget->updateOpacity(value); }

// Fiber filtering
void MainWindow::findInteresectedTracts(QList<quint32> &intersectedFibers)
{
    // we will only save the index of the intersed tracts
    if (ROIorder.isEmpty())
        return;
    // loop on fibers to get intersected fibers
    // assure currentSelectedFibers contains fibers before length filtering
    if (!selectedFibersList.isEmpty())
        currentSelectedFibers = selectedFibersList.last();
    else {
        currentSelectedFibers.clear();
        currentSelectedFibers.reserve(fibers.size());
        for (int i = 0; i< fibers.size();++i)
            currentSelectedFibers.append(i);
    }
    // translate the axis according to direction
    // adjust the ROI
    QMatrix trans;
    QRectF roiR;
    QGraphicsEllipseItem roiE;
    QPolygonF roiP;
    uchar lastROIType = ROIorder.last();
    if (!AxialVectorButton->isChecked())   // mirror only
        trans = QMatrix(1,0,0,-1,0,mainColoredImage.height());
    else                                    // transpose and mirror
        trans = QMatrix(0,1,1,0,0,0)*QMatrix(-1,0,0,1,mainColoredImage.height(),0);

    //check for points in the last drawn ROI
    if (!rectROIs.isEmpty() && lastROIType == RECT)
        roiR = trans.mapRect(rectROIs.last()).normalized();
    else if (!ovalROIs.isEmpty() && lastROIType == OVAL)
        roiE.setRect(trans.mapRect(ovalROIs.last()));
    else if (!freeHandROIs.isEmpty() && lastROIType == FREEHAND)
        roiP = trans.map(freeHandROIs.last());
    // create the ROI plane
    QVector3D planeP, planeN, I;
    if (CoronalVectorButton->isChecked()) {
        planeN = QVector3D(1,0,0);
        planeP = QVector3D(selModelSlice,0,0);
    }
    else if (SagitalVectorButton->isChecked()) {
        planeN = QVector3D(0,1,0);
        planeP = QVector3D(0,selModelSlice,0);
    }
    else if (AxialVectorButton->isChecked()) {
        planeN = QVector3D(0,0,1);
        planeP = QVector3D(0,0,selModelSlice);
    }
    QPointF point;
    QVector3D p1,p2, u, w;
    quint64 idx;
    for (int f=0;f<currentSelectedFibers.size();f++)
    {
        idx = currentSelectedFibers[f];
        // loop through the fiber's segments and check intersected lines
        // get the intersection point and check if it is in the ROI
        for (int j = 0;j<fibers.getFiber(idx).size()-1;++j) {
            p1 = fibers.getFiber(idx).at(j);
            p2 = fibers.getFiber(idx).at(j+1);
            // test if segment cuts through the slice of interrest
            if ( (CoronalVectorButton->isChecked() &&
                  ( ( (p1.x() <= selModelSlice) && (selModelSlice <= p2.x()) ) ||
                    ( (p1.x() >= selModelSlice) && (selModelSlice >= p2.x()) )) ) ||
                 (SagitalVectorButton->isChecked() &&
                  ( ( (p1.y() <= selModelSlice) && (selModelSlice <= p2.y()) ) ||
                    ( (p1.y() >= selModelSlice) && (selModelSlice >= p2.y()) )) ) ||
                 (AxialVectorButton->isChecked() &&
                  ( ( (p1.z() <= selModelSlice) && (selModelSlice <= p2.z()) ) ||
                    ( (p1.z() >= selModelSlice) && (selModelSlice >= p2.z()) )) ) ) {

                u = p2 - p1;
                w = planeP - p1;

                // compute segment intersect poin
                I = p1 + (QVector3D::dotProduct(planeN,w)/QVector3D::dotProduct(planeN,u))*u;
                if (CoronalVectorButton->isChecked())
                    point = QPointF(I.y(),I.z());
                else if (SagitalVectorButton->isChecked())
                    point = QPointF(I.x(),I.z());
                else if (AxialVectorButton->isChecked())
                    point = QPointF(I.x(),I.y());
                //check for points in the last drawn ROI and save intersected fiber index
                if ((lastROIType == RECT && roiR.contains(point)) ||
                        ((lastROIType == OVAL) && roiE.contains(point)) ||
                        ((lastROIType == FREEHAND) && roiP.containsPoint(point,Qt::OddEvenFill) ))
                {
                    intersectedFibers.append(f);
                    break;
                }
            }
        }
    }
}
void MainWindow::filterFibersByROI()
{
    if (fibers.isEmpty() || ROIorder.isEmpty())
        return;

//    QTime timer;
//    timer.start();

    // get intersected fibers with ROI
    QList<quint32> intersectedFibers;
    // note that this function returns the indeces of the intersected fibers
    // from the currentSelectedFibers. In other words, the elements of
    // intersectedFibers are not the indeces of fibers, they are in fact the
    // indeces of the currentSelectedFibers, this is done for the NOT case
    findInteresectedTracts(intersectedFibers);

//    qDebug()<<timer.elapsed();

    if (AND->isChecked()) {             // keep intersected fibers
        if (!intersectedFibers.isEmpty()) {
            QList<quint32> tmp;
            tmp.reserve(intersectedFibers.size());
            for (int i = 0 ; i < intersectedFibers.size(); ++i)
                tmp.append(currentSelectedFibers[intersectedFibers[i]]);
            currentSelectedFibers = tmp;
        }
        else                            // all fibers are gone
            currentSelectedFibers.clear();
    }
    else if (NOT->isChecked()) {        // remove intersected fibers
        if (!intersectedFibers.isEmpty())
            for (int i = intersectedFibers.size()-1; i>-1; --i)
                currentSelectedFibers.removeAt(intersectedFibers[i]);
    }
    selectedFibersList.append(currentSelectedFibers);
    UndoROI_Button->setEnabled(true);
    filteringActions.append(ROI_ACTION);

    // add item to the ROI-VOI List
    QTreeWidgetItem *item = new QTreeWidgetItem();
    QString txt = QString::number(selModelSlice);
    if (CoronalVectorButton->isChecked())
        txt = txt + "/Coronal";
    if (SagitalVectorButton->isChecked())
        txt = txt + "/Sagittal";
    if (AxialVectorButton->isChecked())
        txt = txt + "/Axial";
    item->setText(0,txt);
    item->setToolTip(0,txt);
    if (AND->isChecked())
        item->setText(1,"AND");
    else
        item->setText(1,"NOT");
    if (ROIorder.last() == RECT)
        item->setText(2,"RECTANGLE");
    else if (ROIorder.last() == OVAL)
        item->setText(2,"ELLIPSE");
    else if (ROIorder.last() == FREEHAND)
        item->setText(2,"FREEHAND");
    item->setToolTip(2,item->text(2));
    item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    ROI_VOI_List->addTopLevelItem(item);

    visFibers();

}
// control length slider
void MainWindow::lengthSliderMoved()
{
    LeftSliderLabel->setText(QString::number(FiberLengthSlider->lowerValue()));
    RightSliderLabel->setText(QString::number(FiberLengthSlider->upperValue()));
}
void MainWindow::lengthSliderChanged()
{
    if (DisplaySelected->isChecked() && !selectedFibersList.isEmpty())
        currentSelectedFibers = selectedFibersList.last();
    else {
        currentSelectedFibers.clear();
        for (int i = 0;i<fibers.size();i++)
            currentSelectedFibers.append(i);
    }
    visFibers();
}

void MainWindow::fiberStats()
{
    if (!fibers.isEmpty()) {
        if (selectedFibersList.isEmpty() || DisplayAll->isChecked())
            (StatsDialog(fibers,NULL,activeVol,activeImgIsLabel,this)).exec();
        else
            (StatsDialog(fibers,&currentSelectedFibers,activeVol,activeImgIsLabel,this)).exec();
    }
}

void MainWindow::renderButPressed()
{
    if (! (activeImgIsLabel && LUT_Box->currentIndex() > 0) )
        return;
    // get unique elements from the active volume
    // the unique elements are the labels vector, they are used
    // to act as the interface with the tree table
    vec labels = unique(vec(activeVol.memptr(),activeVol.n_elem));
    // check if any lables are negative, display a warning
    if (labels[0] < 0) {
        QMessageBox::critical(this, "CoNECt",
                              QString("Labels should contain non-negative integer values, "
                                      "can't render the current label image."));
        return;
    }
    // check if the largest label is greater than the color table
    if (labels[labels.n_elem-1] >= cTable.size()) {
        QMessageBox::critical(this, "CoNECt",
                              QString("The number of labels is greater than the Color Table, "
                                      "can't render the current label image."));
        return;

    }
    // remove the zero if first element
    if (labels[0] == 0) labels.shed_row(0);
   MyProgressDialog *prog = new MyProgressDialog(this);
   prog->disableCancel();
   prog->disableMain();
    if (!VTKWidget->renderLabelVolume(labels,cTable,prog))
        return;
    delete prog;
    // update label properties
    updateOpacitySpinner = true;
    labelVisbilityStatus = ones(labels.n_elem);
    labelOpacityValues = 100*ones(labels.n_elem);
    labelColorList.clear();
    // fill the tree list
    int idx = LUT_Box->currentIndex();
    LabelsControlBox->setEnabled(true);
    QPixmap backGrCol = QPixmap(12,12);

    QTreeWidgetItem *root = new QTreeWidgetItem(LabelsList);
    root->setText(0,QString("Models for: ") + ImageComboBox->currentText());
    root->setToolTip(0,ImageComboBox->currentText());
    root->setExpanded(true);
    root->setFirstColumnSpanned(true);

    for (uint i = 0;i <labels.n_elem;i++) {
        QColor c = cTable.at(labels(i));
        labelColorList.append(c);
        // fill table with the lables getNames()
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setIcon(0,QIcon(":/images/visible.png"));
        item->setToolTip(0,"Visibility");
        backGrCol.fill(c);
        item->setIcon(1,QIcon(backGrCol));
        item->setToolTip(1,"Color");
        item->setData(2,Qt::EditRole,QVariant(100));
        item->setToolTip(2,"Opacity");
        item->setText(3,cTablesLabels[idx].at(labels(i)));
        item->setToolTip(3,QString("%1: %2").
                         arg(cTablesLabels[idx].at(labels(i)))
                         .arg(labels(i)));
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        root->addChild(item);
    }
    LabelsList->insertTopLevelItem(0,root);
    LabelsList->setColumnWidth(0,48); // visibility
    LabelsList->setColumnWidth(1,16); // color
    LabelsList->setColumnWidth(2,42); // opacity
    LabelsList->setColumnWidth(3,200);
    // apply a specific editor to the third column:
    // a QpinBox with limits of 0 to 100
    LabelsList->setItemDelegateForColumn(2,new SpinBoxDelegate(LabelOpacitySpinner, LabelsList));
    // allow only clearing models
    RenderButton->setEnabled(false);
    ClearLabelsButton->setEnabled(true);
}
// update controls according to last selected item
void MainWindow::labelItemPressed()
{
    if (VTKWidget->isLabelVolumeEmpty())
        return;
    // prevent selecting the root
    QList<QTreeWidgetItem*> selectedItems = LabelsList->selectedItems();
    if (selectedItems.isEmpty())
        return;
    if (selectedItems.last()->isFirstColumnSpanned()) {
        selectedItems.last()->setSelected(false);
        return;
    }
    // get last selected row
    int idx = LabelsList->selectionModel()->selectedRows(0).last().row();
    // update color, visibility and opacity controls
    LabelColorButton->setPalette( QPalette(labelColorList[idx]) );
    VisibilityStatusCheck->setChecked(labelVisbilityStatus(idx));
    // show the opacity value and prevent any update
    updateOpacitySpinner = false;
    LabelOpacitySpinner->setValue(labelOpacityValues(idx));
    updateOpacitySpinner = true;
}
// controlling labels visibility, color and opacity
void MainWindow::changeLabelVisbility(bool status)
{
    if (VTKWidget->isLabelVolumeEmpty())
        return;
    QModelIndexList indexes = LabelsList->selectionModel()->selectedRows(0);
    QList<QTreeWidgetItem*> selectedItems = LabelsList->selectedItems();
    u32_vec idx = Col<u32>(selectedItems.size());
    for (int i =0; i<selectedItems.size();i++) {
        idx[i] = indexes.at(i).row();
        labelVisbilityStatus(idx[i]) = status;
        if (status)
            selectedItems[i]->setIcon(0,QIcon(":/images/visible.png"));
        else
            selectedItems[i]->setIcon(0,QIcon(":/images/invisible.png"));
    }
    VTKWidget->changeLabelVisbility(idx,status);
}
void MainWindow::changeLabelColor()
{
    if (VTKWidget->isLabelVolumeEmpty())
        return;
    QColor c = QColorDialog::getColor(Qt::red, this, "Select Color");
    if (!c.isValid())
        return;
    QPixmap backGrCol = QPixmap(12,12);
    backGrCol.fill(c);
    QModelIndexList indexes = LabelsList->selectionModel()->selectedRows(0);
    QList<QTreeWidgetItem*> selectedItems = LabelsList->selectedItems();
    u32_vec idx = Col<u32>(selectedItems.size());
    for (int i =0; i<selectedItems.size();i++) {
        idx[i] = indexes.at(i).row();
        labelColorList[idx[i]] = c;
        selectedItems[i]->setIcon(1,QIcon(backGrCol));
    }    
    VTKWidget->changeLabelColor(idx,c);
    LabelColorButton->setPalette( QPalette(c) );
}
void MainWindow::changeLabelOpacity(int value)
{
    if (!updateOpacitySpinner || VTKWidget->isLabelVolumeEmpty())
        return;
    QModelIndexList indexes = LabelsList->selectionModel()->selectedRows(0);
    QList<QTreeWidgetItem*> selectedItems = LabelsList->selectedItems();
    u32_vec idx = Col<u32>(selectedItems.size());
    for (int i =0; i < selectedItems.size();i++) {
        idx[i] = indexes.at(i).row();
        labelOpacityValues(idx[i]) = value;
        selectedItems[i]->setData(2,Qt::EditRole,QVariant(value));
    }
    VTKWidget->updateLabelOpacity(idx,value);
}
// item control
void MainWindow::labelItemDoubleClicked(QTreeWidgetItem *item, int col)
{
    // when an item is double clicked, perform convenient action
    if (item->isFirstColumnSpanned())
        return;
    if (col == 0) {         // visibility
        int idx = LabelsList->selectionModel()->selectedRows(0).first().row();
        bool status = !labelVisbilityStatus(idx);
        labelVisbilityStatus(idx) = status;
        if (status)
            item->setIcon(0,QIcon(":/images/visible.png"));
        else
            item->setIcon(0,QIcon(":/images/invisible.png"));
        u32_vec tmp; tmp<<idx;
        VTKWidget->changeLabelVisbility(tmp,status);
        VisibilityStatusCheck->setChecked(status);
    }
    else if (col == 1)      // color change
        changeLabelColor();
    // opacity is updated through the spinner
}


void MainWindow::clearLabelsButPress()
{
    VTKWidget->clearLabelVolumes();
    // clear variables
    labelOpacityValues.clear();
    labelVisbilityStatus.clear();
    labelColorList.clear();
    LabelsList->clear();
    // reset controllers and GUI
    RenderButton->setEnabled(true);
    ClearLabelsButton->setEnabled(false);
    QColor bg = this->palette().window().color().rgb();
    LabelColorButton->setPalette( QPalette(bg) );
    LabelOpacitySpinner->setValue(100);
    LabelsControlBox->setEnabled(false);
}

bool MainWindow::isActiveImageLabelMap()
{
    return (accu(abs(activeVol - round(activeVol))) == 0);
}

void MainWindow::fill_VOI_List()
{
    Label_VOI_List->clear();
    // check the existance of fibers
    if (fibers.isEmpty() || !activeImgIsLabel)
        return;
    if (!isActiveImageLabelMap()) {
        QMessageBox::critical(this,"Error","It appears that the chosen image "
                              "is not a label map as suggested. Can not use it as VOI.");
        return;
    }
    // get unique label values
    vec labels = sort(unique(vec(activeVol.memptr(),activeVol.n_elem)),0);
    if (labels[0] == 0)
        labels.shed_row(0);
    // fill the list
    Label_VOI_List->setUpdatesEnabled(false);
    int idx = LUT_Box->currentIndex();
    for (int i = 0; i<labels.n_elem; ++i) {
        QString txt = QString::number(labels[i]);
        if (idx > 0 && cTablesLabels[idx].size() >= labels.n_elem)
                txt = txt + " - " + cTablesLabels[idx].at(labels(i));
        Label_VOI_List->addItem(txt);
    }
    Label_VOI_List->setUpdatesEnabled(true);
}

void MainWindow::onAdd_VOI_LabelButtonPress()
{
    int idx = Label_VOI_List->currentRow();
    if (idx < 0 || fibers.isEmpty())
        return;

    // get label value from the entry
    int label = Label_VOI_List->currentItem()->text().
            split(QRegExp("(\\s+)"), QString::SkipEmptyParts).first().toInt();

    // get intersected fibers with ROI
    QList<quint32> intersectedFibers;
    findInteresectedTractsWithVOI(intersectedFibers, label);

    if (AND->isChecked()) {             // keep intersected fibers
        if (!intersectedFibers.isEmpty()) {
            QList<quint32> tmp;
            for (int i = 0 ; i < intersectedFibers.size(); ++i)
                tmp.append(currentSelectedFibers[intersectedFibers[i]]);
            currentSelectedFibers = tmp;
        }
        else                            // all fibers are gone
            currentSelectedFibers.clear();
    }
    else if (NOT->isChecked()) {        // remove intersected fibers
        if (!intersectedFibers.isEmpty())
            for (int i = intersectedFibers.size()-1; i>-1; --i)
                currentSelectedFibers.removeAt(intersectedFibers[i]);
    }
    selectedFibersList.append(currentSelectedFibers);
    UndoROI_Button->setEnabled(true);
    filteringActions.append(VOI_ACTION);

    // add item to the ROI-VOI List
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0,ImageComboBox->currentText());
    item->setToolTip(0,ImageComboBox->currentText());
    if (AND->isChecked())
        item->setText(1,"AND");
    else
        item->setText(1,"NOT");
    item->setText(2,Label_VOI_List->currentItem()->text());
    item->setToolTip(2,Label_VOI_List->currentItem()->text());
    item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    ROI_VOI_List->addTopLevelItem(item);

    visFibers();
}

void MainWindow::findInteresectedTractsWithVOI(QList<quint32> &intersectedFibers, int label)
{
    // loop on fibers to get intersected fibers
    // assure currentSelectedFibers contains fibers before length filtering
    if (!selectedFibersList.isEmpty())
        currentSelectedFibers = selectedFibersList.last();
    else {
        currentSelectedFibers.clear();
        for (int i = 0; i< fibers.size();i++)
            currentSelectedFibers.append(i);
    }

    quint64 idx;
    int x = 0, y = 0, z = 0;
    for (int f=0;f<currentSelectedFibers.size();++f)
    {
        idx = currentSelectedFibers[f];
        // check if fiber intersects with VOI
        for (int j = 0;j<fibers.getFiber(idx).size();++j) {
            x = qMax(qMin(qRound(fibers.getFiber(idx).at(j).x()),int(nRowsImg-1)),0);
            y = qMax(qMin(qRound(fibers.getFiber(idx).at(j).y()),int(nColsImg-1)),0);
            z = qMax(qMin(qRound(fibers.getFiber(idx).at(j).z()),int(nSlicesImg-1)),0);
            if (activeVol(x,y,z) == label) {
                intersectedFibers.append(f);
                break;
            }
        }
    }
}
// connectome testing
void MainWindow::openConnectome()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Connectome File", ".",
                                                    QString("Connectome file (*.cnct)"));
    if (fileName.isEmpty())
        return;
    MyProgressDialog *progress = new MyProgressDialog(this);
    progress->disableMain();
    progress->disableCancel();
    progress->setRange(0,3);
    progress->setModal(true);
    progress->setLabelText("Computing metrics ... ");
    progress->setFixedSize(progress->sizeHint());
    progress->setValue(0);
    progress->show();
    if (!cm.read(fileName)) {
        delete progress;
        return;
    }
    progress->setValue(1);
    progress->setLabelText("Preparing visualization ... ");
    // focus tabs to connectome
    DisplayTab->setCurrentIndex(1);
    ControlTab->setCurrentIndex(3);
    // visualize
    CnctView->plotConnectome(cm.getNW());
    progress->setValue(2);
    CVTKWidget->visualizeConnectome(cm,getNodalVP(),getEdgeVP());
    progress->setValue(3);
    // enable vis and control areas
    CVTKWidget->setEnabled(true);
    CnctView->setEnabled(true);
    enableConnectomeControls(true);
    delete progress;
}

VisualizationParameters MainWindow::getNodalVP()
{
    VisualizationParameters vp;
    // display
    vp.dType = (NoLabelCheck->isChecked())?NONE:(AllLabelCheck->isChecked())?ALL:THRESHOLDS;
    vp.dThreshold = LabelThresholdSpinner->value();
    vp.dThresholdIdx = LabelThresholdBox->currentIndex();
    // size
    vp.sType = (EqualNodeSizeCheck->isChecked())?EQUAL:WEIGHT;
    vp.size = EqualNodeSizeSpinner->value();
    vp.sWeightIdx = WeightedNodeSizeBox->currentIndex();
    vp.sScale = ScaleNodeSizeSpinner->value();
    // color
    vp.cType = (SameNodeColorCheck->isChecked())?
                SAME:(ModularNodeColorCheck->isChecked())?MODULAR:THRESHOLD;
    if (vp.cType == SAME)
        vp.cList.append(SameNodeColorButton->
                        palette().color(QPalette::Active,QPalette::Button));
    else if (vp.cType == MODULAR) {
        // randomly generate a color list if the nodeColorList is not
        // equal to the number of modules
        uint n = max(cm.getLocalMetrics().row(9))+1;
        if (nodeColorList.size() != n) {
            nodeColorList.clear();
            nodeColorList.reserve(n);
            arma_rng::set_seed_random();
            for (uint i=0;i<n;++i) {
                urowvec c = randi<urowvec>(3,distr_param(0,255));
                nodeColorList.append(QColor(c(0),c(1),c(2)));
            }
        }
        vp.cList = nodeColorList;
    }
    else {
        vp.cList.append(ThresholdNodeColorBeforeButton->
                        palette().color(QPalette::Active,QPalette::Button));
        vp.cList.append(ThresholdNodeColorAfterButton->
                        palette().color(QPalette::Active,QPalette::Button));
    }
    vp.cModularIdx = ModularNodeColorBox->currentIndex();
    vp.cThresholdIdx = ThresholdNodeColorBox->currentIndex();
    vp.cThreshold = ThresholdNodeColorBeforeSpinner->value();
    return vp;
}

VisualizationParameters MainWindow::getEdgeVP()
{
    VisualizationParameters vp;
    // display
    vp.dType = (NoEdgeCheck->isChecked())?NONE:(AllEdgeCheck->isChecked())?ALL:THRESHOLDS;
    vp.dThreshold = ThresholdEdgeSpinner->value();
    vp.dThresholdIdx = ThresholdEdgeBox->currentIndex();
    // size
    vp.sType = (EqualEdgeSizeCheck->isChecked())?EQUAL:WEIGHT;
    vp.size = EqualEdgeSizeSpinner->value();
    vp.sWeightIdx = WeightedEdgeSizeBox->currentIndex();
    vp.sScale = ScaleEdgeSizeSpinner->value();
    // color
    vp.cType = (SameEdgeColorCheck->isChecked())?
                SAME:(ModularEdgeColorCheck->isChecked())?MODULAR:THRESHOLD;
    if (vp.cType == SAME)
        vp.cList.append(SameEdgeColorButton->
                        palette().color(QPalette::Active,QPalette::Button));
    else if (vp.cType == MODULAR) {
        uint n = max(cm.getLocalMetrics().row(9))+1;
        if (edgeColorList.size() != n) {
            edgeColorList.clear();
            edgeColorList.reserve(n);
            arma_rng::set_seed_random();
            for (uint i=0;i<n;++i) {
                urowvec c = randi<urowvec>(3,distr_param(0,255));
                edgeColorList.append(QColor(c(0),c(1),c(2)));
            }
        }
        vp.cList = edgeColorList;
    }
    else {
        vp.cList.append(ThresholdEdgeColorBeforeButton->
                        palette().color(QPalette::Active,QPalette::Button));
        vp.cList.append(ThresholdEdgeColorAfterButton->
                        palette().color(QPalette::Active,QPalette::Button));
    }
    vp.cModularIdx = ModularEdgeColorBox->currentIndex();
    vp.cThresholdIdx = ThresholdEdgeColorBox->currentIndex();
    vp.cThreshold = ThresholdEdgeColorBeforeSpinner->value();
    return vp;
}

void MainWindow::handleMouseMoveConnectome(QMouseEvent *ev)
{
    QPointF pt = CnctView->maptToViewScene(ev->pos());
    if (!CnctView->viewContains(pt)) {
        LocationLabel->clear();
        return;
    }
    int x = qFloor(pt.x()), y = qFloor(pt.y());
    LocationLabel->setText( QString("(%1-%2) %3 <-> %4 : %5").arg(x+1).arg(y+1).
                            arg(cm.getNames().at(x)).arg(cm.getNames().at(y)).
                            arg(cm.getNW().at(x,y)) );
}

void MainWindow::handleMousePressConnectome(QMouseEvent *ev)
{
    if (ev->button() == Qt::MidButton) {
        QPointF pt = CnctView->maptToViewScene(ev->pos());
        if (CnctView->viewContains(pt)) {
            int x = qFloor(pt.x()), y = qFloor(pt.y());
            CnctView->addSelection(x,y);
            CVTKWidget->select(x,y);
            ConnectomeLabel->setText( QString("(%1-%2) %3 <-> %4 : %5").arg(x+1).arg(y+1).
                                      arg(cm.getNames().at(x)).arg(cm.getNames().at(y)).
                                      arg(cm.getNW().at(x,y)) );
        }
        else {
            CnctView->removeSelection();
            CVTKWidget->deselect();
            ConnectomeLabel->clear();
        }
    }
}
// update nodes display
void MainWindow::onNodeLabelChange()
{
    CVTKWidget->updateNodesLabels(cm,getNodalVP());
}
void MainWindow::onNodeLabelThresholdMetricChange()
{
    uint idx = LabelThresholdBox->currentIndex();
    double  low = min(cm.getLocalMetrics().row(idx)),
            high = max(cm.getLocalMetrics().row(idx))+1;
    LabelThresholdSpinner->setRange(low,high);
    LabelThresholdSpinner->setSingleStep((high-low)/25);
    onNodeLabelChange();
}

void MainWindow::onNodeSizeChange()
{
    CVTKWidget->visualizeNodes(cm,getNodalVP());
}
void MainWindow::nodeAutoSizeScale()
{
    // figure out an appropriate scale based on a max radius of 2.5
    double scale;
    if (EqualNodeSizeCheck->isChecked())
        scale = 2.5/EqualNodeSizeSpinner->value();
    else
        scale = 2.5/max(cm.getLocalMetrics().row(WeightedNodeSizeBox->currentIndex()));
    ScaleNodeSizeSpinner->setValue(scale);
    ScaleNodeSizeSpinner->setSingleStep(scale/25);
    onNodeSizeChange();
}

void MainWindow::onNodeColorChange()
{
    CVTKWidget->updateNodesColor(cm,getNodalVP());
}
void MainWindow::onNodeSameColorButtonPress()
{
    QColor c = QColorDialog::getColor(SameNodeColorButton->palette().
                                      color(QPalette::Active,QPalette::Button),
                                      this, "Select Color");
    if (!c.isValid())
        return;
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,c);
    SameNodeColorButton->setPalette(pal);
    onNodeColorChange();
}
void MainWindow::onNodeModularColorButtonPress()
{
    ModularDialog *w = new ModularDialog(nodeColorList,this);
    if (w->exec()) {
        nodeColorList = w->cListOut;
        onNodeColorChange();
    }
    delete w;
}
void MainWindow::onNodeThresholdBeforeColorButtonPress()
{
    QColor c = QColorDialog::getColor(ThresholdNodeColorBeforeButton->palette().
                                      color(QPalette::Active,QPalette::Button),
                                      this, "Select Color");
    if (!c.isValid())
        return;
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,c);
    ThresholdNodeColorBeforeButton->setPalette(pal);
    onNodeColorChange();
}
void MainWindow::onNodeThresholdAfterColorButtonPress()
{
    QColor c = QColorDialog::getColor(ThresholdNodeColorAfterButton->palette().
                                      color(QPalette::Active,QPalette::Button),
                                      this, "Select Color");
    if (!c.isValid())
        return;
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,c);
    ThresholdNodeColorAfterButton->setPalette(pal);
    onNodeColorChange();
}
void MainWindow::onNodeColorThresholdMetricChange()
{
    uint idx = ThresholdNodeColorBox->currentIndex();
    double  low = min(cm.getLocalMetrics().row(idx)),
            high = max(cm.getLocalMetrics().row(idx))+1;
    ThresholdNodeColorBeforeSpinner->setRange(low,high);
    ThresholdNodeColorBeforeSpinner->setSingleStep((high-low)/50);
    onNodeColorChange();
}

// update edges display
void MainWindow::onEdgeDisplayChange()
{
    CVTKWidget->visualizeEdges(cm,getEdgeVP());
}
void MainWindow::onEdgeDisplayThresholdMetricChange()
{
    // this function is called only when threshold is chosen
    uint idx = ThresholdEdgeBox->currentIndex();
    double  low =( idx == 0)?cm.getNW().min():cm.getEdgeBetweeness().min(),
            high =(idx == 0)?cm.getNW().max()+1:cm.getEdgeBetweeness().max()+1;
    ThresholdEdgeSpinner->setRange(low,high);
    ThresholdEdgeSpinner->setSingleStep((high-low)/25);
    onEdgeDisplayChange();
}

void MainWindow::onEdgeSizeChange()
{
    CVTKWidget->visualizeEdges(cm,getEdgeVP());
}
void MainWindow::edgeAutoSizeScale()
{
    // figure out an appropriate scale based on a max radius of 1.5
    double scale;
    if (EqualEdgeSizeCheck->isChecked())
        scale = 0.5/EqualEdgeSizeSpinner->value();
    else {
        if (WeightedEdgeSizeBox->currentIndex() == 0)
            scale = 1.5/cm.getNW().max();
        else
            scale = 1.5/cm.getEdgeBetweeness().max();
    }
    ScaleEdgeSizeSpinner->setValue(scale);
    ScaleEdgeSizeSpinner->setSingleStep(scale/25);
    onEdgeSizeChange();
}

void MainWindow::onEdgeColorChange()
{
    CVTKWidget->updateEdgesColor(cm,getEdgeVP());
}
void MainWindow::onEdgeSameColorButtonPress()
{
    QColor c = QColorDialog::getColor(SameEdgeColorButton->palette().
                                      color(QPalette::Active,QPalette::Button),
                                      this, "Select Color");
    if (!c.isValid())
        return;
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,c);
    SameEdgeColorButton->setPalette(pal);
    onEdgeColorChange();
}
void MainWindow::onEdgeModularColorButtonPress()
{
    ModularDialog *w = new ModularDialog(edgeColorList,this);
    if (w->exec()) {
        edgeColorList = w->cListOut;
        onEdgeColorChange();
    }
    delete w;
}
void MainWindow::onEdgeThresholdBeforeColorButtonPress()
{
    QColor c = QColorDialog::getColor(ThresholdEdgeColorBeforeButton->palette().
                                      color(QPalette::Active,QPalette::Button),
                                      this, "Select Color");
    if (!c.isValid())
        return;
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,c);
    ThresholdEdgeColorBeforeButton->setPalette(pal);
    onEdgeColorChange();
}
void MainWindow::onEdgeThresholdAfterColorButtonPress()
{
    QColor c = QColorDialog::getColor(ThresholdEdgeColorAfterButton->palette().
                                      color(QPalette::Active,QPalette::Button),
                                      this, "Select Color");
    if (!c.isValid())
        return;
    QPalette pal;
    pal.setColor(QPalette::Active,QPalette::Button,c);
    ThresholdEdgeColorAfterButton->setPalette(pal);
    onEdgeColorChange();
}
void MainWindow::onEdgeColorThresholdMetricChange()
{
    uint idx = ThresholdEdgeColorBox->currentIndex();
    double  low =( idx == 0)?cm.getNW().min():cm.getEdgeBetweeness().min(),
            high =(idx == 0)?cm.getNW().max()+1:cm.getEdgeBetweeness().max()+1;
    ThresholdEdgeColorBeforeSpinner->setRange(low,high);
    ThresholdEdgeColorBeforeSpinner->setSingleStep((high-low)/25);
    onEdgeColorChange();
}

void MainWindow::displayMetrics() { ShowMetricsDialog(cm,this).exec(); }

// this is just a test code
void MainWindow::generateConnectome()
{
    if (fibers.isEmpty()) {
        QMessageBox::critical(0, "Warning!", "No fibers exist, please load "
                              "or generate full brain tractography.");
        return;
    }
    else if (!activeImgIsLabel) {
        QMessageBox::critical(0, "Warning!", "Current image is not a label, can not proceed.");
        return;
    }
    // construct a dialog to choose between Selected or All fibers
    ConnectomeGenerateDialog *w = new ConnectomeGenerateDialog(this);
    if (!w->exec()) { delete w; return; }
    QString fileName = w->getFileName();
    delete w;
    if (fileName.isEmpty())
        return;
    s16_cube lbl = conv_to<s16_cube>::from(activeVol);
    MyProgressDialog *prog = new MyProgressDialog(this);
    prog->disableCancel();
    prog->disableMain();
    bool error = false;
    if (cm.generate(lbl,fibers,true,prog)){
        if (!cm.readNames(fileName)) {
            QMessageBox::critical(0, "Warning!", "Number of labels in the generated network does"
                                  "not match with the label file. Can not proceed.");
            error = true;
        }
    }
    else {
        QMessageBox::critical(0, "Warning!", "Error generating network.");
        error = true;
    }
    delete prog;
    if (error) {
        cm.clear();
        return;
    }
    cm.computeMetrics();
    fileName = QFileDialog::getSaveFileName(this,"Save Connectome File", ".", "(*.cnct)");
    if (!fileName.isEmpty()) {
        // assure it is the right extension
        QFileInfo fi(fileName);
        if (fi.suffix() != "cnct")
            fileName.append(".cnct");
        cm.write(fileName);
    }
    // focus tabs to connectome
    DisplayTab->setCurrentIndex(1);
    ControlTab->setCurrentIndex(3);
    // visualize
    CnctView->plotConnectome(cm.getNW());
    CVTKWidget->visualizeConnectome(cm,getNodalVP(),getEdgeVP());
    // enable vis and control areas
    CVTKWidget->setEnabled(true);
    CnctView->setEnabled(true);
    enableConnectomeControls(true);
}

void MainWindow::onConnectomeMaskOpacityChange(int value)
{
    CVTKWidget->updateBrainMaskOpacity(value);
}

void MainWindow::onConnectomeMaskVisibilityChange(bool status)
{
    CVTKWidget->updateBrainMaskVisibility(status);
}

void MainWindow::onSaveMetricsButtonPress()
{
    if (cm.isEmpty() || !cm.haveMetrics())
        return;
    QString fileName = QFileDialog::getSaveFileName(this,"Save Metrics ... ", ".", "(*.txt)");
    if (fileName.isEmpty()) //check for cancel press
        return;
    // assure it is the right extension
    QFileInfo fi(fileName);
    if (fi.suffix().isEmpty())
        fileName += ".txt";
    cm.writeMetrics(fileName);
}
