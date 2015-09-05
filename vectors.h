#ifndef VECTORS_H
#define VECTORS_H

#include <QtGui>
#include <QGraphicsItem>
#include <QVarLengthArray>

class Vectors : public QGraphicsItem
{
public:
    Vectors(int x, int y, QVarLengthArray<QLineF> lines);

    QRectF boundingRect() const { return QRectF(0, 0, 1, 1); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

private:
    int x, y;                           // position
    QVarLengthArray<QLineF> lines;      // the lines
};

#endif
