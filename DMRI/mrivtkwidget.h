#ifndef MRIVTKWIDGET_H
#define MRIVTKWIDGET_H

#include <QtGui>
#include "datatypes.h"
#include "armadillo"
#include "UI\myprogressdialog.h"

#include <QVTKWidget.h>
// needed for the 3 cuts visualization
#include "vtkRenderWindow.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkSmartPointer.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkCamera.h>
// needed for the fiber tracts
#include <vtkCellArray.h>
#include <vtkProperty.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
// needed for the visualization
#include <vtkAlgorithm.h>
#include <vtkMarchingCubes.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkImageToStructuredPoints.h>
#include <vtkImageThreshold.h>
#include <vtkThreshold.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkDecimatePro.h>
#include <vtkStripper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataReader.h>

#include "DMRI\fibertracts.h"

using namespace arma;

/*
  This class is a derivative of the QVTKWidget, it establishes the GUI interface
  and saves the volume to be displayed as well as the different cuts. It also
  cares about the visualization of the fiber tracts and label volumes. It handles
  slice and opacity changes.
  */

class MRIvtkWidget : public QVTKWidget
{
public:
    MRIvtkWidget(QWidget *parent);

    // visualize volumes
    void setVolume(const cube &activeVolume,bool isGrayscaleVolume,
                   float dxImg,float dyImg,float dzImg,
                   double minActVol=0, double maxActVol=0);
    void drawCurrentVolume(const QVector<QRgb> &cTable,bool isColorVolume,
                           int selectedSlices[3],int opValue,bool enableCuts[3]);
    void changeCoronalSlice(int slice);
    void changeSagittalSlice(int slice);
    void changeAxialSlice(int slice);
    void changeSliceVisibility(ImageOrientation o,bool status);
    void updateOpacity(int value);
    void clear();
    inline bool isEmpty() {return empty;}
    // visualize fiber tracts
    void visualizeFibers(const FiberTracts &fibers,const QList<quint32> &selectedFibers,
                         int skip,int colorMethod,int colorOrientation[3]);
    void clearFibers();
    // visualize label maps
    bool renderLabelVolume(const vec &labels,const QVector<QRgb> &cTable,
                           MyProgressDialog *progress);
    void changeLabelVisbility(u32_vec idx,bool status);
    void changeLabelColor(u32_vec idx,QColor c);
    void updateLabelOpacity(u32_vec idx,int value);
    void clearLabelVolumes();
    inline bool isLabelVolumeEmpty() {return labelsActors.isEmpty();}

    //
    void updateScalarBarPosition() {scalarBar->SetDisplayPosition(this->width()-50,50);}



private:
    // volume stuff
    bool    empty;
    int     nRowsImg, nColsImg, nSlicesImg,
            selCorSlice, selSagSlice, selAxSlice;
    double  minVolumeValue,maxVolumeValue;

    //visualization stuff
    vtkSmartPointer<vtkRenderer>        mainRenderer;
    vtkSmartPointer<vtkImageData>       brain3D;
    vtkSmartPointer<vtkImageActor>      coronalActor, sagittalActor, axialActor;
    vtkSmartPointer<vtkActor>           linesActor;
    vtkSmartPointer<vtkScalarBarActor>  scalarBar;
    //label images
    QList <vtkSmartPointer<vtkActor> >  labelsActors;
};

#endif // MRIVTKWIDGET_H
