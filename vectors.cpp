#include "vectors.h"

/*
    This class draws a set of intersected lines inside a square.
    The lines, the background color, and the position of the item
    should be passed to the constructor.
    The item prevent the drawing of the lines if the level of details
    (zoom) is not high enough.
  */
Vectors::Vectors(int x, int y, QVarLengthArray<QLineF> lines)
{
    this->x = x;
    this->y = y;
    this->setPos(x,y);
    this->lines = lines;
    setZValue(1);
}

void Vectors::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    // don't draw vectors if too small
    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    if (lod < 5)
        return;
    // Draw lines
    QPen pen = painter->pen();
    pen.setWidth(0);
    painter->setPen(pen);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->drawLines(lines.data(), lines.size());
}
