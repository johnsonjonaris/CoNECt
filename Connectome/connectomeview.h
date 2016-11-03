#ifndef CONNECTOMEVIEW_H
#define CONNECTOMEVIEW_H

#include <QWidget>
#include "armadillo"
#include "cview.h"
#include "Algorithms\misc_functions.h"
#include "ui_ConnectomeView.h"

#include <QDebug>

using namespace arma;

/**
  * \class ConnectomeView
  *
  * A collection of UI items that visualize a brain network and allows the user
  * to perform selection and discover actions onto the network.
  */

class ConnectomeView : public QWidget, private Ui::ConnectomeView
{
    Q_OBJECT
    QGraphicsRectItem *selection;           ///< rectangle selction item
    QGraphicsPixmapItem *nwPixmapItem;      ///< network is drawn as a pixmap
    uchar_mat nwImg;                        ///< raw data of the network

signals:
    void mouseMoved( QMouseEvent *ev );
    void mouseReleased( QMouseEvent *ev );

public:

    CView *View;                            ///< graphic view scene housing all items

    ConnectomeView(QWidget *parent = 0);
    // we don't have to delete the QGraphicItems since
    // nwPixmapItem is deleted when deleting the scene
    // and selection is either deleted when removing the
    // selection or when deleting the scene
    ~ConnectomeView() { qDeleteAll(this->children()); }
    /// plot a connectome
    void plotConnectome(const mat &NW);
    /// remove selection rectangle
    void removeSelection();
    /// add selection rectangle on full row (one line = one node (vertex))
    void addSelection(int y);
    /// add selection rectangle on full row (one item = one edge)
    void addSelection(int x, int y);
    /// clear the whole view
    void clear();
    inline QPointF maptToViewScene(QPoint pt) { return View->mapToScene(pt); }
    inline bool viewContains(QPointF pt) { return View->sceneRect().contains(pt); }

protected slots:
    void onMouseMoved( QMouseEvent *ev) { mouseMoved(ev); }
    void onMouseReleased( QMouseEvent *ev) { qDebug("released"); mouseReleased(ev); }

private slots:
    /// adjust displayed network according to selected brightness
    void onBrightnessSliderMove(int);
};

#endif // CONNECTOMEVIEW_H
