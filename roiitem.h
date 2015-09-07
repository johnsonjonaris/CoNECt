#ifndef ROIITEM_H
#define ROIITEM_H

#include <QGraphicsItem>
#include <QtGui>
#include "datatypes.h"

class roiItem : public QGraphicsItem
{
public:

    roiItem(quint16 id);
    void setInfo(ROIType t, ROIShape s, QPolygonF p)
    { type = t; shape = s; poly = p; }
    void setInfo(ROIType t, ROIShape s, QRectF r)
    { type = t; shape = s; rect = r; }

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);



private:
    quint16 id_n;
    QPolygonF poly;
    QRectF rect;
    ROIType type;
    ROIShape shape;

};

#endif // ROIITEM_H
