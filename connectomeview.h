#ifndef CONNECTOMEVIEW_H
#define CONNECTOMEVIEW_H

#include <QWidget>
#include "armadillo"
#include "cview.h"
#include "misc_functions.h"
#include "ui_ConnectomeView.h"

#include <QDebug>

using namespace arma;

class ConnectomeView : public QWidget, private Ui::ConnectomeView
{
    Q_OBJECT
    QGraphicsRectItem *selection;
    QGraphicsPixmapItem *nwPixmapItem;
    uchar_mat nwImg;

signals:
    void mouseMoved( QMouseEvent *ev );
    void mouseReleased( QMouseEvent *ev );

public:

    CView *View;

    ConnectomeView(QWidget *parent = 0);
    ~ConnectomeView()
    {
        // we don't have to delete the QGraphicItems since
        // nwPixmapItem is deleted when deleting the scene
        // and selection is either deleted when removing the
        // selection or when deleting the scene
        qDeleteAll(this->children());
    }
    void plotConnectome(const mat &NW);
    void removeSelection();
    void addSelection(int y);           // one line = one node (vertex)
    void addSelection(int x,int y);     // one item = one edge
    void clear();
    inline QPointF maptToViewScene(QPoint pt) { return View->mapToScene(pt);}
    inline bool viewContains(QPointF pt) { return View->sceneRect().contains(pt);}

protected slots:
    void viewMouseMoved( QMouseEvent *ev) { mouseMoved(ev);}
    void viewMouseReleased( QMouseEvent *ev) {mouseReleased(ev);}
private slots:
    void onBrightnessSliderMove(int);
};

#endif // CONNECTOMEVIEW_H
