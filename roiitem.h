#ifndef ROIITEM_H
#define ROIITEM_H

#include <QGraphicsItem>
#include <QtGui>
#include "datatypes.h"

/**
  * \class roiItem
  *
  * region of interest graphic item.
  */
class roiItem : public QGraphicsItem
{
    quint16 id;         ///< identification number
    QPolygonF poly;     ///< polygon
    QRectF rect;        ///< rectangle
    ROIType type;       ///< region of interest type
    ROIShape shape;     ///< region of interest shape

public:

    roiItem(quint16 id): id(id) { setZValue(3); }
    void setInfo(ROIType t, ROIShape s, QPolygonF p) { type = t; shape = s; poly = p; }
    void setInfo(ROIType t, ROIShape s, QRectF r) { type = t; shape = s; rect = r; }

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);
};

#endif // ROIITEM_H
