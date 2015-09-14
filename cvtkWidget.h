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

#include "datatypes.h"
#include "armadillo"
#include "connectome.h"
#include <QDebug>

using namespace arma;

/**
  * \class CvtkWidget
  *
  * VTK Widget class that visualizes connectomes. Visualization uses color codes to
  * colorize nodes and edges according to graph theoretic metrics. It also overlay brain
  * image on visualized network.
  */

class CvtkWidget : public QVTKWidget
{
    vtkSmartPointer<vtkRenderer>            mainRenderer;   ///< main renderer
    QList< vtkSmartPointer<vtkActor> >      nodeActorList;  ///< list of all node actors
    QList< vtkSmartPointer<vtkActor> >      edgeActorList;  ///< list of all edge actors
    QList< vtkSmartPointer<vtkActor> >      labelActorList; ///< list of all label actors
    vtkSmartPointer<vtkActor>               brainActor;     ///< brain actor
    bool                                    selected;       ///<
    int                                     xs, ys;         ///< selected edge coordinates
    int                                     edgeIdx;        ///< selected edge index in edgeActorList
    uchar_mat                               displayedEdges; ///< mark edges to be displayed

    /// visualize brain image
    void visualizeBrain(const Connectome &cm);
public:
    CvtkWidget(QWidget *parent);
    /// visualize a connectome including nodes and edges
    void visualizeConnectome(const Connectome &cm, const VisualizationParameters &nvp,
                             const VisualizationParameters &evp);
    /// visualize nodes
    void visualizeNodes(const Connectome &cm, const VisualizationParameters &nvp);
    /// visualize edges
    void visualizeEdges(const Connectome &cm, const VisualizationParameters &evp);
    /// update nodes colors
    void updateNodesColor(const Connectome &cm, const VisualizationParameters &nvp);
    /// update edge colors
    void updateEdgesColor(const Connectome &cm, const VisualizationParameters &evp);
    /// update nodes labels
    void updateNodesLabels(const Connectome &cm, const VisualizationParameters &nvp);
    /// highlight selected edge or node.
    void select(int xx, int yy = -1);
    /// remove selection
    void deselect();
    /// change brain image opacity
    void updateBrainMaskOpacity(int value);
    /// change brain image visibility
    void updateBrainMaskVisibility(bool status);
    /// clear widget from all its contents
    void clear();
};

#endif // CVTKWIDGET_H
