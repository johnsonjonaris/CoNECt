#include "roiitem.h"

roiItem::roiItem(quint16 id)
{
    id_n = id;
    setZValue(3);
}

void roiItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    QPen pen = painter->pen();
    pen.setColor(type == AND_ROI?Qt::white:Qt::yellow);
    pen.setWidth(0);
    painter->setPen(pen);
    if (shape == FREEHAND)
        painter->drawPolygon(poly);
    else if (shape == RECT)
        painter->drawRect(rect);
    else if (shape == OVAL)
        painter->drawEllipse(rect);

//    QFont font("Times", 1, QFont::Bold);
//    QPointF center = boundingRect().center();
//    QString id = QString::number(id_n);
//    painter->setFont(font);
//    qDebug()<<option->fontMetrics.boundingRect(id);
//    qDebug()<<center;
//    center.setX(center.x() - (option->fontMetrics.width(id) / 2));
//    center.setY(center.y() - (option->fontMetrics.height() / 2));
//    qDebug()<<center;
//    qDebug()<<boundingRect();
//    QRectF r(center.x()-1,center.y()+1,2,2);
//    painter->drawText(r,Qt::AlignCenter,id);
//    QRectF rr = option->fontMetrics.boundingRect(id);
//    rr.translate(center);
//    painter->drawRect(r);

//    QGraphicsTextItem  *txtItem = new
//            QGraphicsTextItem(id,this);
//    txtItem->setZValue(3);
//    txtItem->setFont(QFont("Times", 1, QFont::Bold));
//    txtItem->setDefaultTextColor(pen.color());
//    QRectF r = txtItem->boundingRect().translated(center);
//    center.setX(center.x() - r.width()/2);
//    center.setY(center.y() - r.height()/2);
//    txtItem->setPos(center);

//    painter->drawRect(txtItem->boundingRect().translated(center));


}

QRectF roiItem::boundingRect() const
{
    if (shape == FREEHAND)
        return poly.boundingRect();
    else if (shape == RECT || shape == OVAL)
        return rect;
}
