#ifndef CVTKWIDGET_H
#define CVTKWIDGET_H

#include <QVTKWidget.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkVectorText.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
//
#include <vtkImageData.h>
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

#include "DataTypes.h"
#include "armadillo"
#include "connectome.h"
#include <QDebug>

using namespace arma;

class CvtkWidget : public QVTKWidget
{
    vtkSmartPointer<vtkRenderer>            mainRenderer;
    QList< vtkSmartPointer<vtkActor> >      nodeActorList;
    QList< vtkSmartPointer<vtkActor> >      edgeActorList;
    QList< vtkSmartPointer<vtkActor> >      labelActorList;
    vtkSmartPointer<vtkActor>               brainActor;
    bool                                    selected;
    int                                     xs,ys, edgeIdx;
    uchar_mat                               displayedEdges;

    void visualizeBrain(const Connectome &cm);
public:
    CvtkWidget(QWidget *parent);
    void visualizeConnectome(const Connectome &cm,
                             const VisualizationParameters &nvp,
                             const VisualizationParameters &evp);
    // when visualizing from scratch or changing size
    void visualizeNodes(const Connectome &cm,
                        const VisualizationParameters &nvp);
    void visualizeEdges(const Connectome &cm,
                        const VisualizationParameters &evp);
    // when only color change is needed
    void updateNodesColor(const Connectome &cm,
                          const VisualizationParameters &nvp);
    void updateEdgesColor(const Connectome &cm,
                          const VisualizationParameters &evp);
    // update labels
    void updateNodesLabels(const Connectome &cm,
                           const VisualizationParameters &nvp);
    void select(int xx, int yy = -1);
    void deselect();
    // manipulate mask
    void updateBrainMaskOpacity(int value);
    void updateBrainMaskVisibility(bool status);


    // clear
    void clear();
};

#endif // CVTKWIDGET_H
