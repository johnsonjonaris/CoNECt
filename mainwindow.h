#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow2.h"

using namespace arma;   // don't forget it VIP
using namespace std;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
    QxtSpanSlider               *FiberLengthSlider;
    QLabel                      *LeftSliderLabel, *RightSliderLabel;
    QHBoxLayout                 *SliderHBoxLayout;
    View                        *VectorsView, *CoronalView, *SagittalView, *AxialView;
    QToolBar                    *MainToolBar;
    QStatusBar                  *StatusBars;
    QLabel                      *LocationLabel, *FileInfo, *ActVolInfo;
    TableTree                   *LabelsList;
    MRIvtkWidget                *VTKWidget;
    ConnectomeView              *CnctView;
    CvtkWidget                  *CVTKWidget;
    // active image variables
    bool                        activeImgIsLabel, allowSychronize,
                                allowCorUpdate, allowSagUpdate, allowAxUpdate,
                                allowMainUpdate, resetSize, allowLUTupdate;
    int                         currentImage;
    uint                        selCorSlice, selSagSlice, selAxSlice, selModelSlice;
    QList < QVector<QRgb> >     cTables;
    QList < QStringList >       cTablesLabels;
    QVector<QRgb>               cTable;
    quint32                     nRowsImg,nColsImg,nSlicesImg, nVoxImg;
    float                       dxImg,dyImg,dzImg, dxByDyImg,dzByDxImg,dzByDyImg;
    double                      minActVol, maxActVol;

    // loaded Images
    struct {
        QList<uchar_cube>       ucharImg;   // unsigned character (1 byte)
        QList<s16_cube>         shortImg;   // signed short int (2 bytes)
        QList<s32_cube>         intImg;     // signed integer (4 bytes)
        QList<fcube>            floatImg;   // float (4 bytes)
        QList<cube>             doubleImg;  // double (8 bytes)
    }loadedImgs;
    cube                        activeVol;
    uint                        nLoadedUchar, nLoadedShort, nLoadedInt,
                                nLoadedFloat, nLoadedDouble, nLoadedImg;
    mat                         loadedImgsInfo;
    //label images
    bool                        updateOpacitySpinner;
    vec                         labelVisbilityStatus, labelOpacityValues;
    QList<QColor>               labelColorList;

    // diffusion model data
    DiffusionModel              dm;
    // vector visualization and colormap
    uchar_cube                  Rx,Ry,Rz;
    QImage                      mainColoredImage;

    // ROI stuff
    bool                        freehandROI_IsShown, ovalROI_IsShown, rectROI_IsShown;
    QList<QGraphicsItem *>      ROIs;                   // active drawn items
    QPolygonF                   freehandPath;           // temporary polygon being drawn
    QRectF                      rectOvalROI;            // temporary rectangle or oval being drawn
    QList<QPolygonF>            freeHandROIs;           // list of freehand ROI polygons
    QList<QRectF>               ovalROIs, rectROIs;     // list of rectangle and oval ROIs
    QList<ROIShape>             ROIorder;               // the order by which ROIs are drawn
    QList< QList<quint32> >     selectedFibersList;     // contains a list of the selected fibers indexes
    QList<quint32>              currentSelectedFibers;  // current fibers to be displayed
    QList<uchar>                filteringActions;       // contains a list of filtering actions:
                                                        // ROI Selection, Length or Color Filtering
    umat                        ROI_Info;               // contains information about ROIs
                                                        // shape, type, slice, orientation

    // fibers
    QColor                      fiberColor;
    FiberTracts                 fibers;
    int                         fiberColorOrientation[3]; // rgb

    // connectome
    Connectome                  cm;
    QVector<QColor>             nodeColorList, edgeColorList;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() { qDeleteAll(this->children());}

public slots:
    void resetViews();

private slots:
    //
    void changeCutDirection ();
    // updating tabs
    void tab1ChangeAction (int selected); // change tabs
    void tab2ChangeAction (int selected); // change tabs
    // updating views
    void showCoronalOnly(); void showSagittalOnly();    void showAxialOnly();
    void showVectorOnly();
    void show3DOnly();
    // file control
    void openModel();
    void closeAll();
    void openImage();       void closeImage();          void saveImage();
    void openFiber();                                   void saveFiber();
    void onWizardButtonPress();
    //zooming control
    void zoomInPressed(bool status);    void zoomOutPressed(bool status);
    // updating views from spinners
    void updateCoronalSlice();
    void updateSagittalSlice();
    void updateAxialSlice();
    void updateMainSlice(int slice);
    void resizeWhenSizeChanged();
    // handle mouse, key press and mouse press Events
    // for Coronal, Axial, Sagittal widgets
    void handleMouseMoveCoronal(QMouseEvent *);
    void handleMousePressCoronal(QMouseEvent *);
    void handleKeyPressCoronal(QKeyEvent *);
    void handleMouseMoveSagittal(QMouseEvent *);
    void handleMousePressSagittal(QMouseEvent *);
    void handleMouseMoveAxial(QMouseEvent *);
    void handleMousePressAxial(QMouseEvent *);
    // updating vector table
    void updateInvertX();   void updateInvertY();       void updateInvertZ();
    void updateSwapXY();    void updateSwapXZ();        void updateSwapYZ();
    void updateNormal(bool);
    void showTable();
    // handle mouse, key press and mouse press Events
    // for Main widgets
    void handleMouseMoveVector(QMouseEvent *);
    void handleMousePressVector(QMouseEvent *);
    void handleMouseReleaseVector(QMouseEvent *);
    void handlePaintEventVector(QPaintEvent *);
    // ROI and VOI
    void freehandROIPressed(bool status);
    void ovalROIPressed(bool status);
    void rectROIPressed(bool status);
    void clearROIsPressed() { clearROIs(); }
    void undoLastROI();
    void onAdd_VOI_LabelButtonPress();
    // tractpgraphy controls
    void generateFibers();
    void changeFiberDisplayState();
    void fiberColorPalettePressed();
    void filterFibersByROI();
    void clearFibers();
    // fiber visualization
    void colorMethodChanged(int newMethod);
    void fiberColorOrientChanged(int newOrient);
    void skipFiberSliderMoving(int val)
    { SkipPercLabel->setText(QString::number(val).append("%")); }
    void skipFiberSliderChanged() { visFibers(); }
    // fiber management
    void fiberStats();
    void lengthSliderMoved();
    void lengthSliderChanged();
    // 3D visualization control
    void visFibers();
    void changeImage(int);
    void onCoronalSpinnerChange(int);
    void onSagittalSpinnerChange(int);
    void onAxialSpinnerChange(int);
    void enable3DCoronal(bool choice);
    void enable3DAxial(bool choice);
    void enable3DSagittal(bool choice);
    void updateOpacity(int value);
    // segmented labels visualization
    void renderButPressed();
    void clearLabelsButPress();
    void labelItemPressed();
    void labelItemDoubleClicked(QTreeWidgetItem*,int);
    void changeLabelVisbility(bool status);
    void changeLabelColor();
    void changeLabelOpacity(int value);
    // LUT
    void openLUT();    void changeLUT(int);    void deleteLUT();
    //
    void about() { AboutFrame a(this); a.exec();}
    // Connectome stuff
    void openConnectome();
    void handleMouseMoveConnectome(QMouseEvent *);
    void handleMousePressConnectome(QMouseEvent *);
    // changes in visualization
    void onNodeLabelChange();
    void onNodeLabelThresholdMetricChange();

    void onNodeSizeChange();
    void nodeAutoSizeScale();

    void onNodeColorChange();
    void onNodeSameColorButtonPress();
    void onNodeModularColorButtonPress();
    void onNodeThresholdBeforeColorButtonPress();
    void onNodeThresholdAfterColorButtonPress();
    void onNodeColorThresholdMetricChange();

    void onEdgeDisplayChange();
    void onEdgeDisplayThresholdMetricChange();

    void onEdgeSizeChange();
    void edgeAutoSizeScale();

    void onEdgeColorChange();
    void onEdgeSameColorButtonPress();
    void onEdgeModularColorButtonPress();
    void onEdgeThresholdBeforeColorButtonPress();
    void onEdgeThresholdAfterColorButtonPress();
    void onEdgeColorThresholdMetricChange();

    void onConnectomeMaskOpacityChange(int value);
    void onConnectomeMaskVisibilityChange(bool status);

    void onSaveMetricsButtonPress();
    void displayMetrics();

    void generateConnectome();


protected:
    void resizeEvent(QResizeEvent* ev) {
        Q_UNUSED(ev); resizeWhenSizeChanged(); }
private:
    // GUI
    void initalize();
    void createCustomWidgets();
    void createStatusBar();
    void createToolBar();
    void connectSignals();
    void create3D();
    void initializeLUT();
    void resetImageControls();
    void resetVectorControls();
    void resetFiberControls();
    void resetConnectomeControls();
    void enableImageControls(bool);
    void enableVectorControls(bool);
    void enableFiberControls(bool);
    void enableLabelControls(bool);
    void enableConnectomeControls(bool);
    // wizard stuff
    bool processDTI_DWI_Subject(const QString &dwiFile, ImageFileType inType, const mat &gTable,
                                float bValue, int nLevel, bool getMetrics,TrackingParameters *tp,
                                ImageFileType outType, const QString &outFolder, QString &log,
                                MyProgressDialog *prog);
    bool DICOM2NII(const QString &inFolder,const QString &outFolder,
                   bool toNii, QString &out);
    //
    void storeDTIMetrics();
    // LUT
    bool readLUT(const QString &fileName);
    // update views
    void updateColorSlice();
    void updateVectors();
    void updateVectorsTOD();
    void updateVectorsDTI();
    void clearROIs();
    void synchronizeViews();
    void updateLabelsSize();
    void updateCoronalSize();
    void updateSagittalSize();
    void updateAxialSize();
    void updateVectorSize();
    // image 2D visualization
    void clearLoadedImages();
    void activeImageChanged();
    bool isActiveImageLabelMap();
    void fill_VOI_List();
    // 3D vis
    void draw3Ddata();
    // fiber filters
    void findInteresectedTracts(QList<quint32> &intersectedFibers);
    void findInteresectedTractsWithVOI(QList<quint32> &intersectedFibers, int label);
    // connectome
    VisualizationParameters getNodalVP();
    VisualizationParameters getEdgeVP();


};

#endif // MAINWINDOW_H
