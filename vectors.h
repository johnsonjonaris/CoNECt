#ifndef VECTORS_H
#define VECTORS_H

#include <QtGui>
#include <QGraphicsItem>
#include <QVarLengthArray>

/**
  * \class Vectors

  *  This class draws a set of intersected lines inside a square.
  *  The lines, the background color, and the position of the item
  *  should be passed to the constructor.
  *  The item prevent the drawing of the lines if the level of details
  *  (zoom) is not high enough.
  */

class Vectors : public QGraphicsItem
{
public:
    Vectors(int x, int y, QVarLengthArray<QLineF> lines);
    /// return the bouding rectangle of the vector item
    QRectF boundingRect() const { return QRectF(0, 0, 1, 1); }
    /// paint a set of lines on top of each other
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

private:
    int x, y;                           ///< position
    QVarLengthArray<QLineF> lines;      ///< the lines
};

#endif
